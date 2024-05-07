/*写can配置，需要知道设置滤波，发送帧，接受帧，需要配置那些可以在HAL文件里面看解说，
写法大同小异*/


#include "chong_can.h"
#include "can.h"

uint8_t CAN_TxData[8],CAN_RxData[8]; //定义两个八位无符号数组来存放要发送的数据和接收到的数据
uint8_t CAN2_TxData[8],CAN2_TxData2[8],CAN2_RxData[8];
uint32_t TxMailbox = 0;

CAN_TxHeaderTypeDef Tx1Message;  //定义一个名字为Tx1Message的CAN_TxHeaderTypeDef类型的结构体，用来存放发送数据的配置
CAN_RxHeaderTypeDef Rx1Message;  //同上

CAN_TxHeaderTypeDef Tx2Message;  //定义一个名字为Tx1Message的CAN_TxHeaderTypeDef类型的结构体，用来存放发送数据的配置
CAN_RxHeaderTypeDef Rx2Message;  //同上

MotoData Moto_1;//一号电机的数据结构体 
MotoData Moto_2;
MotoData Moto_3;
MotoData Moto_4;
MotoData Moto_yaw;
MotoData Moto_Arm_1;
MotoData Moto_Arm_2;

void CanFilter_Init(void)
{
	CAN_FilterTypeDef Canfilter;  
	
	Canfilter.FilterMode = CAN_FILTERMODE_IDMASK;  //设置为掩码模式
	Canfilter.FilterScale = CAN_FILTERSCALE_32BIT;  //32位的掩码模式（除32位的掩码模式，还有16位的）
	
	Canfilter.FilterIdHigh = 0x0200<<5;//根据中文参考手册中的筛选器组寄存器构成中规定，32位筛选器组成为高11位为标准帧ID,所以将ID左移五位
	Canfilter.FilterIdLow  = 0x0000;    //0x200 = B10 0000 0000,   0x0200
	Canfilter.FilterMaskIdHigh = 0x03F0<<5;//设掩码为B11 1110 0000 留低五位供电机ID1-8任意配置  0x03F0
	Canfilter.FilterMaskIdLow  = 0x0000;  
	
	Canfilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;//过滤FIFO0邮箱
	Canfilter.FilterActivation = CAN_FILTER_ENABLE;//使能过滤器
	Canfilter.FilterBank = 0;        //使能哪个过滤器，单独用CAN1可以选0~13
	Canfilter.SlaveStartFilterBank = 14;//给can2分配过滤器，单独用CAN1可以不鸟它，也可以备着后边用
	
	HAL_CAN_ConfigFilter(&hcan1,&Canfilter);  //开启滤波，上面的代码可以理解为只是配置，这个函数是用来将上面的设置录入到寄存器的，
	                                          //你可以理解为上面是你在改参数，这里是设置按钮，点下去了你修改的值才会输入到寄存器
	
}
/**
* @brief  CAN2的滤波函数
* @param  None
* @return None
*/
void Can2Filter_Init(void)
{
	CAN_FilterTypeDef Canfilter; 
	
	Canfilter.FilterMode = CAN_FILTERMODE_IDMASK; 
	Canfilter.FilterScale = CAN_FILTERSCALE_32BIT; 
	
	Canfilter.FilterIdHigh = 0x0200<<5;
	Canfilter.FilterIdLow  = 0x0000;   
	Canfilter.FilterMaskIdHigh = 0x0000;
	Canfilter.FilterMaskIdLow  = 0x0000;  
	
	Canfilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	Canfilter.FilterActivation = CAN_FILTER_ENABLE;
	Canfilter.FilterBank = 14;        
	Canfilter.SlaveStartFilterBank = 14;
	
	HAL_CAN_ConfigFilter(&hcan2,&Canfilter);
}





/* 完整版的初始化函数，并开启相关功能 */
//*放在MX_CAN1_Init();函数下边*//
void Can_Init(void)
{
	
	CanFilter_Init();
	Can2Filter_Init();
	
	HAL_CAN_Start(&hcan1);  
	HAL_CAN_Start(&hcan2);
	
	HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);//使能中断,当FIFO0接收到数据时进入中断
	HAL_CAN_ActivateNotification(&hcan2,CAN_IT_RX_FIFO0_MSG_PENDING);//使能中断,当FIFO0接收到数据时进入中断
	
	
}
	

///** 单电机电流控制函数 控制ID在0x205-0x208的电机 **//
///*形参current为电机输出电流，形参ID为电机ID号*//
uint8_t Can_SendMoto(int16_t Current,uint16_t ID)   
{
	Tx2Message.StdId = 0x1FF; //电机发送标识符id
	Tx2Message.ExtId = 0x00;  //不采用扩展帧
	Tx2Message.IDE   = CAN_ID_STD;  //帧类型  标准帧  （我们采用的是标准帧，除此之外还有扩展帧，扩展帧的优点是ID数可以设置得更多，但是咱用不上）
	Tx2Message.RTR   = CAN_RTR_DATA;//帧格式  数据帧   （可以理解为用来发送数据的帧类型，除这个之外还有遥控帧，是啥自行查询）
	Tx2Message.DLC   = 8;          //你要发送的数据长度，或者说数据量，这里是8个字节
	Tx2Message.TransmitGlobalTime = DISABLE;  //指定是否在开始时捕获时间戳计数器值,没开也用不上，所以这里也不使能，想知道为啥不开，自己去查它是干嘛用的
	
	switch ( ID ) ///根据输入的ID将电流值赋值到特定的数据位
	{
		case 0x205:
		{
			CAN2_TxData[0] = (int8_t)(Current>>8);   //这种用法可以理解为强制转换+移位，先将我们输入的16位数据强制转换成8位数据，此时高八位会被挤出去，也就是消失，但是因为我们将原本
			CAN2_TxData[1] = (int8_t)Current;        //16位的数据右移了8位，则原来在高数的八位移动到了低八位，例如：12345678 00000000这个16位的数据经过右移则变成00000000 12345678 
			                                        //又因为数据已经被强制转换为8位的，高8位的位置已经取消了，则此时CAN_TxData[0]装着12345678这八个原来Current高八位的数据
		}break;
	  case 0x206:
		{
			CAN2_TxData[2] = (int8_t)(Current>>8);
			CAN2_TxData[3] = (int8_t)Current;
		}break;
		case 0x207:
		{
			CAN2_TxData[4] = (int8_t)(Current>>8);
			CAN2_TxData[5] = (int8_t)Current;
		}break;
		case 0x208:
		{
			CAN2_TxData[6] = (int8_t)(Current>>8);
			CAN2_TxData[7] = (int8_t)Current;
		}break;		
	}


	
	if(HAL_CAN_AddTxMessage(&hcan2,&Tx2Message,CAN2_TxData,&TxMailbox) != HAL_OK) //发送函数，如果发送失败则返回1，成功则返回0，用来验证通讯是否正常，以及兼顾发送信息的任务
	{
		return 1;
	}
	return 0;
}

