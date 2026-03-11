#include "sys.h"
#include "svpwm.h"
#include "my_define.h"


sv_mod_t sv = SVGEN_DEFAULTS;

void svpwm_calc_macro(sv_mod_t* p){
	SVGENDQ_MACRO((*p));
}

void svpwm_overflow_check(sv_mod_t* p){
	
	_iq max_time = _IQ(1);
	_iq min_time = _IQ(-1);
	
	if(p->Ta > max_time){
		p->Ta = max_time;
	}
	if(p->Ta < min_time){
		p->Ta = min_time;
	}	
	if(p->Tb > max_time){
		p->Tb = max_time;
	}
	if(p->Tb < min_time){
		p->Tb = min_time;
	}
	if(p->Tc > max_time){
		p->Tc = max_time;
	}
	if(p->Tc < min_time){
		p->Tc = min_time;
	}
}

void svpwm_calc(sv_mod_t* v){
	
	sv_mod_t *p = v;
	if(p == NULL){
		//todo error detect
	}
	p->tmp1= p->Ubeta;															
	p->tmp2= _IQdiv2(p->Ubeta) + (_IQmpy(_IQ(0.866),p->Ualpha));					
  p->tmp3= p->tmp2 - p->tmp1;													
																				
	p->VecSector=3;																
	p->VecSector=(p->tmp2> 0)?( p->VecSector-1):p->VecSector;						
	p->VecSector=(p->tmp3> 0)?( p->VecSector-1):p->VecSector;						
	p->VecSector=(p->tmp1< 0)?(7-p->VecSector) :p->VecSector;						
																				
	if(p->VecSector==1 || p->VecSector==4){
		p->Ta= p->tmp2;												
    	p->Tb= p->tmp1-p->tmp3;
      	p->Tc=-p->tmp2;
	}
	else if(p->VecSector==2 || p->VecSector==5){
		p->Ta= p->tmp3+p->tmp2;
    	p->Tb= p->tmp1;
      	p->Tc=-p->tmp1;
	}else{
		p->Ta= p->tmp3;
		p->Tb=-p->tmp3;												
      	p->Tc=-(p->tmp1+p->tmp2);
	}
	
	svpwm_overflow_check(p);
}



