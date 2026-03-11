/*  
############  author Fu Z  ############ 
串口 To EC传感器
*/
#include "sys.h"		    
#include "usart3.h"
#include "usart1.h"
#include "delay.h"
#include "led.h"
#include "nvic.h"
#include "My_Define.h"
#include "timer.h"
#include "crc16_modbus.h"
//////////////////////////////////////////////////////////////////////////////////

// **************  串口3-EC LORA  **************
#define head            0x55
#define device_address  0xaa


#define USARTx                   USART3
#define PeriphClock_USARTx       RCC_APB1Periph_USART3
#define PeriphClock_USARTx_GPIO  RCC_APB2Periph_GPIOB
#define USARTx_Pin_TX            GPIO_Pin_10  
#define USARTx_Pin_RX            GPIO_Pin_11  
#define USARTx_Pin_GPIO_Group    GPIOB

////////////////////////////////////////////////////////////////////////////////// 	 
  	  
//接收缓存区 	
volatile u8 USART3_RX_BUF[640];  	//接收缓冲,最大64个字节.
//接收到的数据长度
volatile u8 USART3_RX_CNT=0;   

volatile char	Speed = 0;
volatile char	servo_error_arc = 0;
volatile char Delay_Run = 0;

 void USART3_Init(u32 bound)        //初始化IO 串口3
{  	 
	
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	//NVIC_InitTypeDef NVIC_InitStructure;
	
	//串口时钟使能，GPIO 时钟使能，复用时钟使能
	RCC_APB2PeriphClockCmd(PeriphClock_USARTx_GPIO,ENABLE); //  串口xGPIO口时钟使能
	RCC_APB1PeriphClockCmd(PeriphClock_USARTx,ENABLE);// 串口x时钟使能
	
	//复位串口
	USART_DeInit(USARTx); 
	
	//  *********  485  需控制读写！！  ************
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //LED-->PB.2 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.12
	GPIO_ResetBits(GPIOB,GPIO_Pin_2);
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
		
#if EN_USART3_RX	

	NVIC_Config(6);
	USART_ITConfig(USARTx, USART_IT_IDLE, ENABLE);//
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);//开启接受中断
	
#endif	

  //使能串口
	USART_Cmd(USARTx, ENABLE);                    //使能串口x
  USART_ClearFlag(USARTx, USART_FLAG_TC);       //清除标识符
	
}

//**************串口3发送一位数据*************
void USART3_Send_Byte(unsigned char byte)       
{
//	GPIO_SetBits(GPIOB,GPIO_Pin_2);
	USART_SendData(USART3, byte);        
	while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET); 
//	GPIO_ResetBits(GPIOB,GPIO_Pin_2);         
}

void USART3_Send(u16 count)
{

	
}

//**************串口3中断服务函数*************
void USART3_IRQHandler(void)                   //
{
	u8 state = USARTx->SR;
	u8 res = USARTx->DR;	
	//字符中断
	if(state& USART_FLAG_RXNE)				//接收到数据				//接收到数据
		{	 
			USART3_RX_BUF[USART3_RX_CNT++]=res;		
			if(USART3_RX_CNT>=8)
			{
				USART3_info_deal();	
				USART3_RX_CNT=0;	
			}
			
			USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		} 
	
	//空闲中断
  else if(state& USART_FLAG_IDLE)		 //接收完毕进入数据校验、处理、解析
		{   
			USART_ClearITPendingBit(USART3, USART_IT_IDLE);
		} 

} 

void USART3_info_deal(void)
{
  int i=0;
	int error_flag=0;
	
	//for(i=0;i<8;i++)
	{ 
	/*	if(USART3_RX_BUF[i] == device_address
				&&USART3_RX_BUF[i+1] == head)
		{
			USART3_Send_Byte(0xa5);												// 
			delay_us(100);
		}			
		*/
		
		i= 0;
		if(USART3_RX_BUF[i] == head
				&&USART3_RX_BUF[i+1] == device_address)
			{
				USART3_data_translate(i);		               // 符合协议则解析信息	  
			} 
		else 
		{
	    //数据有误
			error_flag++;
		}
		USART3_RX_BUF[i]=0;
		
		if(error_flag>=5)
			return;
	}
	
}


void USART3_data_translate(int index)   
{

}



