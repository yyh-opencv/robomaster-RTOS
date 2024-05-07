#include "main.h"
#include "stm32f4xx_hal.h"


//in the beginning of task ,wait a time
//任务开始空闲一段时间
#define CHASSIS_TASK_INIT_TIME 357


struct	chaiess_ir
{ 

	  
	uint8_t s1;
	uint8_t s2;
	int16_t x;
	int16_t y;
	int16_t z;
};


void chaiess_task(void *parmes);
