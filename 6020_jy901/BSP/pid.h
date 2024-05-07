#ifndef __PID_H__
#define __PID_H__

#include "main.h"
#include "stm32f4xx_hal.h"


typedef struct
{
	int16_t Now;//本次误差
	int16_t Last;//上次误差
	int16_t earlier;//上上次误差
	int16_t Last_out; //上次的输出
	int16_t Now_out; //本次输出
	
}Error;  //pid运算误差结构体，可存放不同电机的pid运算数据，达到一个pid函数给多个电机使用的效果

typedef struct
{
	float KP_angle;
  float KD_angle;
  int16_t  balance_angle;
  float error;
  int16_t back_pwm;
}Angle;

typedef struct
{
	float P_angle;
  float D_angle;
  int16_t  turn_angle;
  float err_angle;
  int16_t AngleControlOut;
}jy901_Angle;

int16_t PID_chassis(Error *chas_error,int16_t speed_now,int16_t speed_exp,float kp,float ki,float kd);
int16_t Balance_angle(Angle *angle,int16_t angle_back);
int16_t  AngleControl(jy901_Angle *angle);
#endif
