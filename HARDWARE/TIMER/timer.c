#include "My_Define.h"
#include "ipark.h"
#include "dma2.h"


#define TIMx                   TIM3
#define PeriphClock_TIMx       RCC_APB1Periph_TIM3
#define PeriphClock_TIMx_GPIO  RCC_APB2Periph_GPIOA
#define TIMx_Pin_ch1           GPIO_Pin_8  
//#define TIMx_Pin_ch2           GPIO_Pin_9  
#define TIMx_Pin_GPIO_Group    GPIOA

/*
 * 配置PB13输出，即全使能TIM1_CH1N通道，禁用TIM1_CH1通道，禁用更新中断
 * 其它参数看着修改
 */
void Timer1Init(u16 arr,u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA, ENABLE);	//使能定时器1时钟
	
 
   //设置该引脚为复用输出功能,输出TIM2 CH1的PWM脉冲波形	GPIOA.0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11; //TIM_CH1  TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
 
   //初始化TIM2
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM Channel 1-4 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC1
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC2
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC3
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC4
 
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIM1_CH1上的预装载寄存器
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIM1_CH2上的预装载寄存器
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIM1_CH3上的预装载寄存器
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIM1_CH4上的预装载寄存器
 
	TIM_Cmd(TIM1, ENABLE);  //使能TIM1
	TIM_ARRPreloadConfig(TIM1, ENABLE); //使能TIMx在ARR上的预装载寄存器
	TIM_CtrlPWMOutputs(TIM1,ENABLE);        //MOE 主输出使能,高级定时器必须开启这个

}



void TIM2_Init(u16 arr,u16 psc)
{  
			GPIO_InitTypeDef GPIO_InitStructure;
      TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
      TIM_OCInitTypeDef TIM_OCInitStructure;

      //使能定时器TIM2时钟，注意TIM4时钟为APB1，而非APB2
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
      //使能PWM输出GPIO口时钟
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); 
                                                                          
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//定时器TIM4的PWM输出通道1，TIM4_CH1
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO

      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//定时器TIM4的PWM输出通道1，TIM4_CH2
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO


      TIM_TimeBaseStructure.TIM_Period = arr-1;//自动重装值
      TIM_TimeBaseStructure.TIM_Prescaler =psc-1; //时钟预分频数
      TIM_TimeBaseStructure.TIM_ClockDivision = 0;
      TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM向上计数模式
      TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //初始化TIM2
     
      //初始化TIM2_CH1的PWM模式
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//设置PWM模式1
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
      TIM_OCInitStructure.TIM_Pulse = 0; //
      TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//输出极性为高
      TIM_OC1Init(TIM2, &TIM_OCInitStructure);//初始化TIM4_CH1

      //初始化TIM2_CH2的PWM模式
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
      TIM_OCInitStructure.TIM_Pulse = 0;
      TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
       //TIM4_CH2初始化，注意为OC2，而不是OC1，下面两个通道依次类推。
      TIM_OC2Init(TIM2, &TIM_OCInitStructure);

      //使能4个通道的预装载寄存器
      TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);//OC1
      TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);//OC2
      TIM_ARRPreloadConfig(TIM2, ENABLE); //使能重装寄存器
			
		  NVIC_Config(7);
			TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
			TIM_ClearFlag(TIM2, TIM_FLAG_Update);
			TIM_ARRPreloadConfig(TIM2, ENABLE); //使能TIMx在ARR上的预装载寄存器
      TIM_Cmd(TIM2, ENABLE);//使能定时器TIM2，准备工作 
			
			TIM2_PWM_set_ms(1500,1500);
}

void TIM4_Init(u16 arr,u16 psc)
{  
			GPIO_InitTypeDef GPIO_InitStructure;
      TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
      TIM_OCInitTypeDef TIM_OCInitStructure;

      //使能定时器TIM4时钟，注意TIM4时钟为APB1，而非APB2
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
      //使能PWM输出GPIO口时钟
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); 
                                                                          
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//定时器TIM4的PWM输出通道1，TIM4_CH1
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO

      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//定时器TIM4的PWM输出通道1，TIM4_CH2
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO

      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//定时器TIM4的PWM输出通道3，TIM4_CH3
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO

      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//定时器TIM4的PWM输出通道2，TIM4_CH4
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO

      TIM_TimeBaseStructure.TIM_Period = arr;//自动重装值
      TIM_TimeBaseStructure.TIM_Prescaler =psc; //时钟预分频数
      TIM_TimeBaseStructure.TIM_ClockDivision = 0;
      TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM向上计数模式
      TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //初始化TIM4
     
      //初始化TIM4_CH1的PWM模式
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//设置PWM模式1
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
      TIM_OCInitStructure.TIM_Pulse = 0; //
      TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//输出极性为高
      TIM_OC1Init(TIM4, &TIM_OCInitStructure);//初始化TIM4_CH1

      //初始化TIM4_CH2的PWM模式
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
      TIM_OCInitStructure.TIM_Pulse = 0;
      TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
       //TIM4_CH2初始化，注意为OC2，而不是OC1，下面两个通道依次类推。
      TIM_OC2Init(TIM4, &TIM_OCInitStructure);

       //初始化TIM4_CH3的PWM模式
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
      TIM_OCInitStructure.TIM_Pulse = 0;
      TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
      TIM_OC3Init(TIM4, &TIM_OCInitStructure);

      //初始化TIM4_CH4的PWM模式
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
      TIM_OCInitStructure.TIM_Pulse = 0;
      TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
      TIM_OC4Init(TIM4, &TIM_OCInitStructure);

      //使能4个通道的预装载寄存器
      TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);//OC1
      TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);//OC2
      TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);//OC3
      TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);//OC4
      TIM_ARRPreloadConfig(TIM4, ENABLE); //使能重装寄存器
			
			/*
		  NVIC_Config(4);
			TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
			TIM_ClearFlag(TIM4, TIM_FLAG_Update);
			TIM_ARRPreloadConfig(TIM4, ENABLE); //使能TIMx在ARR上的预装载寄存器
			*/
      TIM_Cmd(TIM4, ENABLE);//使能定时器TIM4，准备工作 
			
			TIM4_PWM_set_ms(1500,1500,1500,1500);

}


