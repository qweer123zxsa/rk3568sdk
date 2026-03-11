#include "led.h"
#include "delay.h"
 

//初始化PA为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_INIT(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PA端口时钟


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_11; //LED-->端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIO
	GPIO_ResetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_11);			

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14; //LED-->端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIO
	GPIO_ResetBits(GPIOC,GPIO_Pin_13|GPIO_Pin_14);			

}

