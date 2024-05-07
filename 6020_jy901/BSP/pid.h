#ifndef __PID_H__
#define __PID_H__

#include "main.h"
#include "stm32f4xx_hal.h"


typedef struct
{
	int16_t Now;//�������
	int16_t Last;//�ϴ����
	int16_t earlier;//���ϴ����
	int16_t Last_out; //�ϴε����
	int16_t Now_out; //�������
	
}Error;  //pid�������ṹ�壬�ɴ�Ų�ͬ�����pid�������ݣ��ﵽһ��pid������������ʹ�õ�Ч��

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
