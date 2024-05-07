#include "RC_control.h"
#include "RC_task.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h" 

//static uint16_t RC_Communication;

extern SemaphoreHandle_t SERC_control; // 遥控器信号量
extern RC_control     RC;   //遥控器所有摇杆的数	
//遥控器队列
QueueHandle_t g_xQueuechasiss;

//队列创建初始化�
void QueueCreat_Init(void)
{
	g_xQueuechasiss=xQueueCreate(RC_chasiess_Len,sizeof(struct RC_chasiess_sent ));
}






/***************遥控器任务****************/
void control_Test(void *prames)
{
	static struct RC_chasiess_sent sent;
	while(1)
	{
		if(pdTRUE == xSemaphoreTake(SERC_control,portMAX_DELAY))//等待二值信号量  永久等待
			{ 
				
				
				Controller_handler();//遥控数据拼接处理
				sent.s1=RC.s1;
				sent.s2=RC.s2;
				sent.x=RC.x;
				sent.y=RC.y;
				sent.z=RC.z;
				xQueueSend(g_xQueuechasiss,&sent,0);
				vTaskDelay(20);//FreeRTOS的延时是一种阻塞态并不会占用系统资源
				/*******/
			}

	}
}








