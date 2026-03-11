/*  
############  author Fu Z  ############ 
全局变量定义
*/
#ifndef __MY_DEFINE_H
#define __MY_DEFINE_H	 

#include "stm32f10x.h"
#include "IQmathLib.h"
#include "timer.h"
#include "timer1.h"
#include "misc.h"
#include "nvic.h"
#include "sys.h" 
#include "delay.h"
#include "dma2.h"
#include "led.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "AS5600.H"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "clarke.h"
#include "park.h"
#include "svpwm.h"
#include "pi.h"
#include "motor_control.h"
#include "exti.h"
#include "adc.h"



extern volatile _iq Ud_SET,Uq_SET,SPD_SET,ANG_SET,ID_SET,IQ_SET,ANG_KP,ANG_KI,ID_KP,ID_KI,IQ_KI,IQ_KP;  
extern volatile _iq MOD; //调制度  
extern volatile u16 PWM_FRQ;
extern volatile _iq PWM_DUTY;	
extern _iq INI_ANGLE;


extern PI_CONTROLLER pi_ang1,pi_id1,pi_iq1,pi_spd1;
extern CLARKE	clarke1;
extern PARK park1;
#endif
