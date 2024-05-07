#include "RC_control.h"
#include "RC_task.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h" 

//static uint16_t RC_Communication;

extern SemaphoreHandle_t SERC_control; // ң�����ź���
extern RC_control     RC;   //ң��������ҡ�˵���	
//ң��������
QueueHandle_t g_xQueuechasiss;

//���д�����ʼ���
void QueueCreat_Init(void)
{
	g_xQueuechasiss=xQueueCreate(RC_chasiess_Len,sizeof(struct RC_chasiess_sent ));
}






/***************ң��������****************/
void control_Test(void *prames)
{
	static struct RC_chasiess_sent sent;
	while(1)
	{
		if(pdTRUE == xSemaphoreTake(SERC_control,portMAX_DELAY))//�ȴ���ֵ�ź���  ���õȴ�
			{ 
				
				
				Controller_handler();//ң������ƴ�Ӵ���
				sent.s1=RC.s1;
				sent.s2=RC.s2;
				sent.x=RC.x;
				sent.y=RC.y;
				sent.z=RC.z;
				xQueueSend(g_xQueuechasiss,&sent,0);
				vTaskDelay(20);//FreeRTOS����ʱ��һ������̬������ռ��ϵͳ��Դ
				/*******/
			}

	}
}