void TIM3_Init(u16 arr,u16 psc)
{  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //使能GPIO外设时钟使能
	                                                                     	

	                                                                                
	TIM_TimeBaseStructure.TIM_Period = arr-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler = psc-1; //设置用来作为TIMx时钟频率除数的预分频值  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

  NVIC_Config(5);
 
  TIM_ClearFlag(TIM3, TIM_FLAG_Update);
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_ARRPreloadConfig(TIM3, ENABLE); //使能TIM3在ARR上的预装载寄存器
	TIM_Cmd(TIM3, ENABLE);  //使能TIM3
	
}



void TIM4_PWM_set_ms(u16 ch1,u16 ch2, u16 ch3,u16 ch4)
{
	TIM_SetCompare1(TIM4,ch1);//
  TIM_SetCompare2(TIM4,ch2);//
  TIM_SetCompare3(TIM4,ch3);//
  TIM_SetCompare4(TIM4,ch4);//
}

void TIM2_PWM_set_ms(u16 ch1,u16 ch2)
{
	TIM_SetCompare1(TIM2,ch1);//
  TIM_SetCompare2(TIM2,ch2);//

}

// (u16)(_IQtoF(svpwm.Ta) *1000+1000)          (u16)(_IQtoF(pi_ang1.Ref)*1000+1000)  
//(u16)(_IQtoF(ipark_parameter.Angle)*1000     (u16)(_IQtoF(pi_ang1.Fbk)*1000+1000) 
#define view_1 		(u16)(_IQtoF(pi_spd1.Ref) *1000+1000)   // 速度给定
#define view_2 		(u16)(_IQtoF(pi_spd1.Fbk) *1000+1000)   // 速度反馈
#define view_3 		(u16)(_IQtoF(pi_ang1.Ref) *1000+1000)   // 角度给的
#define view_4 		(u16)(_IQtoF(pi_ang1.Fbk) *1000+1000)   // 角度反馈
#define view_5 		(u16)(_IQtoF(pi_id1.Ref) *1000+1000)    // Id给定
#define view_6		(u16)(_IQtoF(park1.Ds) *1000+1000) 			// Id反馈
#define view_7		(u16)(_IQtoF(pi_iq1.Ref) *1000+1000)  	// Iq给定
#define view_8 		(u16)(_IQtoF(park1.Qs) *1000+1000) 			// IQ反馈

// 定时器2中断 
void TIM3_IRQHandler(void)
{ 
	u16 temp1 = 0;
	u16 send1,send2,send3,send4;
	
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)//  溢出中断
	{
		LED1 =~LED1;
		
		switch(RUN_LEVEL)
		{
			case 1:	
				send1 = view_5;
				send2 = view_6;
				send3 = view_7;
				send4 = view_8; break;
			case 2:
				send1 = view_1;
				send2 = view_2;
				send3 = view_7;
				send4 = view_8; break;
			case 3:
				send1 = view_3;
				send2 = view_4;
				send3 = view_1;
				send4 = view_2; break;
			default:
				send1 = view_5;
				send2 = view_6;
				send3 = view_7;
				send4 = view_8; break;
		}

		
		uart2_send_data[0] = 0x55;
		
		temp1 = send1;
		uart2_send_data[1] = temp1>>8;
		uart2_send_data[2] = temp1&0xff;
		
		temp1 = send2;
		uart2_send_data[3] = temp1>>8;
		uart2_send_data[4] = temp1&0xff;
		
		temp1 = send3;
		uart2_send_data[5] = temp1>>8;
		uart2_send_data[6] = temp1&0xff;
		
		temp1 = send4;
		uart2_send_data[7] = temp1>>8;
		uart2_send_data[8] = temp1&0xff;
		
		uart2_send_data[9] = 0xFF;
		uart2_send_len = 10;
		usart2_send_dma();	// 使能串口1 DMA发送   
		
	}	
  TIM_ClearFlag(TIM3, TIM_FLAG_Update); 	
}

