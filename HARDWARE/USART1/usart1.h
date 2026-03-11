#ifndef __usart1_H
#define __usart1_H			 
#include "sys.h"	 								  
 	  	
extern u8 USART1_RX_BUF[640]; 		//接收缓冲,最大64个字节
extern u8 USART1_RX_CNT;   				//接收到的数据长度
//extern int Shutdown;							//接收到PC发送的停止运动准备上浮的信息

//模式控制PA0
//如果想串口中断接收，设置EN_USART2_RX为1，否则设置为0
#define EN_USART1_RX 	1			//0,不接收;1,接收.

														 
void USART1_Init(u32 bound);
void USART1_Send_Data(u8 *buf,u8 len);
void USART1_Receive_Data(u8 *buf,u8 *len);	

void USART1_info_deal(void);
void USART1_data_translate(void);
void USART1_Send_Byte(unsigned char byte);


#endif	   
















