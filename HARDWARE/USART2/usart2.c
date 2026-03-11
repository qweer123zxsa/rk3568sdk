/*  
############  author Fu Z  ############ 
串口 To 485总线 外接传感器
*/
#include "sys.h"		    
#include "usart2.h"
#include "usart3.h"
#include "delay.h"
#include "led.h"
#include "nvic.h"
#include "usart1.h"
#include "My_Define.h"
#include "crc16_modbus.h"
//////////////////////////////////////////////////////////////////////////////////
// **********************  串口2-485 *************************
#define head            0x5A
#define device_address  0x00
#define point_address   0x14

#define USARTx                   USART2
#define PeriphClock_USARTx       RCC_APB1Periph_USART2
#define PeriphClock_USARTx_GPIO  RCC_APB2Periph_GPIOA
#define USARTx_Pin_TX            GPIO_Pin_2  
#define USARTx_Pin_RX            GPIO_Pin_3  
#define USARTx_Pin_GPIO_Group    GPIOA

////////////////////////////////////////////////////////////////////////////////// 	 
  	  
//接收缓存区 	
u8 USART2_RX_BUF[640];  	//接收缓冲,最大64个字节.
//接收到的数据长度
u8 USART2_RX_CNT;



 void USART2_Init(u32 bound)        //初始化IO串口	
{  	 
	
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
	
	//串口时钟使能，GPIO 时钟使能，复用时钟使能
	RCC_APB2PeriphClockCmd(PeriphClock_USARTx_GPIO,ENABLE); //  串口xGPIO口时钟使能
	RCC_APB1PeriphClockCmd(PeriphClock_USARTx,ENABLE);// 串口x时钟使能
		
  //复位串口
	USART_DeInit(USARTx); 
	
	//  *********  485  需控制读写！！  ************
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; //LED-->PA.4 端口配置
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
//  GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.12
//	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	//**********************************************
	
  //GPIO端口模式设置
  GPIO_InitStructure.GPIO_Pin = USARTx_Pin_TX; //USARTx_TX
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(USARTx_Pin_GPIO_Group, &GPIO_InitStructure);//初始化GPIO
   
  GPIO_InitStructure.GPIO_Pin = USARTx_Pin_RX;//USARTx_RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(USARTx_Pin_GPIO_Group, &GPIO_InitStructure);//初始化GPIO

	//串口参数初始化
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	
	USART_Init(USARTx, &USART_InitStructure); //初始化串口
	
#if EN_USART2_RX	

	NVIC_Config(4);
	USART_ITConfig(USARTx, USART_IT_IDLE, ENABLE);//
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);//开启接受中断
	
#endif	

  //使能串口
	USART_Cmd(USARTx, ENABLE);                    //使能串口x
  USART_ClearFlag(USARTx, USART_FLAG_TC);       //清除标识符
	USART2_RX_CNT=0;															//默认为接收模式
	
}

//**************串口2发送一位数据*************

void USART2_Send_Byte(unsigned char byte)       
{
//	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	USART_SendData(USARTx, byte);        
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!= SET);  
//  GPIO_ResetBits(GPIOA,GPIO_Pin_4);     
}

void USART2_Send(u16 count)
{
	//uint8_t  sum; 
	//sum = head+point_address+device_address+(count>>8)+(count&0xff);  //和值校验
	
	USART2_Send_Byte(head);
	USART2_Send_Byte(point_address);	
	USART2_Send_Byte(device_address);
	USART2_Send_Byte(count>>8);  //发送高位
	USART2_Send_Byte(count&0xff);     //发送低位
	//USART2_Send_Byte(sum);
	
}

//**************串口2中断服务函数*************

//void USART2_IRQHandler(void)                   //
//{
//	 u8 state = USART2->SR;
//	 u8 res = USART2->DR;

