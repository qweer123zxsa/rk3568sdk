/*  
############  author Fu Z  ############ 
����To ��λ��
*/
#include "sys.h"		    
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "delay.h"
#include "led.h"
#include "nvic.h"
#include "timer.h"
#include "stm32f10x_tim.h"
#include "stdio.h"
#include "My_Define.h"
//////////////////////////////////////////////////////////////////////////////////
// *********  ����1-PC  **********

#define head            0x55
#define device_address  0xaa
#define point_address   0x14

#define USARTx                   USART1
#define PeriphClock_USARTx       RCC_APB2Periph_USART1
#define PeriphClock_USARTx_GPIO  RCC_APB2Periph_GPIOA
#define USARTx_Pin_TX            GPIO_Pin_9  
#define USARTx_Pin_RX            GPIO_Pin_10  
#define USARTx_Pin_GPIO_Group    GPIOA

////////////////////////////////////////////////////////////////////////////////// 	 
  	  
//���ջ����� 	
u8 USART1_RX_BUF[640];  	//���ջ���,���64���ֽ�.
//���յ������ݳ���
u8 USART1_RX_CNT;   


void USART1_GPIO_Init(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	////////////////////////////////////////////////////////////////////////////////// 			
	 //USARTx_TX  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.2
   
  //USARTx_RX	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.3 
   
	//GPIO_ResetBits(GPIOA,GPIO_Pin_9|GPIO_Pin_10);		
////////////////////////////////////////////////////////////////////////////////// 

}

void USART1_Init(u32 bound)        //��ʼ��IO ����2
{  	 
	
 USART_InitTypeDef USART_InitStructure;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE); //  ����xGPIO��ʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);// ����xʱ��ʹ��
	
  ////////////////////////////////////////////////////////////////////////////////// 			 
   //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1
  USART_ClearFlag(USART1, USART_FLAG_TC);


#if EN_USART1_RX	

	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���������ж�

NVIC_Config(3);


#endif	
	
	USART1_GPIO_Init();
}

//**************发送1单个一位数据*************
void USART1_Send_Byte(unsigned char byte)       
{
   USART_SendData(USART1, byte);        
   while( USART_GetFlagStatus(USARTx,USART_FLAG_TC)!= SET);  
     
}

//**************发送1发送一组数据*************
void USART1_Send_Data(u8 *buf, u8 len)
{
	u8 i;
	for(i = 0; i < len; i++)
	{
		USART1_Send_Byte(buf[i]);
	}
}

//**************����1�жϷ�����*************
void USART1_IRQHandler(void)                   //
{
	u8 state = USART1->SR;
	u8 res = USART1->DR;
	
	//�ַ��ж�
	if(state& USART_FLAG_RXNE)				//���յ�����
	{	 		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	  //res =USART_ReceiveData(USART1);														//��ȡ���յ�������USARTx->DR
		USART1_RX_BUF[USART1_RX_CNT++]=res;		
	} 
	
	//�����ж�
  else if(state& USART_FLAG_IDLE)	 //������Ͻ�������У�顢����������
	{ 
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);

		USART1_info_deal();	
		USART1_RX_CNT=0;	
	} 

} 

	
void USART1_info_deal(void)
{

}

//***** ����AUV��λ����Ϣ *******

void USART1_data_translate()   
{

}



