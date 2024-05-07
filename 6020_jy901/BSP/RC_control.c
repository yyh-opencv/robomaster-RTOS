//#include "usart.h"
#include "RC_control.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h" 
#include "jy901.h"
static uint8_t  controller_data[36];//����һ���������飬��������18֡�����ݣ�ͬʱ����һ���ֿռ��ֹ���ݹ���

extern DMA_HandleTypeDef hdma_usart1_rx;
SemaphoreHandle_t SERC_control;


//**����ң�������ݽṹ��**//
RC_control     RC;   //ң��������ҡ�˵�����							
Mouse_control  Mouse;// ��������

static Key_control    Key;  // ���м��̰�ť������

//**ң������ʼ������**//
void RC_Init(void)
{
	
	__HAL_UART_CLEAR_IDLEFLAG(&huart3);//���UART�����жϱ�־λ
	__HAL_UART_ENABLE_IT(&huart3,UART_IT_IDLE);//ʹ�ܿ����ж�IDLE
  HAL_UART_Receive_DMA(&huart3,controller_data,36);
  SERC_control=xSemaphoreCreateBinary();	
//	

}


//**ң�����ݴ�����**//
void Controller_handler(void)
{
  static unsigned short ch[4];
	
	//*ң����ҡ������*//
	ch[0] = ((int16_t)controller_data[0] | ((int16_t)controller_data[1] << 8)) & 0x07FF;
	ch[1] = (((int16_t)controller_data[1] >> 3) | ((int16_t)controller_data[2] << 5)) & 0x07FF;
	ch[2] = (((int16_t)controller_data[2] >> 6) | ((int16_t)controller_data[3] << 2) | ((int16_t)controller_data[4] << 10)) & 0x07FF;
	ch[3] = (((int16_t)controller_data[4] >> 1) | ((int16_t)controller_data[5]<<7)) & 0x07FF;
	RC.x     = ( ch[2]-1024 ) * RC_sent_X;
	RC.y     = ( ch[3]-1024 ) * RC_sent_Y;
	RC.z     = ( ch[0]-1024 ) * RC_sent_Z;
	RC.pitch = ( ch[1]-1024 ) * RC_sent_pitch;
	
	//*ң����ͨ����������*//
	RC.s1 = ((controller_data[5] >> 4) & 0x000C) >> 2;
	RC.s2 = ((controller_data[5] >> 4) & 0x0003);
	
	//*���̰�������*//
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
	
	
	//*���λ������*//
	Mouse.x = ((int16_t)controller_data[6]) | ((int16_t)controller_data[7] << 8);
	Mouse.y = ((int16_t)controller_data[8]) | ((int16_t)controller_data[9] << 8);
	Mouse.z = ((int16_t)controller_data[10]) | ((int16_t)controller_data[11] << 8);
	Mouse.L = controller_data[12];
	Mouse.R = controller_data[13];
		
	Mouse.Wheel = -(((int16_t)controller_data[16] | (int16_t)controller_data[17] << 8)-1024) * RC_Wheel;	
	
}



//**DMA�����ж�У�����ݺ���**//


void IDLE_Handler(void)
{
		static uint16_t jy901_Date_have,jy901_Data_exist;
    static uint32_t Data_lave,Data_exist; //ʣ��ռ䣬�ѽ��յ��ֽ���	
		if((__HAL_UART_GET_FLAG(&huart3,UART_FLAG_IDLE)!= RESET))  //�ж��Ƿ��������ж�
		{
			volatile BaseType_t xHigherPriorityTaskWoken = pdFALSE;
			__HAL_UART_CLEAR_IDLEFLAG(&huart3);//���UART�����жϱ�־λ
			
			HAL_UART_DMAStop(&huart3); //�ر�DMA�����ܵ�����
			
			Data_lave = __HAL_DMA_GET_COUNTER(&hdma_usart3_rx); //��ȡ��������ж�ʱ��DMA��ʣ�Ŀռ��ֽ������ܵĿռ���36���ֽ�

			Data_exist = 36-Data_lave;  //�ܵĿռ��ֽ��� - ʣ��Ŀռ��ֽ��� = �Ѿ����յ��ֽ��� 

			if(Data_exist == 18)  // �������DMA������յ�18���ֽڵ����ݣ������������ȷ�ģ��������ݴ�����
			{
				
				xSemaphoreGiveFromISR(SERC_control,NULL);
				
			}
			
			
			HAL_UART_Receive_DMA(&huart3,controller_data,36); //���´�DMA�Ľ��չ���
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
		
		else if((__HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE)!= RESET))  //�ж��Ƿ��������ж�
		{
			volatile BaseType_t xHigherPriorityTaskWoken = pdFALSE;
			__HAL_UART_CLEAR_IDLEFLAG(&huart1);//���UART�����жϱ�־λ
			HAL_UART_DMAStop(&huart1); //�ر�DMA�����ܵ�����
			
			jy901_Date_have = __HAL_DMA_GET_COUNTER(&hdma_usart1_rx); //��ȡ��������ж�ʱ��DMA��ʣ�Ŀռ��ֽ������ܵĿռ���36���ֽ�
			jy901_Data_exist = 36-jy901_Date_have;  //�ܵĿռ��ֽ��� - ʣ��Ŀռ��ֽ��� = �Ѿ����յ��ֽ��� 
			if(jy901_Data_exist == 33) 
			{
				if(RC.s1==1)//
				{
					xSemaphoreGiveFromISR(jy901_control,&xHigherPriorityTaskWoken);
				}
			}
			HAL_UART_Receive_DMA(&huart1,JY901_data.RxBuffer,36); //���´�DMA�Ľ��չ���
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
}







