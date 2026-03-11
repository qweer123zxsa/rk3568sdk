#include "timer1.h"
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_it.h"
#include <stdio.h>
#include "my_define.h"


#define SVPWM_USE_BDT 	1
#define USE_HARD_PWM 	1
#define PWM_DUTY 80


#define TimerPeriod  ((72000000 / PWM_FRQ )-1)

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void pwm_rcc_init(void)
{
  /* TIM1, GPIOA, GPIOB, GPIOE and AFIO clocks enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA | 
                         RCC_APB2Periph_AFIO |
                         RCC_APB2Periph_GPIOB, ENABLE);
}

void pwm_cnt_irq_init(void)
{
	
}

/**
  * @brief  Configure the TIM1 Pins.
  * @param  None
  * @retval None
  * @note
  *			PA8 /T1_CH1  ---> HIn3
  *			PA9 /T1_CH2  ---> HIn2
  *			PA10/T1_CH3  ---> HIn1
  *										Out2 ---> PA0/ADC0
  *										Out3 ---> PA1/ADC1
  *			PB15/T1_CHN3 ---> LIn1
  *			PB14/T1_CHN2 ---> LIn2
  *			PB13/T1_CHN1 ---> LIn3
  */
void pwm_pin_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIOA Configuration: Channel 1, 2 and 3 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* GPIOB Configuration: Channel 1N, 2N and 3N as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void pwm_tim_init(void){

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_BDTRInitTypeDef      TIM_BDTRInitStructure;
//	NVIC_InitTypeDef 	NVIC_InitStructure;
	
	uint16_t Channel1Pulse = 0, Channel2Pulse = 0, Channel3Pulse = 0;

	/* TIM1 Configuration ---------------------------------------------------
	 Generate 7 PWM signals with 4 different duty cycles:
	 TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
	 SystemCoreClock is set to 72 MHz for Low-density, Medium-density, High-density
	 and Connectivity line devices and to 24 MHz for Low-Density Value line and
	 Medium-Density Value line devices
	 
	 The objective is to generate 7 PWM signal at 17.57 KHz:
	   - TIM1_Period = (SystemCoreClock / 17570) - 1
	 The channel 1 and channel 1N duty cycle is set to 50%
	 The channel 2 and channel 2N duty cycle is set to 50%
	 The channel 3 and channel 3N duty cycle is set to 50%
	 The Timer pulse is calculated as follows:
	   - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100
	----------------------------------------------------------------------- */
	
	// Compute CCR1 value to generate a duty cycle at 50% for channel 1 and 1N 
	Channel1Pulse = (uint16_t) (((uint32_t) PWM_DUTY * (TimerPeriod - 1)) / 100);
	// Compute CCR2 value to generate a duty cycle at 37.5%  for channel 2 and 2N 
	Channel2Pulse = (uint16_t) (((uint32_t) PWM_DUTY * (TimerPeriod - 1)) / 100);
	// Compute CCR3 value to generate a duty cycle at 25%  for channel 3 and 3N 
	Channel3Pulse = (uint16_t) (((uint32_t) PWM_DUTY * (TimerPeriod - 1)) / 100);

	TIM_DeInit(TIM1);

	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;// TIM_PSCReloadMode_Update
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned2;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	TIM_ITConfig(TIM1,TIM_IT_Update, ENABLE);
	NVIC_Config(8);
	
	/* Channel 1, 2, 3 Configuration in PWM mode */
	#if USE_HARD_PWM

	/**	
	TIM_OCMode_PWM1	PWM模式1
	在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为有效电平，否则为无效电平
	在向下计数时，一旦TIMx_CNT>TIMx_CCR1时通道1为无效电平(OC1REF=0)，否则为有效 电平(OC1REF=1)。
	TIM_OCMode_PWM2 PWM模式2
	在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为无效电平，否则为有效电平
	在向下计数时，一旦TIMx_CNT>TIMx_CCR1时通道1为有效电平，否则为无效电平。
	*/	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OCInitStructure.TIM_Pulse = Channel2Pulse;
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OCInitStructure.TIM_Pulse = Channel3Pulse;
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM1,TIM_OCPreload_Enable);
	
	#endif	

#if SVPWM_USE_BDT
	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
	TIM_BDTRInitStructure.TIM_DeadTime = 0xCE; // 0x1100 1110
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;			   
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
	TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
#endif	
	TIM_CCPreloadControl(TIM1,ENABLE);
	/* TIM1 counter enable */	
	TIM_Cmd(TIM1, ENABLE);
	/* TIM1 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void pwm_init(void){
	pwm_rcc_init();
	pwm_pin_init();
	pwm_cnt_irq_init();
	pwm_tim_init();
}

int32_t get_pwm_period(void){
	return (int32_t)(TimerPeriod);
}

_iq IQtoABS0_1(_iq num)
{
	num = num + _IQ(1.0) ;
	
	return _IQdiv2(num);

}

void pwm_reset_duty(uint8_t index,_iq duty){
	uint16_t cnt;
	float temp;
	
	duty = IQtoABS0_1(duty);
	temp = _IQtoF(duty);
	cnt = (uint16_t)(temp*TimerPeriod);

	
	switch(index){
		case 1:
			TIM1->CCR1 = cnt;
		break;
		case 2:
			TIM1->CCR2 = cnt;
		break;
		case 3:
			TIM1->CCR3 = cnt;		
		break;
	}	
}

void pwm_disable(void){
	TIM_CtrlPWMOutputs(TIM1, DISABLE);
}

void pwm_enable(void){
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}


// **** 定时器中断触发 ****  
void TIM1_UP_IRQHandler(void)
{ 

	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)//  溢出中断
	{
		LED0 = 1;
		
		if((RUN_BIT == RUN))
		{
			motor_control(0);
			
		}
		else if(INIOK_BIT == RUN)
		{

		}
		else if(INI_BIT == RUN)
		{
			init_angle();
		}
		else
		{
			INV_DISABLE; 
			PWM_DISABLE;
			motor_control(0);
		}

		
		LED0 = 0;
	}	

  TIM_ClearFlag(TIM1, TIM_FLAG_Update); 	
}



