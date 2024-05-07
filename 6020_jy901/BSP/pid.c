#include "pid.h"
 #include "yaw_task.h"
 
extern struct jy901_chaiess_ir jy901_ir;

 
//*底盘电机增量式pid运算*//
//*形参输入，第一个为Error类型的结构体，第二个为电机反馈的实时速度，第三个为目标速度，后边跟着三个调参系数*//
int16_t PID_chassis(Error *chas_error,int16_t speed_now,int16_t speed_exp,float kp,float ki,float kd) 
{
	chas_error->Now =speed_exp - speed_now;
	
	chas_error->Now_out = kp * (chas_error->Now - chas_error->Last ) //kp乘(本次误差 - 上次误差）
	                      + ki * chas_error->Now  //ki乘本次误差
	                      + kd * (chas_error->Now - 2*chas_error->Last+chas_error->earlier)//kd乘（本次误差-2*上次误差+上上次误差）
	                      +chas_error->Last_out;

	if(chas_error->Now_out>=10000)//电机电流最多只能 -16384~+16384 详见"C620无刷电机调速器使用说明P32"
	{
		chas_error->Now_out =10000;  //pid输出如果超过这个值将超出电机实际输出电流，称为数据溢出，故在此做判断对其限幅
	}
	if(chas_error->Now_out<=-10000)
	{
		chas_error->Now_out= -10000;
	}	
	chas_error->Last_out = chas_error->Now_out; //将本次输出赋值给“上次输出”，留到下次pid运算使用
	chas_error->earlier  = chas_error->Last;    //将上次误差赋值给“上上次误差”，留到下次pid运算使用
	chas_error->Last     = chas_error->Now;     //将本次误差赋值给“上次误差”
	
	return chas_error->Now_out; //返回pid运算的值，供can通讯发送电流函数使用
}

//位置式云台速度环：


//云台电机角度环：
int16_t Balance_angle(Angle *angle,int16_t angle_back)
{
	static int16_t D_Bias,Last_Bias;
	angle->error=angle->balance_angle-angle_back;
	D_Bias=angle->error-Last_Bias;
	angle->back_pwm=-(angle->KP_angle)*angle->error-angle->KD_angle*D_Bias;
	Last_Bias=angle->error;//保持上一次的偏差
  return angle->back_pwm;
}

//jy901云台电机角度环

int16_t  AngleControl(jy901_Angle *angle) 
{
	
	angle->err_angle=angle->turn_angle- jy901_ir.angle[2];
	//角度差值归一处理
	if(angle->err_angle > 180) angle->err_angle = angle->err_angle - 360.0;
	else if(angle->err_angle < -180) angle->err_angle = 360.0 + angle->err_angle;

	angle->AngleControlOut = -(angle->err_angle * angle->P_angle) +jy901_ir.w[2] * angle->D_angle;
	return angle->AngleControlOut;

}
