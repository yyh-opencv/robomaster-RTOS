#ifndef __RC_CONTROL_H__
#define __RC_CONTROL_H__

#include "main.h"
#include "stm32f4xx_hal.h"

#define RC_sent_X  2.5   //�����ȣ�ң������ҡ��ֵ����ת����һ��Ϊ ��660��������ڸ���ֵ�и��߱��ʵ�Ҫ�󣬿���ͨ���˴��Ŵ�
#define RC_sent_Y 2.5
#define RC_sent_Z 2.5 
#define RC_sent_pitch 2

#define RC_Wheel 2.5

#define Signal_normal 1     //ң���ź�ͨѶ�����־����ͨѶ����ʱ����־λ�� 1 �������ж��г������ӣ�ֱ���ﵽһ����С���û�����ֹͣ

extern uint8_t controller_data[36];
extern uint16_t RC_Communication;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_rx;



typedef	struct
	{ 

	  int16_t          x;
		int16_t          y;
		int16_t          z;
		int16_t      pitch;
		
		unsigned char s1;
		unsigned char s2;
	}RC_control;
	
typedef	struct 
	{
		unsigned short x;
		unsigned short y;
		unsigned short z;
		unsigned char L;
		unsigned char R;
		short Wheel;
	}Mouse_control;
	
typedef	struct
	{
		unsigned short Board; //�޷��Ŷ�����
		unsigned short W;
		unsigned short S;
		unsigned short A;
		unsigned short D;
		unsigned short SHIFT;
		unsigned short CTRL;
		unsigned short Q;
		unsigned short E;
		unsigned short R;
		unsigned short F;
		unsigned short G;
		unsigned short Z;
		unsigned short X;
		unsigned short C;
		unsigned short V;
		unsigned short B;
		
	}Key_control;
	
extern  RC_control     RC;   							//����ң�������ݽṹ��
extern Mouse_control  Mouse;
extern Key_control    Key;
void control_Test(void *prames);
	
void RC_Init(void);
void Controller_handler(void);
void IDLE_Handler(void);

#endif
