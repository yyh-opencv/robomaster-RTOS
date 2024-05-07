#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h" 
#include "queue.h"
#include "chaiess.h"
#include "RC_task.h"
#include "chong_can.h"
#include "pid.h"
#include "driver_uart.h"
#include "yaw_task.h"
#include "RC_control.h"



extern Mouse_control  Mouse;// 鼠标的数据
extern QueueHandle_t g_xQueuejy901;
Angle angle_yaw={0.5,1,3300,0,0};
jy901_Angle angle_jy901={4,0.5,0,0,0};
struct jy901_chaiess_ir jy901_ir;



void yaw_task(void *parems)
{
	
	static int16_t out,out2;
	static int16_t out3,out4;
	static Error yaw;
	
	
	
	vTaskDelay(YAW_TASK_INIT_TIME);
	
	while(1)
	{
		if(RC.s1==3)//普通模式
		{
			angle_yaw.balance_angle=YAW_Middle+Mouse.Wheel;
			out=Balance_angle(&angle_yaw,Moto_yaw.angle);
			out2=PID_chassis(&yaw,Moto_yaw.speed,out,30,5,0);//串级PID电流环+角度环;
			Can_SendMoto(-out2,0x205);
		}
		else if(RC.s1==1)//云台跟随模式
		{
			angle_jy901.turn_angle=(Mouse.Wheel/15);
			xQueueReceive( g_xQueuejy901,&jy901_ir,0);
			out3=AngleControl(&angle_jy901);
			out4=PID_chassis(&yaw,Moto_yaw.speed,out3,30,5,0);//串级PID电流环+jy901角度环;
			Can_SendMoto(-out4,0x205);
		}
		else if(RC.s1==2)//小陀螺
		{
//			angle_jy901.turn_angle=(Mouse.Wheel/15);
//			xQueueReceive( g_xQueuejy901,&jy901_ir,0);
//			out3=AngleControl(&angle_jy901);
//			out4=PID_chassis(&yaw,Moto_yaw.speed,out3,30,5,0);//串级PID电流环+jy901角度环;
//			Can_SendMoto(-out4,0x205);
		}
		
	
		vTaskDelay(8);
	}
	
}

