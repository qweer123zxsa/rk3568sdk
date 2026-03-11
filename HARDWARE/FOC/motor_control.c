#include "My_Define.h"
#include "motor_control.h"
#include "rmp_cntl.h"
#include "rampgen.h"
#include "my_pi.h"
#include "pi.h"
#include "clarke.h"
#include "ipark.h"
#include "adc.h"

// ############### MOD CONFIG  ##############
volatile u8 RUN_FLAG = 0;		// 运行状态指示
//_iq INI_ANGLE = 0;					
volatile u8 RUN_LEVEL = 0;	// 运行闭环类别
// ############ MOTOR Parameters CONFIG  ###########
volatile u16 PWM_FRQ = 5000;		// PWM频率
#define T  					1.0/PWM_FRQ	// 采样周期
#define Np 					7						// 极对数
#define BASE_FRQ		350.0				// 基频
#define Imax				_IQ( 1.5)		// 最大电流幅值
#define Imin				_IQ(-1.5)		// 最小电流幅值
// ############## PI Parameters CONFIG  ############
volatile _iq ID_SET = _IQ(0);
volatile _iq IQ_SET = _IQ(0.2);  
volatile _iq ANG_SET = _IQ(0.1);
volatile _iq SPD_SET = _IQ(0.1);

_iq ANGLE_OFFEST = _IQ(-0.38452);		// 磁编码器初始位置增益，初始寻相更新

// ###############  KP = 3; KI = 0.0001 ############
volatile _iq ANG_KP = _IQ(4);
volatile _iq ANG_KI = _IQ(0.000);
#define		ANG_PI_MAX 	_IQ(0.8);
#define		ANG_PI_MIN 	_IQ(-0.8);
// #################################################
// ##############  KP = 0.6; KI = 0.001 ############
volatile _iq SPD_KP = _IQ(1.5);
volatile _iq SPD_KI = _IQ(0.001);
#define		SPD_PI_MAX 	_IQ(0.4);
#define		SPD_PI_MIN 	_IQ(-0.4);
// #################################################
// ##############  KP = 0.4; KI = 0.001 ############
volatile _iq ID_KP = _IQ(0.4);
volatile _iq ID_KI = _IQ(0.001);
#define		ID_PI_MAX 	_IQ(0.4);
#define		ID_PI_MIN 	_IQ(-0.4);
// #################################################
// ##############  KP = 0.6; KI = 0.001 ############
volatile _iq IQ_KP = _IQ(0.6);
volatile _iq IQ_KI = _IQ(0.001);
#define		IQ_PI_MAX 	_IQ(0.4);
#define		IQ_PI_MIN 	_IQ(-0.4);
// #################################################

// ##################  FOC结构体  ##################
sv_mod_t 			svpwm = SVGEN_DEFAULTS;
RMPCNTL rc1 = RMPCNTL_DEFAULTS;
RAMPGEN rg1 = RAMPGEN_DEFAULTS;
PI_CONTROLLER pi_ang1 = PI_CONTROLLER_DEFAULTS;
PI_CONTROLLER pi_id1 = PI_CONTROLLER_DEFAULTS;
PI_CONTROLLER pi_iq1 = PI_CONTROLLER_DEFAULTS;
PI_CONTROLLER pi_spd1 = PI_CONTROLLER_DEFAULTS;
CLARKE	clarke1 = CLARKE_DEFAULTS;
PARK		park1 = PARK_DEFAULTS;
// ############### other varibles #################
	// 用于角度计算
char cross_zero_flag = 0;
	// 用于速度计算
static s16 record_spd_s6 = 0;
static s16 spd_angle_ctr = 0;
static u16 spd_period_ctr = 0;
static u16 spd_pid_period_ctr = 0;
static u16 ang_pid_period_ctr = 0;
			_iq spd_temp = 0;

// #################################################

