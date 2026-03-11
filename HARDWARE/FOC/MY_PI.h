/*
 * MY_PI.h
 *
 *  Created on: 2020Äê12ÔÂ9ÈÕ
 *      Author: 63117
 */

#ifndef _MY_PI_H_
#define _MY_PI_H_


typedef struct {  _iq  Ref;             // Input: Reference input1
                  _iq  Fdb;             // Input: Feedback input2
                  _iq  Err;             // Variable: Error3
                  _iq  Kp;              // Parameter: Proportional gain4
                  _iq  Up;              // Variable: Proportional output5
                  _iq  Ui;              // Variable: Integral output6
                  _iq  Ud;              // Variable: Derivative output7
                  _iq  OutPreSat;       // Variable: Pre-saturated output8
                  _iq  OutUiMax;//9
                  _iq  OutUiMin;//10
                  _iq  OutMax;          // Parameter: Maximum output11
                  _iq  OutMin;          // Parameter: Minimum output12
                  _iq  Out;             // Output: PID output13
                  _iq  SatErr;          // Variable: Saturated difference14
                  _iq  Ki;              // Parameter: Integral gain15
                  _iq  Kc;              // Parameter: Integral correction gain16
                  _iq  Kd;              // Parameter: Derivative gain17
                  _iq  Up1;             // History: Previous proportional output18
                } MY_PIDREG3;


/*-----------------------------------------------------------------------------
Default initalizer for the PIDREG3 object.
-----------------------------------------------------------------------------*/
#define MY_PIDREG3_DEFAULTS { 0,        \
                           0,           \
                           0,           \
                           _IQ(1.3),    \
                           0,           \
                           0,           \
                           0,           \
                           0,           \
                           0.2,         \
                           -0.2,        \
                           _IQ(1),      \
                           _IQ(-1),     \
                           0,           \
                           0,           \
                           _IQ(0.02),   \
                           _IQ(0.5),    \
                           _IQ(0),   \
                           0            \
                          }

/*------------------------------------------------------------------------------
    PID Macro Definition
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    PI_GRANDO Macro Definition
------------------------------------------------------------------------------*/

#define MY_PID_MACRO(v)                                                                                 \
    v.Err = v.Ref - v.Fdb;                                  /* Compute the error */                     \
    v.Up= _IQmpy(v.Kp,v.Err);                               /* Compute the proportional output */       \
    v.Ui= v.Ui + _IQmpy(v.Ki,v.Up);                         /* Compute the integral output */           \
    v.Ui=  _IQsat(v.Ui, v.OutUiMax, v.OutUiMin);            /* Compute the Ui output */                 \
    v.Ud = _IQmpy(v.Kd,(v.Up - v.Up1));                                                                 \
    v.OutPreSat = v.Up + v.Ui + v.Ud;                                                                   \
    v.Out = _IQsat(v.OutPreSat, v.OutMax, v.OutMin);        /* Saturate the output */                   \
                                                                                                        \
    v.Up1 = v.Up;                                          /* Update the previous proportional output */\



#define reset_my_pi_macro(v)            \
        v.Kp =  SPD_KP;                 \
        v.Ki =  SPD_KI;                 \
        v.Ui_max = SPD_PI_UI_MAX;       \



#endif /* MY_PI_H_ */
