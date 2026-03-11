#include "stm32f10x.h"
#include "encode.h"
#include "misc.h"
#include "nvic.h"
#include "sys.h" 
#include "delay.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"

void TIM4_Mode_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;  	

	//PB6 ch1  ,PB7 ch2 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//使能TIM4时钟	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能GPIOB时钟
	
	GPIO_StructInit(&GPIO_InitStructure);//将GPIO_InitStruct中的参数按缺省值输入
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//PB6 PB7上拉	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);        

	GPIO_StructInit(&GPIO_InitStructure);//将GPIO_InitStruct中的参数按缺省值输入
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//PB6下拉	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);   	

	NVIC_Config(2);

	TIM_DeInit(TIM4);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = 1000-1;  //设定计数器重装值   
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1; //TIM3时钟预分频值,1=1/1000000s
	TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1 ;//设置时钟分割 T_dts = T_ck_int	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数 
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);  

  TIM_ICInitStructure.TIM_Channel = TIM_Channel_1; //通道1
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;  //上升沿触发
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;  //不分频
  TIM_ICInitStructure.TIM_ICFilter = 0x00; //IC1F=0000 不滤波
  TIM_ICInit(TIM4, &TIM_ICInitStructure); 

                 

//	TIM_ARRPreloadConfig(TIM4, ENABLE);//使能预装载
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);//清除TIM4的更新标志位
	TIM_ITConfig(TIM4, TIM_IT_Update|TIM_IT_CC1,ENABLE);//运行更新中断
	//Reset counter
	TIM4->CNT = 0;//

	//TIM_Cmd(TIM4, ENABLE);   //启动TIM4定时器
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);

}


void TIM4_Init(void)
{
  TIM4_Mode_Config();
}