//	//字符中断
//	if(state& USART_FLAG_RXNE)				//接收到数据
//	{	 		
//		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
//	  //res =USART_ReceiveData(USART1);														//读取接收到的数据USARTx->DR
//		USART2_RX_BUF[USART2_RX_CNT++]=res;		
//	} 
//	
//	//空闲中断
//  else if(state& USART_FLAG_IDLE)	 //接收完毕进入数据校验、处理、解析
//	{ 
//		USART_ClearITPendingBit(USART2, USART_IT_IDLE);
//		USART2_info_deal();	
//		USART2_RX_CNT=0;	
//	} 

//} 

void USART2_info_deal(void)
{
	u16 temp,i;


	if(USART2_RX_BUF[0] == 0x55
			&&USART2_RX_BUF[1] == 0xaa)	{		
			// 通讯成功指示, 仅当不运行时进入循环
			if(RUN_BIT != RUN){
				for(i=0;i<10;i++)	{
					LED1 = ~LED1;
					delay_ms(20);
				}
			}
			// 清除pi变量
			clear_pi();
			// 协议解析
			switch(USART2_RX_BUF[2]){
				case 0x01:	
					temp = ((u16)USART2_RX_BUF[3]<<8)+USART2_RX_BUF[4];
					pi_ang1.Kp = _IQ(temp/1000.0);
					break;
				case 0x02:
					temp = ((u16)USART2_RX_BUF[3]<<8)+USART2_RX_BUF[4];
					pi_ang1.Ki = _IQ(temp/1000.0);
					break;
				case 0x03:
					temp = ((u16)USART2_RX_BUF[3]<<8)+USART2_RX_BUF[4];
					ANG_SET = _IQsat(_IQ((temp-1000)/1000.0),_IQ(1),_IQ(0.0));
					break;
				case 0x06:
					temp = ((u16)USART2_RX_BUF[3]<<8)+USART2_RX_BUF[4];
					ID_SET = _IQsat(_IQ((temp-1000)/1000.0),_IQ(1),_IQ(-1.0));
					break;
				case 0x07:
					temp = ((u16)USART2_RX_BUF[3]<<8)+USART2_RX_BUF[4];
					IQ_SET = _IQsat(_IQ((temp-1000)/1000.0),_IQ(1),_IQ(-1.0));
					break;
				case 0x08:
					temp = ((u16)USART2_RX_BUF[3]<<8)+USART2_RX_BUF[4];
					SPD_SET = _IQsat(_IQ((temp-1000)/1000.0),_IQ(1),_IQ(-1));
					break;
				case 0x09:
					temp = ((u16)USART2_RX_BUF[3]<<8)+USART2_RX_BUF[4];
					pi_id1.Kp = _IQsat(_IQ((temp)/1000.0),_IQ(10),_IQ(0.0));
					break;
				case 0x0a:
					temp = ((u16)USART2_RX_BUF[3]<<8)+USART2_RX_BUF[4];
					pi_id1.Ki = _IQsat(_IQ((temp)/1000.0),_IQ(1),_IQ(0.0));
					break;
				case 0x0b:
					temp = ((u16)USART2_RX_BUF[3]<<8)+USART2_RX_BUF[4];
					pi_iq1.Kp = _IQsat(_IQ((temp)/1000.0),_IQ(10),_IQ(0.0));
					break;
				case 0x0c:
					temp = ((u16)USART2_RX_BUF[3]<<8)+USART2_RX_BUF[4];
					pi_iq1.Ki = _IQsat(_IQ((temp)/1000.0),_IQ(1),_IQ(0.0));
					break;
				case 0x0d:	
					temp = ((u16)USART2_RX_BUF[3]<<8)+USART2_RX_BUF[4];
					pi_spd1.Kp = _IQ(temp/1000.0);
					break;
				case 0x0e:
					temp = ((u16)USART2_RX_BUF[3]<<8)+USART2_RX_BUF[4];
					pi_spd1.Ki = _IQ(temp/1000.0);
					break;
				case 0x0f:
					commond_stop();
					RUN_LEVEL = USART2_RX_BUF[4];
				case 0x04:
					commond_start();
					break;
				case 0x05:
					commond_stop();
			}
   				
		} 


}


