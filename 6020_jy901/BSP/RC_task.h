#include "main.h"
#include "stm32f4xx_hal.h"


#define RC_chasiess_Len 20

struct RC_chasiess_sent{
	uint8_t s1;
	uint8_t s2;
	int16_t x;
	int16_t y;
	int16_t z;
};

void QueueCreat_Init(void);
void control_Test(void *prames);

