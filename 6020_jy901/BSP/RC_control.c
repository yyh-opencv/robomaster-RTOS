//#include "usart.h"
#include "RC_control.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h" 
#include "jy901.h"
static uint8_t  controller_data[36];//定义一个缓冲数组，用来接收18帧的数据，同时留下一部分空间防止数据过冲

extern DMA_HandleTypeDef hdma_usart1_rx;
SemaphoreHandle_t SERC_control;


//**声明遥控器数据结构体**//
RC_control     RC;   //遥控器所有摇杆的数据							
Mouse_control  Mouse;// 鼠标的数据

static Key_control    Key;  // 所有键盘按钮的数据

//**遥控器初始化函数**//
void RC_Init(void)
{
	
	__HAL_UART_CLEAR_IDLEFLAG(&huart3);//清除UART空闲中断标志位
	__HAL_UART_ENABLE_IT(&huart3,UART_IT_IDLE);//使能空闲中断IDLE
  HAL_UART_Receive_DMA(&huart3,controller_data,36);
  SERC_control=xSemaphoreCreateBinary();	
//	

}


//**遥控数据处理函数**//
void Controller_handler(void)
{
  static unsigned short ch[4];
	
	//*遥控器摇杆数据*//
	ch[0] = ((int16_t)controller_data[0] | ((int16_t)controller_data[1] << 8)) & 0x07FF;
	ch[1] = (((int16_t)controller_data[1] >> 3) | ((int16_t)controller_data[2] << 5)) & 0x07FF;
	ch[2] = (((int16_t)controller_data[2] >> 6) | ((int16_t)controller_data[3] << 2) | ((int16_t)controller_data[4] << 10)) & 0x07FF;
	ch[3] = (((int16_t)controller_data[4] >> 1) | ((int16_t)controller_data[5]<<7)) & 0x07FF;
	RC.x     = ( ch[2]-1024 ) * RC_sent_X;
	RC.y     = ( ch[3]-1024 ) * RC_sent_Y;
	RC.z     = ( ch[0]-1024 ) * RC_sent_Z;
	RC.pitch = ( ch[1]-1024 ) * RC_sent_pitch;
	
	//*遥控器通道拨杆数据*//
	RC.s1 = ((controller_data[5] >> 4) & 0x000C) >> 2;
	RC.s2 = ((controller_data[5] >> 4) & 0x0003);
	
	//*键盘按键数据*//
	Key.Board =  ((int16_t)controller_data[14]) | ((int16_t)controller_data[15]<<8);
	Key.W = ((Key.Board>>0)&0x01);
	Key.S = ((Key.Board>>1)&0x01);
	Key.A = ((Key.Board>>2)&0x01);
	Key.D = ((Key.Board>>3)&0x01);
	Key.SHIFT = ((Key.Board>>4)&0x01);
	Key.CTRL  = ((Key.Board>>5)&0x01);
	Key.Q = ((Key.Board>>6)&0x01);
	Key.E = ((Key.Board>>7)&0x01);
	Key.R = ((Key.Board>>8)&0x01);
	Key.F = ((Key.Board>>9)&0x01);
	Key.G = ((Key.Board>>10)&0x01);
	Key.Z = ((Key.Board>>11)&0x01);
	Key.X = ((Key.Board>>12)&0x01);
	Key.C = ((Key.Board>>13)&0x01);
	Key.V = ((Key.Board>>14)&0x01);
	Key.B = ((Key.Board>>15)&0x01);
	
	
	//*鼠标位移数据*//
	Mouse.x = ((int16_t)controller_data[6]) | ((int16_t)controller_data[7] << 8);
	Mouse.y = ((int16_t)controller_data[8]) | ((int16_t)controller_data[9] << 8);
	Mouse.z = ((int16_t)controller_data[10]) | ((int16_t)controller_data[11] << 8);
	Mouse.L = controller_data[12];
	Mouse.R = controller_data[13];
		
	Mouse.Wheel = -(((int16_t)controller_data[16] | (int16_t)controller_data[17] << 8)-1024) * RC_Wheel;	
	
}



//**DMA空闲中断校验数据函数**//


void IDLE_Handler(void)
{
		static uint16_t jy901_Date_have,jy901_Data_exist;
    static uint32_t Data_lave,Data_exist; //剩余空间，已接收的字节数	
		if((__HAL_UART_GET_FLAG(&huart3,UART_FLAG_IDLE)!= RESET))  //判断是否进入空闲中断
		{
			volatile BaseType_t xHigherPriorityTaskWoken = pdFALSE;
			__HAL_UART_CLEAR_IDLEFLAG(&huart3);//清除UART空闲中断标志位
			
			HAL_UART_DMAStop(&huart3); //关闭DMA避免受到干扰
			
			Data_lave = __HAL_DMA_GET_COUNTER(&hdma_usart3_rx); //获取进入空闲中断时，DMA所剩的空间字节数，总的空间是36个字节

			Data_exist = 36-Data_lave;  //总的空间字节数 - 剩余的空间字节数 = 已经接收的字节数 

			if(Data_exist == 18)  // 如果本次DMA传输接收到18个字节的数据，则该数据是正确的，进入数据处理函数
			{
				
				xSemaphoreGiveFromISR(SERC_control,NULL);
				
			}
			
			
			HAL_UART_Receive_DMA(&huart3,controller_data,36); //重新打开DMA的接收功能
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
		
		else if((__HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE)!= RESET))  //判断是否进入空闲中断
		{
			volatile BaseType_t xHigherPriorityTaskWoken = pdFALSE;
			__HAL_UART_CLEAR_IDLEFLAG(&huart1);//清除UART空闲中断标志位
			HAL_UART_DMAStop(&huart1); //关闭DMA避免受到干扰
			
			jy901_Date_have = __HAL_DMA_GET_COUNTER(&hdma_usart1_rx); //获取进入空闲中断时，DMA所剩的空间字节数，总的空间是36个字节
			jy901_Data_exist = 36-jy901_Date_have;  //总的空间字节数 - 剩余的空间字节数 = 已经接收的字节数 
			if(jy901_Data_exist == 33) 
			{
				if(RC.s1==1)//
				{
					xSemaphoreGiveFromISR(jy901_control,&xHigherPriorityTaskWoken);
				}
			}
			HAL_UART_Receive_DMA(&huart1,JY901_data.RxBuffer,36); //重新打开DMA的接收功能
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
}







