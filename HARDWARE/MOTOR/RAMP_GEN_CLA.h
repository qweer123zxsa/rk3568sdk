#ifndef __RAMP_GEN_CLA_H__
#define __RAMP_GEN_CLA_H__
#include "IQmathLib.h"

typedef struct { _iq  Freq; 			// Input: Ramp frequency (pu) 	
		 	     _iq  StepAngleMax;	// Parameter: Maximum step angle (pu)		
	 	 	     _iq  Angle;			// Variable: Step angle (pu)					  
			     _iq  Gain;			// Input: Ramp gain (pu)
			     _iq  Out;  	 		// Output: Ramp signal (pu) 	
			     _iq  Offset;			// Input: Ramp offset (pu) 				 
	  	  	   } RAMP_GEN_CLA;	            

/*------------------------------------------------------------------------------
	RAMP(Sawtooh) Generator Macro Definition
------------------------------------------------------------------------------*/                                               

#define RAMP_GEN_CLA_MACRO(v)								\
/* Compute the angle rate */								\
	v.Angle += v.StepAngleMax*v.Freq;					\
																						\
/* Saturate the angle rate within (-1,1) */	\
	if (v.Angle>_IQ(1.0))													\
		v.Angle -= _IQ(1.0);												\
	else if (v.Angle<_IQ(-1.0))									\
		v.Angle +=_IQ(1.0);												\
		v.Out=v.Angle;

#define RAMP_GEN_CLA_INIT(v)	\
	v.Angle=_IQ(0.0);								\
	v.Freq=_IQ(0.0);										\
	v.StepAngleMax=BASE_FREQ*T;	\
	v.Out=_IQ(0.0);									\
	v.Gain = _IQ(1); 								\
	v.Offset = _IQ(0);								
	
#endif // __RAMPGEN_H__
