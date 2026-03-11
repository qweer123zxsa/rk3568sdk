#ifndef __TIMER1_H__
#define __TIMER1_H__
#include "sys.h" 
#include "IQmathLib.h"

void Timer1Init(u16 arr,u16 psc);
void	pwm_init(void);
void pwm_enable(void);
void pwm_disable(void);
void pwm_reset_duty(uint8_t index,_iq duty);

#endif

