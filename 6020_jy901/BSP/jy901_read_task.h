#include "main.h"
#include "stm32f4xx_hal.h"


#define jy901_chasiess_Len 10

struct jy901_chasiess_sent{
		float angle[3];
		float w[3];
};

void jy901_QueueCreat_Init(void);
void jy901_read_Test(void *prames);

