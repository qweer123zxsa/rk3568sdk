#ifndef __usart3_H
#define __usart3_H			 
#include "sys.h"	 								  
 	  	

extern volatile u8 USART3_RX_BUF[640]; 		//接收缓冲,最大64个字节
extern volatile u8 USART3_RX_CNT;   				//接收到的数据长度

//模式控制PA0
//如果想串口中断接收，设置EN_USART2_RX为1，否则设置为0
#define EN_USART3_RX 	1			//0,不接收;1,接收.

														 
void USART3_Init(u32 bound);
void USART3_Send_Data(u8 *buf,u8 len);
void USART3_Receive_Data(u8 *buf,u8 *len);	

void USART3_info_deal(void);
void USART3_Send_Byte(unsigned char byte);
void USART3_Send(u16 count);
void USART3_EC_request(void);
void USART3_hardware_check(void);
void data_upload(void);

void USART3_data_translate(int index);
void data_recording(unsigned  int data, int num);
	
#endif	   
















