#ifndef __HC_H__
#define __HC_H__
#include "sys.h" 
void Hcsr04Init(void);
static void OpenTimerForHc(void) ;
static void CloseTimerForHc(void) ;
u32 GetEchoTimer(void);
float Hcsr04GetLength(void );

#endif