// ################  初始化控制器变量  ###################
void ctl_struc_init()
{
	// RC 模块，生成斜坡
	// RC1 -- 频率
  rc1.RampHighLimit = _IQ(1.0);
  rc1.RampLowLimit = _IQ(0);
  rc1.RampDelayMax = 10; // 决定上升速度
  rc1.TargetValue = _IQ(0);
	
	pi_ang1.Kp = ANG_KP;
	pi_ang1.Ki = ANG_KI;
	pi_ang1.Umax = ANG_PI_MAX;
	pi_ang1.Umin = ANG_PI_MIN;
	pi_spd1.Kp = SPD_KP;
	pi_spd1.Ki = SPD_KI;
	pi_spd1.Umax = SPD_PI_MAX;
	pi_spd1.Umin = SPD_PI_MIN;
	pi_id1.Kp = ID_KP;
	pi_id1.Ki = ID_KI;
	pi_id1.Umax = ID_PI_MAX;
	pi_id1.Umin = ID_PI_MIN;
	pi_iq1.Kp = IQ_KP;
	pi_iq1.Ki = IQ_KI;
	pi_iq1.Umax = IQ_PI_MAX;
	pi_iq1.Umin = IQ_PI_MIN;
}

// #################################################

// ################  清除所有控制状态、变量  ################
void clr_ctl_vrbs(void)
{
	RESET_INI_BIT;
	RESET_RUN_BIT;
	RESET_INIOK_BIT;
	ANG_SET = 0;
	pi_ang1.ui = 0;
	pi_ang1.i1 = 0;
	pi_spd1.ui = 0;
	pi_spd1.i1 = 0;
	pi_id1.ui	 = 0;
	pi_id1.i1  = 0;
	pi_iq1.ui	 = 0;
	pi_iq1.i1  = 0;
	cross_zero_flag = 0;
  spd_pid_period_ctr = 0;
  ang_pid_period_ctr = 0;
}
// ###################################################

// ################  清除pi积分项  ###################
void clear_pi(void)
{
		pi_ang1.ui = 0;
		pi_ang1.i1 = 0;
		pi_spd1.ui = 0;
		pi_spd1.i1 = 0;
		pi_id1.ui = 0;
		pi_id1.i1 = 0;
		pi_iq1.ui = 0;
		pi_iq1.i1 = 0;
}
// ###################################################

// ###################  启动命令  ####################
void commond_start(void)
{
	if(INIOK_BIT != 1)
		{
			SET_INI_BIT;
		}
		else
		{
			SET_RUN_BIT;
			LED1 = 0;
		}
}

// #################################################

// ###################  停止命令  ####################
void commond_stop(void)
{
		clr_ctl_vrbs();
		LED1 = 1;
}

// #################################################


// ###################  等待  ####################
void waitting(void)
{	
	INV_ENABLE;
	PWM_ENABLE;
	pwm_reset_duty(1,_IQ(0.5));
	pwm_reset_duty(2,_IQ(0.5));
	pwm_reset_duty(3,_IQ(0.5));
}

// #################################################

// ###################  初始寻相  ####################
static u16 ini_sticker = 0;
_iq ini_angle_temp = 0;
void init_angle(void)
{
	ini_sticker +=1;

	ipark_parameter.Angle = 0;
	ipark_parameter.Ds = _IQ(0.1);
	ipark_parameter.Qs = _IQ(0.0);
	ipark_calc(&ipark_parameter);
	
	svpwm.Ualpha = ipark_parameter.Alpha;
	svpwm.Ubeta = ipark_parameter.Beta;


	SVGENDQ_MACRO(svpwm)
		
	INV_ENABLE;
	PWM_ENABLE;
		
	pwm_reset_duty(1,svpwm.Ta);
	pwm_reset_duty(2,svpwm.Tb);
	pwm_reset_duty(3,svpwm.Tc);
// ##############  未用到  #############	
	if(ini_sticker>(PWM_FRQ))
	{		

		ANGLE_OFFEST = _IQmpy(get_el_angle_iq(get_angle_u16()),_IQ(-1));
		ini_sticker = 0;
		SET_INIOK_BIT;
	}	
// #####################################

// #####################################
}

