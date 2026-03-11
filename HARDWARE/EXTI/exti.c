#include "exti.h"
#include "nvic.h"
#include "led.h"
#include "sys.h" 
#include "delay.h"
#include "timer.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_tim.h"
#include "My_Define.h"

#define GPIO_PortSourceGPIOx   GPIO_PortSourceGPIOB
#define GPIO_PinSourcex_0      GPIO_PinSource0
#define GPIO_PinSourcex_1      GPIO_PinSource4
#define GPIO_PinSourcex_2      GPIO_PinSource5
#define EXTI_Linex_0           EXTI_Line0
#define EXTI_Linex_1           EXTI_Line4
#define EXTI_Linex_2           EXTI_Line5
#define PeriphClock_AFIO       RCC_APB2Periph_AFIO
#define PeriphClock_GPIO       RCC_APB2Periph_GPIOB
#define EXTI_Pin0              GPIO_Pin_0
#define EXTI_Pin1              GPIO_Pin_4  
#define EXTI_Pin2              GPIO_Pin_5  
#define EXTI_Pin_GPIO_Group    GPIOB


void EXTI_KEY_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/* config the extiline clock and AFIO clock */
	RCC_APB2PeriphClockCmd(PeriphClock_GPIO| PeriphClock_AFIO,ENABLE);
	/* config the NVIC */
	

	NVIC_Config(1);
	
	

	/* EXTI line gpio config*/	
	GPIO_InitStructure.GPIO_Pin = EXTI_Line0|EXTI_Pin1|EXTI_Pin2;       
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 // 浮空輸入
	GPIO_Init(EXTI_Pin_GPIO_Group, &GPIO_InitStructure);
	/*
	// EXTI line mode config 
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOx,GPIO_PinSourcex_0); 
	EXTI_InitStructure.EXTI_Line = EXTI_Linex_0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
  */
	/* EXTI line mode config */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOx,GPIO_PinSourcex_1); 
	EXTI_InitStructure.EXTI_Line = EXTI_Linex_1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
	
		/* EXTI line mode config */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOx,GPIO_PinSourcex_2); 
	EXTI_InitStructure.EXTI_Line = EXTI_Linex_2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
	


}

int EXTI0_TRIG_MODE=1;// 0代表当前下降沿有效。1代表上升沿有效
//  下降沿有效则此时未检测到物体，待检测到后向上位机发送开始指令

void EXTI0_IRQHandler(void)
{
	
  EXTI_ClearITPendingBit(EXTI_Line0);	
	delay_ms(100); 
	if(PBin(0)==0)
	{    
		
	}		
	//USART2_StartOrStop_To_Pc(1);		// 发送命令告知上位机开始检测


		
}


void EXTI4_IRQHandler(void)
{

//int PWM_PULS_SET=10000;
	delay_ms(20);  // 软件防抖
	if(PBin(4)==0)
	{
		pi_ang1.ui = 0;
		pi_ang1.i1 = 0;
		if(ANG_SET < _IQ(1) )
			ANG_SET += _IQ(0.1);
		else
			ANG_SET = _IQ(0);
		
		commond_start();
	 }				
 EXTI_ClearITPendingBit(EXTI_Line4);

}

void EXTI9_5_IRQHandler(void)
{
	
	delay_ms(20);   //  软件防抖	
	if(PBin(5)==0)
	{
		commond_stop();
 	}	

			EXTI_ClearITPendingBit(EXTI_Line5);
}

