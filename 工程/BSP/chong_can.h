#ifndef __CHONG_CAN_H__

#define __CHONG_CAN_H__

#include "main.h"
#include "stm32f4xx_hal.h"

#define MOTO_ID_1 0x201 
#define MOTO_ID_2 0x202
#define MOTO_ID_3 0x203
#define MOTO_ID_4 0x204
#define MOTO_ID_5 0x205
#define MOTO_ID_6 0x206
#define MOTO_ID_7 0x207
#define MOTO_ID_8 0x208

typedef struct
{
	int16_t angle;     //
	int16_t speed;     //
	int16_t current;   //
	int16_t  celsius;   //
}MotoData;
extern MotoData Moto_yaw;
extern MotoData Moto_Arm_1;
extern MotoData Moto_Arm_2;
//extern uint16_t R[0];
extern uint8_t CAN_TxData[8],CAN_RxData[8];///存放发送的数据和接收的数据
extern uint32_t TxMailbox;					//用于指示CAN消息发送函数HAL_CAN_AddTxMessage使用了哪个邮箱来发送数据

extern CAN_TxHeaderTypeDef Tx1Message;
extern CAN_RxHeaderTypeDef Rx1Message;
extern CAN_HandleTypeDef hcan1;
extern MotoData Moto_1;//五号电机的数据结构体 
extern MotoData Moto_2;//五号电机的数据结构体
extern MotoData Moto_3;
extern MotoData Moto_4;

void CanFilter_Init(void);//CAN_HandleTypeDef *hcan
void Can_Tx1MesInit(CAN_TxHeaderTypeDef *Tx1Mes);
void Can_Rx1MesInit(CAN_RxHeaderTypeDef *Rx1Mes);
void Can_Init(void);
uint8_t Can_SendMoto(int16_t Current,uint16_t ID);///第一个数据输入电流，第二个数据输入电机电调闪烁的ID号
void Can_SendMoto_Chassis(int16_t Current1,int16_t Current2,int16_t Current3,int16_t Current4);
#endif