// ###################  过零处理  ####################
static _iq last_value = 0;
char crose_zero_iq(void)
{
	if( _IQabs(last_value - pi_ang1.Fbk)> _IQ(0.8))
	{
		if((last_value - pi_ang1.Fbk)>_IQ(0.8) )
		{
			last_value = pi_ang1.Fbk+_IQ(1);
			return 1;
		}
		else if((last_value - pi_ang1.Fbk)<_IQ(0.1) )
		{
			last_value = pi_ang1.Fbk+_IQ(-1);
			return 2;
		}
	}
	last_value = pi_ang1.Fbk;
	return 0;
}

// #################################################

// ###################  过流保护  ####################
void over_cur_proctect(PARK v,_iq i_max,_iq i_min)
{
	if(v.Ds>i_max||v.Ds<i_min
			||v.Qs>i_max||v.Qs<i_min
			||clarke1.As>i_max||clarke1.As<i_min
			||clarke1.Bs>i_max||clarke1.Bs<i_min
			||clarke1.Cs>i_max||clarke1.Cs<i_min){
			commond_stop();
		}	
}	
// #################################################

// ###################  转速计算  ####################
// RPS = 500/7;
// ctr/s（额定） = RPS*4096 = 500/7*4096;
// ctr/prd（额定） = ctr/s/PWM_PRD = 500/7*4096/5000 = 58.514;
// 脉冲计数器/4096 [运行距离(圈)]    /  （周期计数器*每个周期的时间）	[实际时间(秒)] / (基频/极对数) [转每秒RPS]		
// 验算： 1个周期，58.5142个计数，计算公式得到 1	

_iq speed_calc_iq(s16 spd_s16,float cal_frq)		//  PWM_FRQ	BASE_FRQ
{
	static s16 spd_temp = 0;
	static s16 spd_dif_temp;
	// 速度：如果基频200hz，7对极，那么每秒钟转过200/7圈
	spd_period_ctr++;
	spd_dif_temp = record_spd_s6 - spd_s16;
	
	if(spd_dif_temp>1000)
		spd_dif_temp = 4096-spd_dif_temp;
	if(spd_dif_temp<-1000)
		spd_dif_temp = -spd_dif_temp - 4096;
	
	
	if(abs(spd_dif_temp)<150)	{
		// ########### 低速法 ##########
		spd_angle_ctr += spd_dif_temp;
		if(spd_period_ctr>25)	{
			spd_temp = _IQdiv(_IQ((double)spd_angle_ctr/4096.0) , _IQ(spd_period_ctr/cal_frq* (BASE_FRQ/Np) ) );
			spd_period_ctr  = 0; // 清除周期计数
			spd_angle_ctr = 0; 	 // 清除低速法角度计数
		}

		// #############################
	}
	else{
		// ############ 高速法 #########
		spd_angle_ctr = 0; // 清除低速法计数
		// 一个周期的增量，除以一个周期的额定量
		spd_temp = _IQdiv( _IQ((double)spd_dif_temp/4096.0) , _IQ((double)(BASE_FRQ/Np/cal_frq)) );

		// #############################		
	}
	
	record_spd_s6 = spd_s16;	// 更新记录
	return spd_temp;
}


// #################################################

// ###################   位置环计算 ################### 
void	ang_pi_cal(u16 ang_u16, _iq ang_ref,char periods)
{
		pi_ang1.Ref = ang_ref;
		pi_ang1.Fbk = _IQ(ang_u16/4096.0);
		cross_zero_flag = crose_zero_iq();
		if(cross_zero_flag == 1)
		{
			pi_ang1.Fbk += _IQ(1);
		}
		else if(cross_zero_flag == 2)
		{
			pi_ang1.Fbk += _IQ(-1);
		}
		
		if( ++ang_pid_period_ctr>periods )
		{
			PI_MACRO(pi_ang1)
			ang_pid_period_ctr = 0;
		}
	}
// #################################################


