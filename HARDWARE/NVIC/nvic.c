#include "stm32f10x.h"
#include "misc.h"
#include "nvic.h"
#include "sys.h" 
#include "delay.h"
/*
÷–∂œ∑÷≈‰Àµ√˜
USART2     0-2     	Ω” ’…œŒªª˙÷∏¡Ó
TIM1       1-0    	∏ﬂº∂∂® ±∆˜£¨…˙≥…PWM
TIM3			 2-2			¥Æø⁄∑¢ÀÕ ˝æ›
EXTI4£¨5   2-0;2-1 	∞¥º¸

*/

//1.-----∞¥º¸÷–∂œ
void EXTI_KEY_NVIC_Config(NVIC_InitTypeDef NVIC_InitStructure1)
{
	/*
	NVIC_InitStructure1.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure1.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure1.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure1.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure1);
	*/
  
	NVIC_InitStructure1.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure1.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure1.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure1.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure1);
	
		NVIC_InitStructure1.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure1.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure1.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure1.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure1);
}


//3----¥Æø⁄1Õ®–≈£¨”√”⁄Modbus-Rtu-485S
void USART1_NVIC_Config(NVIC_InitTypeDef NVIC_InitStructure4)
{
	NVIC_InitStructure4.NVIC_IRQChannel = USART1_IRQn; 
	NVIC_InitStructure4.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure4.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure4.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure4);
}

//4. ¥Æø⁄2
void USART2_NVIC_Config(NVIC_InitTypeDef NVIC_InitStructure3)
{
  //”≈œ»º∂…Ë÷√∏ﬂ”⁄∂® ±∆˜÷–∂œ
	//Usart2 NVIC ≈‰÷√
  NVIC_InitStructure3.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure3.NVIC_IRQChannelPreemptionPriority=0;//«¿’º”≈œ»º∂1
	NVIC_InitStructure3.NVIC_IRQChannelSubPriority =2;		//◊””≈œ»º∂1
	NVIC_InitStructure3.NVIC_IRQChannelCmd = ENABLE;			//IRQÕ®µ¿ πƒ‹
	NVIC_Init(&NVIC_InitStructure3);	//∏˘æ›÷∏∂®µƒ≤Œ ˝≥ı ºªØVICºƒ¥Ê∆˜
}
//6.¥Æø⁄3
void USART3_NVIC_Config(NVIC_InitTypeDef NVIC_InitStructure3)
{
  //”≈œ»º∂…Ë÷√∏ﬂ”⁄∂® ±∆˜÷–∂œ
	//Usart2 NVIC ≈‰÷√
  NVIC_InitStructure3.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure3.NVIC_IRQChannelPreemptionPriority=1;//«¿’º”≈œ»º∂1
	NVIC_InitStructure3.NVIC_IRQChannelSubPriority =1;		//◊””≈œ»º∂1
	NVIC_InitStructure3.NVIC_IRQChannelCmd = ENABLE;			//IRQÕ®µ¿ πƒ‹
	NVIC_Init(&NVIC_InitStructure3);	//∏˘æ›÷∏∂®µƒ≤Œ ˝≥ı ºªØVICºƒ¥Ê∆˜
}
//5 ∂® ±∆˜3
void TIM3_NVIC_Config(NVIC_InitTypeDef NVIC_InitStructure3)
{
  //”≈œ»º∂…Ë÷√µÕ”⁄¥Æø⁄÷–∂œ
	//Usart2 NVIC ≈‰÷√
  NVIC_InitStructure3.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure3.NVIC_IRQChannelPreemptionPriority=2;//«¿’º”≈œ»º∂1
	NVIC_InitStructure3.NVIC_IRQChannelSubPriority =2;		//◊””≈œ»º∂1
	NVIC_InitStructure3.NVIC_IRQChannelCmd = ENABLE;			//IRQÕ®µ¿ πƒ‹
	NVIC_Init(&NVIC_InitStructure3);	//∏˘æ›÷∏∂®µƒ≤Œ ˝≥ı ºªØVICºƒ¥Ê∆˜
}
//7 ∂® ±∆˜2
void TIM2_NVIC_Config(NVIC_InitTypeDef NVIC_InitStructure3)
{
  NVIC_InitStructure3.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure3.NVIC_IRQChannelPreemptionPriority=1;//«¿’º”≈œ»º∂1
	NVIC_InitStructure3.NVIC_IRQChannelSubPriority =1;		//◊””≈œ»º∂1
	NVIC_InitStructure3.NVIC_IRQChannelCmd = ENABLE;			//IRQÕ®µ¿ πƒ‹
	NVIC_Init(&NVIC_InitStructure3);	//∏˘æ›÷∏∂®µƒ≤Œ ˝≥ı ºªØVICºƒ¥Ê∆˜
}
//2 ∂® ±∆˜4
void TIM4_NVIC_Config(NVIC_InitTypeDef NVIC_InitStructure2)
{
	NVIC_InitStructure2.NVIC_IRQChannel = TIM4_IRQn; 
	NVIC_InitStructure2.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure2.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure2.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure2);

}

//8 ∂® ±∆˜1
void TIM1_NVIC_Config(NVIC_InitTypeDef NVIC_InitStructure2)
{
	NVIC_InitStructure2.NVIC_IRQChannel = TIM1_UP_IRQn; 
	NVIC_InitStructure2.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure2.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure2.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure2);

}

void NVIC_Config(u8 Interrupt_flag)
{ 

  	NVIC_InitTypeDef   NVIC_InitStructure;
	/* ≈‰÷√÷–∂œ π”√◊È∫œ  «¿’º Ω3Œª(0-7)£¨œÏ”¶ Ω1Œª(0-1) */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

	if(Interrupt_flag == 1)
	{
		/*Õ‚≤ø÷–∂œ1£¨Zœ‡πÈ¡„----”≈œ»º∂◊Ó∏ﬂ 0 0*/
		EXTI_KEY_NVIC_Config(NVIC_InitStructure);
	}
	
	if(Interrupt_flag == 2)
	{	
		/*TIM4÷–∂œ£¨±‡¬Î∆˜Ω‚¬Î----”≈œ»º∂ 1  1*/
		TIM4_NVIC_Config(NVIC_InitStructure);
	}
	if(Interrupt_flag == 3)
	{
		/* USART1÷–∂œ£¨ ˝æ›Ω” ’----”≈œ»º∂ 2  1*/
		USART1_NVIC_Config(NVIC_InitStructure);
	}
	if(Interrupt_flag == 4)
	{
		/* USART2÷–∂œ£¨ ˝æ›Ω” ’----”≈œ»º∂ 0  1*/
		USART2_NVIC_Config(NVIC_InitStructure);
	}
	if(Interrupt_flag == 5)
	{	
		/*TIM3÷–∂œ£1*/
		TIM3_NVIC_Config(NVIC_InitStructure);
	}
	if(Interrupt_flag == 6)
	{	
		/*USART3÷–∂œ,*/
		USART3_NVIC_Config(NVIC_InitStructure);
	}
	if(Interrupt_flag == 7)
	{	
		/*TIM2÷–∂œ,*/
		TIM2_NVIC_Config(NVIC_InitStructure);
	}
	if(Interrupt_flag == 8)
	{	
		/*TIM1÷–∂œ,*/
		TIM1_NVIC_Config(NVIC_InitStructure);
	}

}


