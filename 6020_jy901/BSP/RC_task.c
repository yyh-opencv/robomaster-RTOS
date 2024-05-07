#include "RC_control.h"
#include "RC_task.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h" 

//static uint16_t RC_Communication;

extern SemaphoreHandle_t SERC_control; // Ò£¿ØÆ÷ĞÅºÅÁ¿
extern RC_control     RC;   //Ò£¿ØÆ÷ËùÓĞÒ¡¸ËµÄÊı	
//Ò£¿ØÆ÷¶ÓÁĞ
QueueHandle_t g_xQueuechasiss;

//¶ÓÁĞ´´½¨³õÊ¼»¯£
void QueueCreat_Init(void)
{
	g_xQueuechasiss=xQueueCreate(RC_chasiess_Len,sizeof(struct RC_chasiess_sent ));
}






/***************Ò£¿ØÆ÷ÈÎÎñ****************/
void control_Test(void *prames)
{
	static struct RC_chasiess_sent sent;
	while(1)
	{
		if(pdTRUE == xSemaphoreTake(SERC_control,portMAX_DELAY))//µÈ´ı¶şÖµĞÅºÅÁ¿  ÓÀ¾ÃµÈ´ı
			{ 
				
				
				Controller_handler();//Ò£¿ØÊı¾İÆ´½Ó´¦Àí
				sent.s1=RC.s1;
				sent.s2=RC.s2;
				sent.x=RC.x;
				sent.y=RC.y;
				sent.z=RC.z;
				xQueueSend(g_xQueuechasiss,&sent,0);
				vTaskDelay(20);//FreeRTOSµÄÑÓÊ±ÊÇÒ»ÖÖ×èÈûÌ¬²¢²»»áÕ¼ÓÃÏµÍ³×ÊÔ´
				/*******/
			}

	}
}