// ###################  速度环计算 ###################   
void spd_pi_cal(_iq angle,_iq spe_ref,char periods)
{
		spd_pid_period_ctr++;
		if(spd_pid_period_ctr<periods)
		{
		//	spd_temp += speed_calc_iq(angle,BASE_FRQ/periods);
		}
		else
		{
			pi_spd1.Ref = spe_ref;
			pi_spd1.Fbk = speed_calc_iq(angle,(PWM_FRQ/(float)periods));
			
			PI_MACRO(pi_spd1)
			
			spd_pid_period_ctr=0;
			spd_temp=0;
		}
}
// #################################################  

// ###################  电流环计算 ###################   
void	cur_pi_cal(_iq id_ref, _iq iq_ref)
{		// PI计算
		pi_id1.Fbk = park1.Ds;
		pi_id1.Ref = id_ref;

		pi_iq1.Fbk = park1.Qs;
		pi_iq1.Ref = iq_ref;
		
		PI_MACRO(pi_id1)	
		PI_MACRO(pi_iq1)		
}
// #################################################   



void motor_control(char level)
{
	_iq angle_temp;
	_iq iq_ref = 0;

	// 角度计算
		angle_temp = get_angle_u16();
	
	// #################  角度环  #####################
	if(RUN_LEVEL == 3)
		ang_pi_cal(angle_temp,ANG_SET,3);
	// ################################################
	
	// ################  速度环  ######################
	if(RUN_LEVEL == 2 ){
		spd_pi_cal(angle_temp,SPD_SET,1);
	}
	if(RUN_LEVEL == 3 ){
		spd_pi_cal(angle_temp, -pi_ang1.Out,1);
	}
	// ################################################
	
	// ################  电流采样变换  ################
		angle_temp = get_el_angle_iq(angle_temp) + ANGLE_OFFEST;//_IQ(-0.38452)
		// 电流采样		
		clarke1.As   = 	_IQ( (Get_Adc1(1)-2048.0)/930.409); 
		clarke1.Bs   = 	_IQ( (Get_Adc2(4)-2049.0)/930.409 );// 1860.818 
		clarke1.Cs   =  _IQ(0) -clarke1.As -clarke1.Bs; 
		CLARKE_MACRO(clarke1)
		// 旋转变换
		park1.Alpha  = clarke1.Alpha; //_IQmpy(clarke1.Alpha,_IQ(1.1547344));
		park1.Beta   = clarke1.Beta;
		park1.Sine 	 = _IQsinPU(angle_temp);
		park1.Cosine = _IQcosPU(angle_temp);
		PARK_MACRO(park1)
	// ################################################
	
	// ############### 过流保护 #######################
		over_cur_proctect(park1,Imax,Imin);
	// ################################################
		
	// ################  电流环  ######################	
		switch(RUN_LEVEL)
		{
			case 1: iq_ref = IQ_SET; break;
			case 2: iq_ref = -pi_spd1.Out;break;
			case 3: iq_ref = -pi_spd1.Out;break;
			default : iq_ref = 0; break;
		}

		cur_pi_cal(ID_SET, iq_ref);
	
		
	// ###############################################	
		ipark_parameter.Angle = angle_temp;
		ipark_parameter.Ds = pi_id1.Out;
		ipark_parameter.Qs = pi_iq1.Out;
		ipark_calc(&ipark_parameter);
	
		svpwm.Ualpha = ipark_parameter.Alpha;
		svpwm.Ubeta = ipark_parameter.Beta;

		//svpwm_calc(&svpwm);
		SVGENDQ_MACRO(svpwm)
		
		if((RUN_BIT == RUN) && (INIOK_BIT == RUN))
		{
			INV_ENABLE;
			PWM_ENABLE;
	
			pwm_reset_duty(1,svpwm.Ta);
			pwm_reset_duty(2,svpwm.Tb);
			pwm_reset_duty(3,svpwm.Tc);
		}
		else
		{
			clr_ctl_vrbs();
		}
			
}

// #################################################

