#ifndef __MOTOR_CONTROL_H
#define __MOTOR_CONTROL_H

#include "My_Define.h"
#include "rmp_cntl.h"
#include "rampgen.h"

#define PWM_ENABLE pwm_enable()
#define PWM_DISABLE pwm_disable()
#define INV_ENABLE PAout(11) = 1
#define INV_DISABLE PAout(11) = 0

// ######### 状态机变量定义 ##########
#define STOP 			0
#define RUN 			1
#define CCW				0
#define CW				1
extern volatile u8 RUN_LEVEL;

extern volatile u8 RUN_FLAG;
#define RUN_BIT					(RUN_FLAG & 0x01)
#define DIR_BIT					((RUN_FLAG>>1)&0x01)
#define INI_BIT					((RUN_FLAG>>2)&0x01)
#define INIOK_BIT				((RUN_FLAG>>3)&0x01)

#define SET_RUN_BIT			(RUN_FLAG |= 0x01)
#define RESET_RUN_BIT		(RUN_FLAG &= 0xFE)
#define SET_DIR_BIT			(RUN_FLAG |= 0x02)
#define RESET_DIR_BIT		(RUN_FLAG &= 0xFD)
#define SET_INI_BIT			(RUN_FLAG |= 0x04)
#define RESET_INI_BIT		(RUN_FLAG &= 0xFB)
#define SET_INIOK_BIT		(RUN_FLAG |= 0x08)
#define RESET_INIOK_BIT	(RUN_FLAG &= 0xF7)

#define SET_CW			SET_DIR_BIT
#define SET_CCW			RESET_DIR_BIT


	

extern	sv_mod_t 	svpwm;
extern RMPCNTL rc1 ;
extern RAMPGEN rg1 ;

void waitting(void);
_iq get_adc_angle(void);
void init_angle(void);
void commond_start(void);
void commond_stop(void);
void clr_ctl_vrbs(void);
void clear_pi(void);
void motor_control(char level);
void ctl_struc_init(void);		


#endif
