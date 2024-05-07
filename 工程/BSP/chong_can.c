/*дcan���ã���Ҫ֪�������˲�������֡������֡����Ҫ������Щ������HAL�ļ����濴��˵��
д����ͬС��*/


#include "chong_can.h"
#include "can.h"

uint8_t CAN_TxData[8],CAN_RxData[8]; //����������λ�޷������������Ҫ���͵����ݺͽ��յ�������
uint8_t CAN2_TxData[8],CAN2_TxData2[8],CAN2_RxData[8];
uint32_t TxMailbox = 0;

CAN_TxHeaderTypeDef Tx1Message;  //����һ������ΪTx1Message��CAN_TxHeaderTypeDef���͵Ľṹ�壬������ŷ������ݵ�����
CAN_RxHeaderTypeDef Rx1Message;  //ͬ��

CAN_TxHeaderTypeDef Tx2Message;  //����һ������ΪTx1Message��CAN_TxHeaderTypeDef���͵Ľṹ�壬������ŷ������ݵ�����
CAN_RxHeaderTypeDef Rx2Message;  //ͬ��

MotoData Moto_1;//һ�ŵ�������ݽṹ�� 
MotoData Moto_2;
MotoData Moto_3;
MotoData Moto_4;
MotoData Moto_yaw;
MotoData Moto_Arm_1;
MotoData Moto_Arm_2;

void CanFilter_Init(void)
{
	CAN_FilterTypeDef Canfilter;  
	
	Canfilter.FilterMode = CAN_FILTERMODE_IDMASK;  //����Ϊ����ģʽ
	Canfilter.FilterScale = CAN_FILTERSCALE_32BIT;  //32λ������ģʽ����32λ������ģʽ������16λ�ģ�
	
	Canfilter.FilterIdHigh = 0x0200<<5;//�������Ĳο��ֲ��е�ɸѡ����Ĵ��������й涨��32λɸѡ�����Ϊ��11λΪ��׼֡ID,���Խ�ID������λ
	Canfilter.FilterIdLow  = 0x0000;    //0x200 = B10 0000 0000,   0x0200
	Canfilter.FilterMaskIdHigh = 0x03F0<<5;//������ΪB11 1110 0000 ������λ�����ID1-8��������  0x03F0
	Canfilter.FilterMaskIdLow  = 0x0000;  
	
	Canfilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;//����FIFO0����
	Canfilter.FilterActivation = CAN_FILTER_ENABLE;//ʹ�ܹ�����
	Canfilter.FilterBank = 0;        //ʹ���ĸ���������������CAN1����ѡ0~13
	Canfilter.SlaveStartFilterBank = 14;//��can2�����������������CAN1���Բ�������Ҳ���Ա��ź����
	
	HAL_CAN_ConfigFilter(&hcan1,&Canfilter);  //�����˲�������Ĵ���������Ϊֻ�����ã�������������������������¼�뵽�Ĵ����ģ�
	                                          //��������Ϊ���������ڸĲ��������������ð�ť������ȥ�����޸ĵ�ֵ�Ż����뵽�Ĵ���
	
}
/**
* @brief  CAN2���˲�����
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





/* ������ĳ�ʼ����������������ع��� */
//*����MX_CAN1_Init();�����±�*//
void Can_Init(void)
{
	
	CanFilter_Init();
	Can2Filter_Init();
	
	HAL_CAN_Start(&hcan1);  
	HAL_CAN_Start(&hcan2);
	
	HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);//ʹ���ж�,��FIFO0���յ�����ʱ�����ж�
	HAL_CAN_ActivateNotification(&hcan2,CAN_IT_RX_FIFO0_MSG_PENDING);//ʹ���ж�,��FIFO0���յ�����ʱ�����ж�
	
	
}
	

