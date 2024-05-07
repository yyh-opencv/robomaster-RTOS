#include "main.h"
#include "stm32f4xx_hal.h"

#define YAW_TASK_INIT_TIME 365
#define YAW_Middle 3300

struct	jy901_chaiess_ir
{ 

	  
	float angle[3];
	float w[3];
};
	
void yaw_task(void *parems);
extern struct jy901_chaiess_ir jy901_ir;

