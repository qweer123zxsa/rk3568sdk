#ifndef __PWM_H__
#define __PWM_H__
#include "sys.h" 

void Timer1Init(u16 arr,u16 psc);
void TIM2_Init(u16 arr,u16 psc);
void TIM3_Init(u16 arr,u16 psc);
void TIM4_Init(u16 arr,u16 psc);
void TIM4_PWM_set_ms(u16 ch1,u16 ch2, u16 ch3,u16 ch4);
void TIM2_PWM_set_ms(u16 ch1,u16 ch2);
void PWM_4_accrt(int speed);
void ALL_PWM(u16 pwm1,u16 PWM2);

#endif

