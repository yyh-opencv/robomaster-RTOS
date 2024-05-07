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

extern QueueHandle_t g_xQueuechasiss;


static SemaphoreHandle_t  Semaphore_CanSent;

static Error chas1_error;  //×ó3508 pidµÄ½á¹¹Ìå Ãû³
static Error chas2_error;  //×ó3508 pidµÄ½á¹¹Ìå Ãû³ÆÆ
static Error chas3_error;  //×ó3508 pidµÄ½á¹¹Ìå Ãû?
static Error chas4_error;  //×ó3508 pidµÄ½á¹¹Ìå Ãû³Æ?

static struct chaiess_ir ir_date;

int16_t chassis_value[5]; //ÓÃÀ´´¢´æNow_outËÙ¶È·µ»ØÖµ¡£
int16_t Target_SL=0;
int16_t Target_SR=0;
int16_t Target_XL=0;
int16_t Target_XR=0;

int16_t yaw_out1,yaw_out2[5];
Angle angle_yaw_mode2={2,1,3300,0,0};


static void chassis(int16_t v_x,int16_t v_y,int16_t v_z)
{
	Target_SL=(v_y+v_x+v_z);
	Target_SR=(-v_y+v_x+v_z);
	Target_XL=(-v_y-v_x+v_z);
	Target_XR=(v_y-v_x+v_z);
}

static void limit_max(int q)
{
	if(q>6000)
	{
		q=6000;
	}
	else if(q<-6000)
		q = -6000;
}

void chaiess_task(void *parmes)
{
	//ÁÙ½ç»¥³âÁ¿,·À´òÂÒI2C
	Semaphore_CanSent=xSemaphoreCreateMutex();
	
	
	vTaskDelay(CHASSIS_TASK_INIT_TIME);
	
	
	while(1)
	{
		//¶Á¶ÓÁÐ
		xQueueReceive( g_xQueuechasiss,&ir_date, 0 );
		
		
		if(ir_date.s1==3 )
		{ 
			chassis(ir_date.x,ir_date.y,ir_date.z);
			limit_max(Target_XR);
			limit_max(Target_XL);
			limit_max(Target_SR);
			limit_max(Target_SL);
			
			xSemaphoreTake(Semaphore_CanSent, portMAX_DELAY);//»ñÈ¡ÐÅºÅÁ¿
			
			chassis_value[1] = PID_chassis(&chas1_error,Moto_1.speed,Target_SL,7,4.5,1.3);
			chassis_value[2] = PID_chassis(&chas2_error,Moto_2.speed,Target_SR,7,4.5,1.3); 	
			chassis_value[3] = PID_chassis(&chas3_error,Moto_3.speed,Target_XL,7,4.5,1.3);
			chassis_value[4] = PID_chassis(&chas4_error,Moto_4.speed,Target_XR,7,4.5,1.3);		
			Can_SendMoto_Chassis(chassis_value[1],chassis_value[2],chassis_value[3],chassis_value[4]);
			xSemaphoreGive(Semaphore_CanSent);//ÊÍ·ÅÐÅºÅÁ¿
		}
		
		else if(ir_date.s1==1) //µ×ÅÌ¸úËæÔÆÌ¨µÄ£¬µ×ÅÌ²Ù×÷
		{
			yaw_out1=-Balance_angle(&angle_yaw_mode2,Moto_yaw.angle);
			yaw_out2[1] = PID_chassis(&chas1_error,Moto_1.speed,yaw_out1,7,4.5,1.3);
			yaw_out2[2] = PID_chassis(&chas2_error,Moto_2.speed,yaw_out1,7,4.5,1.3); 	
			yaw_out2[3] = PID_chassis(&chas3_error,Moto_3.speed,yaw_out1,7,4.5,1.3);
			yaw_out2[4] = PID_chassis(&chas4_error,Moto_4.speed,yaw_out1,7,4.5,1.3);	
			Can_SendMoto_Chassis(	yaw_out2[1],	yaw_out2[2],yaw_out2[3],yaw_out2[4]);
		}
		else if(ir_date.s1==2) //·ÖÍ·ÐÐ¶¯£¬Ð¡ÍÓÂÝÄ£Ê½
		{
			chassis(ir_date.x,ir_date.y,ir_date.z);
			limit_max(Target_XR);
			limit_max(Target_XL);
			limit_max(Target_SR);
			limit_max(Target_SL);
			
			xSemaphoreTake(Semaphore_CanSent, portMAX_DELAY);//»ñÈ¡ÐÅºÅÁ¿
			
			chassis_value[1] = PID_chassis(&chas1_error,Moto_1.speed,Target_SL,7,4.5,1.3);
			chassis_value[2] = PID_chassis(&chas2_error,Moto_2.speed,Target_SR,7,4.5,1.3); 	
			chassis_value[3] = PID_chassis(&chas3_error,Moto_3.speed,Target_XL,7,4.5,1.3);
			chassis_value[4] = PID_chassis(&chas4_error,Moto_4.speed,Target_XR,7,4.5,1.3);		
			Can_SendMoto_Chassis(chassis_value[1],chassis_value[2],chassis_value[3],chassis_value[4]);
			xSemaphoreGive(Semaphore_CanSent);//ÊÍ·ÅÐÅºÅÁ¿
		}

		vTaskDelay(5);
	}
}

