#ifndef __usart2_H
#define __usart2_H			 
#include "sys.h"	 								  
 	  	
extern u8 USART2_RX_BUF[640]; 		//接收缓冲,最大64个字节
extern u8 USART2_RX_CNT;   				//接收到的数据长度

//模式控制PA0
//如果想串口中断接收，设置EN_USART2_RX为1，否则设置为0
#define EN_USART2_RX 	1			//0,不接收;1,接收.

														 
void USART2_Init(u32 bound);
void USART2_Send_Data(u8 *buf,u8 len);
void USART2_Receive_Data(u8 *buf,u8 *len);	

void USART2_info_deal(void);
void USART2_data_translate(int fcn,int index);
void USART2_Send_Byte(unsigned char byte);
void USART2_Send(u16 count);

void USART2_Send_Lora(void);
void USART2_Distance_request(void);
void USART2_Deep_request(void);
void USART2_Distance_Tem_request(void);

char AsciiToHex(char data);
float AsciiToFloat(int len,char data[]);
	
#endif	   



