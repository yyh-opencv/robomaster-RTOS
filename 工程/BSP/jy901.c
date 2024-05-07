#include "jy901.h"
#include "stdio.h"
#include "string.h"
#include "usart.h"
User_USART JY901_data;
extern UART_HandleTypeDef huart1;

//static struct SAcc 		stcAcc;
static struct SGyro 		stcGyro;
static struct SAngle 	stcAngle;

SemaphoreHandle_t jy901_control;//�ź���


//��ʼ������
void User_JY901_Init(void)
{

	__HAL_UART_CLEAR_IDLEFLAG(&huart1);//���UART�����жϱ�־λ
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);//ʹ�ܿ����ж�IDLE
  HAL_UART_Receive_DMA(&huart1,JY901_data.RxBuffer,36);	
	jy901_control=xSemaphoreCreateBinary();
}

void JY901_Process()
{

		for(uint8_t i=0;i<4;i++)
		{
				if(JY901_data.RxBuffer[i*11]!= 0x55) continue;
				switch(JY901_data.RxBuffer[i*11+1])
				{
//						case 0x51:	
//							memcpy(&stcAcc,&JY901_data.RxBuffer[2 + i*11],8);
//							for(uint8_t j = 0; j < 3; j++) JY901_data.acc.a[j] = (float)stcAcc.a[j]/32768.00*16;									//���ٶ�
//						break;
						case 0x52:	
							memcpy(&stcGyro,&JY901_data.RxBuffer[2 + i*11],8);
							for(uint8_t j = 0; j < 3; j++) JY901_data.w.w[j] = (float)stcGyro.w[j]/32768.00*2000;								//���ٶ�
						break;
						case 0x53:	
							memcpy(&stcAngle,&JY901_data.RxBuffer[2 + i*11],8);
							for(uint8_t j = 0; j < 3; j++) JY901_data.angle.angle[j] = (float)stcAngle.Angle[j]/32768.00*180;		//�Ƕ�
						break;
				}
				
		}

}