///** ������������ƺ��� ����ID��0x205-0x208�ĵ�� **//
///*�β�currentΪ�������������β�IDΪ���ID��*//
uint8_t Can_SendMoto(int16_t Current,uint16_t ID)   
{
	Tx2Message.StdId = 0x1FF; //������ͱ�ʶ��id
	Tx2Message.ExtId = 0x00;  //��������չ֡
	Tx2Message.IDE   = CAN_ID_STD;  //֡����  ��׼֡  �����ǲ��õ��Ǳ�׼֡������֮�⻹����չ֡����չ֡���ŵ���ID���������õø��࣬�������ò��ϣ�
	Tx2Message.RTR   = CAN_RTR_DATA;//֡��ʽ  ����֡   ���������Ϊ�����������ݵ�֡���ͣ������֮�⻹��ң��֡����ɶ���в�ѯ��
	Tx2Message.DLC   = 8;          //��Ҫ���͵����ݳ��ȣ�����˵��������������8���ֽ�
	Tx2Message.TransmitGlobalTime = DISABLE;  //ָ���Ƿ��ڿ�ʼʱ����ʱ���������ֵ,û��Ҳ�ò��ϣ���������Ҳ��ʹ�ܣ���֪��Ϊɶ�������Լ�ȥ�����Ǹ����õ�
	
	switch ( ID ) ///���������ID������ֵ��ֵ���ض�������λ
	{
		case 0x205:
		{
			CAN2_TxData[0] = (int8_t)(Current>>8);   //�����÷��������Ϊǿ��ת��+��λ���Ƚ����������16λ����ǿ��ת����8λ���ݣ���ʱ�߰�λ�ᱻ����ȥ��Ҳ������ʧ��������Ϊ���ǽ�ԭ��
			CAN2_TxData[1] = (int8_t)Current;        //16λ������������8λ����ԭ���ڸ����İ�λ�ƶ����˵Ͱ�λ�����磺12345678 00000000���16λ�����ݾ�����������00000000 12345678 
			                                        //����Ϊ�����Ѿ���ǿ��ת��Ϊ8λ�ģ���8λ��λ���Ѿ�ȡ���ˣ����ʱCAN_TxData[0]װ��12345678��˸�ԭ��Current�߰�λ������
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


	
	if(HAL_CAN_AddTxMessage(&hcan2,&Tx2Message,CAN2_TxData,&TxMailbox) != HAL_OK) //���ͺ������������ʧ���򷵻�1���ɹ��򷵻�0��������֤ͨѶ�Ƿ��������Լ���˷�����Ϣ������
	{
		return 1;
	}
	return 0;
}

///***��Ϊ�Ƕ��������̵ĵ������  һ���Զ��ĸ�����ĵ������в�����ʱЧ�Ը��ߣ��ʲ��ж�IDֱ����һ��������һ���ԶԵ����ĸ����������� ***//
///���̵��IDĬ��Ϊ 0x201-0x204����ӦCurrent1-Current4
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
	
	
	
	
	if(hcan == &hcan1)//�����ǲ���can1�����ݣ���ߵ���õö����Ҫ����can2�����������һ���������
	{
		HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO0,&Rx1Message,CAN_RxData);///�����жϺ��ٵ���һ�ν��պ������������Ž��գ�����ͣ�½�������
		
		switch(Rx1Message.StdId)  //��ѯ���յ�����Ϣ��Դ���ĸ�ID���ĸ�������ٽ����յ��ĵ�����ݷֱ��ŵ���������õ�ֵ���Թ��������ڵ���ٶ�����
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
		if(HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING)!=HAL_OK)///����жϹ����Ƿ�����������Ļ�����������
		{
			__HAL_CAN_ENABLE_IT(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);
		}
	}
	if(hcan == &hcan2)
	{
		HAL_CAN_GetRxMessage(&hcan2,CAN_RX_FIFO0,&Rx2Message,CAN2_RxData);///�����жϺ��ٵ���һ�ν��պ������������Ž��գ�����ͣ�½�������
		
		switch(Rx2Message.StdId)  //��ѯ���յ�����Ϣ��Դ���ĸ�ID���ĸ�������ٽ����յ��ĵ�����ݷֱ��ŵ���������õ�ֵ���Թ��������ڵ���ٶ�����
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
		if(HAL_CAN_ActivateNotification(&hcan2,CAN_IT_RX_FIFO0_MSG_PENDING)!=HAL_OK)///����жϹ����Ƿ�����������Ļ�����������
		{
			__HAL_CAN_ENABLE_IT(&hcan2,CAN_IT_RX_FIFO0_MSG_PENDING);
		}
				
	}
}

	