///***因为是对整个底盘的电流输出  一次性对四个电机的电流进行操作，时效性更高，故不判断ID直接用一个函数，一次性对底盘四个电机进行输出 ***//
///底盘电机ID默认为 0x201-0x204，对应Current1-Current4
void Can_SendMoto_Chassis (int16_t Current1,int16_t Current2,int16_t Current3,int16_t Current4)
{
	
	Tx1Message.StdId = 0x200;
	Tx1Message.ExtId = 0x00;
	Tx1Message.IDE   = CAN_ID_STD;
	Tx1Message.RTR   = CAN_RTR_DATA;
	Tx1Message.DLC   = 8;
	Tx1Message.TransmitGlobalTime = DISABLE;
	
	CAN_TxData[0] = (int8_t)(Current1>>8);
	CAN_TxData[1] = (int8_t)Current1;
	 
	CAN_TxData[2] = (int8_t)(Current2>>8);
	CAN_TxData[3] = (int8_t)Current2;
	
	CAN_TxData[4] = (int8_t)(Current3>>8);
	CAN_TxData[5] = (int8_t)Current3;
	
	CAN_TxData[6] = (int8_t)(Current4>>8);
	CAN_TxData[7] = (int8_t)Current4;
	
	HAL_CAN_AddTxMessage(&hcan1,&Tx1Message,CAN_TxData,&TxMailbox);
}









void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	
	
	
	
	if(hcan == &hcan1)//看看是不是can1的数据，后边电机用得多可能要开启can2，所以这里加一步用来辨别
	{
		HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO0,&Rx1Message,CAN_RxData);///进入中断后再调用一次接收函数，让它接着接收，不会停下接收任务
		
		switch(Rx1Message.StdId)  //查询接收到的消息来源于哪个ID（哪个电机）再将接收到的电机数据分别存放到各个定义好的值，以供后续调节电机速度所用
		{
			case 0x201:
			{
				
				Moto_1.speed = ((int16_t)CAN_RxData[2]<<8|(int16_t)CAN_RxData[3]);
			}break;
			case 0x202:
			{
				Moto_2.speed = ((int16_t)CAN_RxData[2]<<8|(int16_t)CAN_RxData[3]);
			}break;
			case 0x203:
			{
				Moto_3.speed = ((int16_t)CAN_RxData[2]<<8|(int16_t)CAN_RxData[3]);
			}break;
			case 0x204:
			{
				Moto_4.speed = ((int16_t)CAN_RxData[2]<<8|(int16_t)CAN_RxData[3]);
//				Moto_Arm_1.angle = ((int16_t)CAN_RxData[0]<<8|(int16_t)CAN_RxData[1]);
//				Moto_Arm_1.speed = ((int16_t)CAN_RxData[2]<<8|(int16_t)CAN_RxData[3]);
			}break;
	
			
		}
		if(HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING)!=HAL_OK)///检查中断功能是否正常，出错的话再重新启动
		{
			__HAL_CAN_ENABLE_IT(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);
		}
	}
	if(hcan == &hcan2)
	{
		HAL_CAN_GetRxMessage(&hcan2,CAN_RX_FIFO0,&Rx2Message,CAN2_RxData);///进入中断后再调用一次接收函数，让它接着接收，不会停下接收任务
		
		switch(Rx2Message.StdId)  //查询接收到的消息来源于哪个ID（哪个电机）再将接收到的电机数据分别存放到各个定义好的值，以供后续调节电机速度所用
		{
			case 0x205:
			{
				Moto_Arm_1.angle = ((int16_t)CAN2_RxData[0]<<8|(int16_t)CAN2_RxData[1]);
				Moto_Arm_1.speed = ((int16_t)CAN2_RxData[2]<<8|(int16_t)CAN2_RxData[3]);
			}break;
			case 0x206:
			{
				Moto_Arm_2.angle = ((int16_t)CAN2_RxData[0]<<8|(int16_t)CAN2_RxData[1]);
				Moto_Arm_2.speed = ((int16_t)CAN2_RxData[2]<<8|(int16_t)CAN2_RxData[3]);
			}break;
		}
		if(HAL_CAN_ActivateNotification(&hcan2,CAN_IT_RX_FIFO0_MSG_PENDING)!=HAL_OK)///检查中断功能是否正常，出错的话再重新启动
		{
			__HAL_CAN_ENABLE_IT(&hcan2,CAN_IT_RX_FIFO0_MSG_PENDING);
		}
				
	}
}

	
