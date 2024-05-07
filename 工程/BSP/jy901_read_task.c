#include "RC_control.h"
#include "RC_task.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h" 
#include "jy901.h"
#include "jy901_read_task.h"

//static uint16_t RC_Communication;


//ң��������
QueueHandle_t g_xQueuejy901;

//���д�����ʼ��?
void jy901_QueueCreat_Init(void)
{
	g_xQueuejy901=xQueueCreate(jy901_chasiess_Len,sizeof(struct jy901_chasiess_sent ));
}






/***************ң��������****************/
void jy901_read_Test(void *prames)
{
	static struct jy901_chasiess_sent sent;
	while(1)
	{
		if(pdTRUE == xSemaphoreTake(jy901_control,portMAX_DELAY))//�ȴ���ֵ�ź���  ���õȴ�
			{ 
				JY901_Process();
				sent.angle[0]=JY901_data.angle.angle[0];
				sent.angle[1]=JY901_data.angle.angle[1];
				sent.angle[2]=JY901_data.angle.angle[2];
				sent.w[0]=JY901_data.w.w[0];
				sent.w[1]=JY901_data.w.w[1];
				sent.w[2]=JY901_data.w.w[2];
				xQueueSend(g_xQueuejy901,&sent,0);
				vTaskDelay(20);//FreeRTOS����ʱ��һ������̬������ռ��ϵͳ��Դ
				/*******/
			}

	}
}








