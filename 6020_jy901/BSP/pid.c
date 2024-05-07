#include "pid.h"
 #include "yaw_task.h"
 
extern struct jy901_chaiess_ir jy901_ir;

 
//*���̵������ʽpid����*//
//*�β����룬��һ��ΪError���͵Ľṹ�壬�ڶ���Ϊ���������ʵʱ�ٶȣ�������ΪĿ���ٶȣ���߸�����������ϵ��*//
int16_t PID_chassis(Error *chas_error,int16_t speed_now,int16_t speed_exp,float kp,float ki,float kd) 
{
	chas_error->Now =speed_exp - speed_now;
	
	chas_error->Now_out = kp * (chas_error->Now - chas_error->Last ) //kp��(������� - �ϴ���
	                      + ki * chas_error->Now  //ki�˱������
	                      + kd * (chas_error->Now - 2*chas_error->Last+chas_error->earlier)//kd�ˣ��������-2*�ϴ����+���ϴ���
	                      +chas_error->Last_out;

	if(chas_error->Now_out>=10000)//����������ֻ�� -16384~+16384 ���"C620��ˢ���������ʹ��˵��P32"
	{
		chas_error->Now_out =10000;  //pid�������������ֵ���������ʵ�������������Ϊ������������ڴ����ж϶����޷�
	}
	if(chas_error->Now_out<=-10000)
	{
		chas_error->Now_out= -10000;
	}	
	chas_error->Last_out = chas_error->Now_out; //�����������ֵ�����ϴ�������������´�pid����ʹ��
	chas_error->earlier  = chas_error->Last;    //���ϴ���ֵ�������ϴ����������´�pid����ʹ��
	chas_error->Last     = chas_error->Now;     //��������ֵ�����ϴ���
	
	return chas_error->Now_out; //����pid�����ֵ����canͨѶ���͵�������ʹ��
}

//λ��ʽ��̨�ٶȻ���


//��̨����ǶȻ���
int16_t Balance_angle(Angle *angle,int16_t angle_back)
{
	static int16_t D_Bias,Last_Bias;
	angle->error=angle->balance_angle-angle_back;
	D_Bias=angle->error-Last_Bias;
	angle->back_pwm=-(angle->KP_angle)*angle->error-angle->KD_angle*D_Bias;
	Last_Bias=angle->error;//������һ�ε�ƫ��
  return angle->back_pwm;
}

//jy901��̨����ǶȻ�

int16_t  AngleControl(jy901_Angle *angle) 
{
	
	angle->err_angle=angle->turn_angle- jy901_ir.angle[2];
	//�ǶȲ�ֵ��һ����
	if(angle->err_angle > 180) angle->err_angle = angle->err_angle - 360.0;
	else if(angle->err_angle < -180) angle->err_angle = 360.0 + angle->err_angle;

	angle->AngleControlOut = -(angle->err_angle * angle->P_angle) +jy901_ir.w[2] * angle->D_angle;
	return angle->AngleControlOut;

}
