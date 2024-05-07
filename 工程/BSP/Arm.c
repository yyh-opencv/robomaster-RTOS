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
#include "Arm.h"
#include "RC_control.h"
int16_t change=410;

int16_t arm_speed_out, arm_speed_out2;

//float p,i;
float arm_angle_out2, arm_angle_out;

//float p,i;
Error Arm1;
Error Arm2;
Angle angle_Arm1={0.1,0.0001,4781,0,0};
Angle angle_Arm2={0.1,0.0001,410,0,0};


/*过零点保护机制,目的是消除电机的过零点偏差大，导致的暴走*/
static void trans_point(Angle  *tar,MotoData *cur)
{

	if(tar->balance_angle - cur->angle > 4096)    //4096 ：半圈机械角度
	{
		 cur->angle += 8192;        //8191,8192无所谓了，四舍五入
	}
	if(tar->balance_angle - cur->angle <- 4096)
	{
		 cur->angle =  cur->angle - 8192;
	}
	else
	{
		//*cur = *cur;
		// do nothing
	}
	
}



void Arm_task(void *parems)
{
	vTaskDelay(316);
	while(1)
	{
		if(RC.pitch>600)
		{
			angle_Arm1.balance_angle+=change;
			angle_Arm2.balance_angle-=change;
			if(angle_Arm1.balance_angle>8192)
			{
				angle_Arm1.balance_angle-=8192;
			}
			if(angle_Arm2.balance_angle<0)
			{
				angle_Arm2.balance_angle+=8192;
			}
		}
		else if(RC.pitch<-600)
		{
			angle_Arm1.balance_angle-=change;
			angle_Arm2.balance_angle+=change;
			if(angle_Arm1.balance_angle<0)
			{
				angle_Arm1.balance_angle+=8192;
			}
			if(angle_Arm2.balance_angle>8192)
			{
				angle_Arm2.balance_angle-=8192;
			}
			
		}
		trans_point(&angle_Arm1,&Moto_Arm_1);
		trans_point(&angle_Arm2,&Moto_Arm_2);
		
		arm_angle_out=-Balance_angle(&angle_Arm1,Moto_Arm_1.angle);
		arm_angle_out2=-Balance_angle(&angle_Arm2,Moto_Arm_2.angle);
		arm_speed_out = PID_arm(&Arm1,Moto_Arm_1.speed,arm_angle_out,8,4.5);	
		arm_speed_out2 = PID_arm(&Arm2,Moto_Arm_2.speed,arm_angle_out2,8,4.5);	
		
		Can_SendMoto(arm_speed_out,0X205);
		Can_SendMoto(arm_speed_out2,0X206);
//		printf("%d,%d\n\r",(Moto_Arm_1.angle-Moto_Arm_2.angle),angle_Arm2.balance_angle);
		vTaskDelay(10);
	}
}
