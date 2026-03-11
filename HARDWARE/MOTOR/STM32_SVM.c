#include "STM32_SVM.H"


static void Clark_calc(struct CLARK *v)
{
    float sqrt_3 = SQRT_3;//添加浮点变量为了编译器优化调用硬件浮点指令
    v->IAlpha = v->Ia;
    v->IBeta  = (sqrt_3/3)*(v->Ia+(v->Ib*2));
}




#define SinPointNum  3600
extern const int Sin_Table[SinPointNum];//正弦表
static void Park_Calc(struct PARK *v)
{
    long Sinthe,Costhe;
    UINT16 Point;
    Point  = v->Theta;
    if(Point<2700)
        Costhe = Sin_Table[Point+900];     
    else
        Costhe = Sin_Table[Point-2700];  // Ualpha = Ucos(the)  UpmMax = 2/3Udc
    Sinthe  = Sin_Table[Point];         // Ubeta  = Usin(the)
    v->Id = v->IAlpha*Costhe+v->IBeta*Sinthe;
    v->Iq = -v->IAlpha*Sinthe+v->IBeta*Costhe;
    v->Id = v->Id/0x8000;
    v->Iq = v->Iq/0x8000;
    v->ActId = (float)(v->Id)*7.15*3.3/4096;//最大采样实际电流11.8a
    v->ActIq = (float)(v->Iq)*7.15*3.3/4096;//最大采样实际电流11.8a
}



static void Anti_Park_Calc(struct PARK *v)
{
    int Sinthe,Costhe;
    UINT16 Point;
    Point  = v->Theta;
    if(Point<2700)
        Costhe = Sin_Table[Point+900];     
    else
        Costhe = Sin_Table[Point-2700];  // Ualpha = Ucos(the)  UpmMax = 2/3Udc
    Sinthe  = Sin_Table[Point];         // Ubeta  = Usin(the)
    v->UAlpha = v->Ud*(float)Costhe - v->Uq*(float)Sinthe;//范围是-sqrt(2)μ?+sqrt(2)
    v->UBeta  = v->Ud*(float)Sinthe + v->Uq*(float)Costhe;//范围是-sqrt(2)μ?+sqrt(2)
}





static void Svpwm_Module(struct SVPWM *pstrSvpwm)
{
    UINT8 u8Sector=0;
    UINT8 a,b,c;
    int t1,t2;
    int temp;
    float sqrt_3 = SQRT_3;//添加浮点变量为了编译器优化调用硬件浮点指令
    float m = K;
    UINT8 Udc = MOTOR_POWER;
    /* 利用以下公式确定扇区 */
//         iClarkBeta_calc(&strSvpwm.v);
//         pstrSvpwm->v.Va = pstrSvpwm->v.Beta;                                           // beta
//         pstrSvpwm->v.Vb = (AMP1000_SQRT_3*pstrSvpwm->v.Alpha/2 - pstrSvpwm->v.Beta*AMP1000/2)/AMP1000;  // alpha*sqrt(3/4)-0.5*beta
//         pstrSvpwm->v.Vc = (-AMP1000_SQRT_3*pstrSvpwm->v.Alpha/2 - pstrSvpwm->v.Beta*AMP1000/2)/AMP1000; // -alpha*sqrt(3/4)-beta*0.5
        pstrSvpwm->Ua = pstrSvpwm->UBeta;                                           // beta
        pstrSvpwm->Ub = (sqrt_3*pstrSvpwm->UAlpha/2 - pstrSvpwm->UBeta/2);  // alpha*sqrt(3)-beta
        pstrSvpwm->Uc = (-sqrt_3*pstrSvpwm->UAlpha/2 - pstrSvpwm->UBeta/2); // -alpha*sqrt(3)-beta
        if(pstrSvpwm->Ua>0)
            a = 1;
        else
            a = 0;
        if(pstrSvpwm->Ub>0)
            b = 1;
        else
            b = 0;
        if(pstrSvpwm->Uc>0)
            c = 1;
        else
            c = 0;
        u8Sector = 4*c + 2*b + a;
        
        //*24/MOTOR_POWER
        /* 利用下面公式计算出X、Y、Z 其中Ts为Timer1_Period,Udc为MOTOR_POWER*/
        pstrSvpwm->Ua = sqrt_3*pstrSvpwm->Ua/Udc*Ts/(0x8000);//X=sqrt(3)*beta*Ts/Udc
        pstrSvpwm->Ub = sqrt_3*pstrSvpwm->Ub/Udc*Ts/(0x8000);//Y=(sqrt(3)/2*beta+3/2*alpha)*Ts/Udc
        pstrSvpwm->Uc = sqrt_3*pstrSvpwm->Uc/Udc*Ts/(0x8000);//Z=(sqrt(3)/2*beta-3/2*alpha)*Ts/Udc
//         pstrSvpwm->v.Va = SQRT_3*pstrSvpwm->v.Va*Ts/(0x8000)*K;//X=sqrt(3)*beta*Ts/Udc
//         pstrSvpwm->v.Vb = SQRT_3*pstrSvpwm->v.Vb*Ts/(0x8000)*K;//Y=(sqrt(3)/2*beta+3/2*alpha)*Ts/Udc
//         pstrSvpwm->v.Vc = SQRT_3*pstrSvpwm->v.Vc*Ts/(0x8000)*K;//Z=(sqrt(3)/2*beta-3/2*alpha)*Ts/Udc
        /* 计算SVPWM占空比 */
        switch(u8Sector)
        {
            case 0:
                        pstrSvpwm->taOn = Time1_Period / 2;
                        pstrSvpwm->tbOn = Time1_Period / 2;
                        pstrSvpwm->tcOn = Time1_Period / 2;
                        break;
            case 1:
//                         if(Ts>(t1+t2))
                        {
//                             t1 = -pstrSvpwm->v.Vc;
//                             t2 = -pstrSvpwm->v.Vb;
                              t1 = -pstrSvpwm->Ub;//U2t  //这个U2矢量先发生，所以在前
                                t2 = -pstrSvpwm->Uc;//U6t
                        }
                        if(Ts<(t1+t2))
                        {
                            t1 = t1*Ts/(t1+t2);
                            t2 = t2*Ts/(t1+t2);
                        }
                        pstrSvpwm->tbOn = ((Ts)- t1 - t2)/4;          //Tbon = (1-t1-t2)/4
                        pstrSvpwm->taOn = pstrSvpwm->tbOn + t1/2;     //Taon = Tbon + t1/2
                        pstrSvpwm->tcOn = pstrSvpwm->taOn + t2/2;     //Tcon = Taon + t2/2
                        break; //2号扇区
            case 2:
//                         t1 = -pstrSvpwm->v.Va;
//                         t2 = -pstrSvpwm->v.Vc;
                        t1 = -pstrSvpwm->Uc;//Ut4
                        t2 = -pstrSvpwm->Ua;//Ut5
                        if(Ts<(t1+t2))
                        {
                            t1 = t1*Ts/(t1+t2);
                            t2 = t2*Ts/(t1+t2);
                        }
                        pstrSvpwm->taOn = ((Ts)-t1 - t2)/4;//Taon = (1-t1-t2)/4
                        pstrSvpwm->tcOn = pstrSvpwm->taOn + t1/2;              //Tcon = Taon + t1/2
                        pstrSvpwm->tbOn = pstrSvpwm->tcOn + t2/2;              //Tbon = Tcon + t2/2
                        break;//6号扇区
            case 3:
                        t1 = pstrSvpwm->Ub;//Ut4
                        t2 = pstrSvpwm->Ua;//Ut6
                        if(Ts<(t1+t2))
                        {
                            t1 = t1*Ts/(t1+t2);
                            t2 = t2*Ts/(t1+t2);
                        }
                        pstrSvpwm->taOn = ((Ts)-t1 - t2)/4;//Taon = (1-t1-t2)/4
                        pstrSvpwm->tbOn = pstrSvpwm->taOn + t1/2;              //Tbon = Taon + t1/2
                        pstrSvpwm->tcOn = pstrSvpwm->tbOn + t2/2;              //Tcon = Tbon + t2/2
                        break;//1号扇区
            case 4:
//                         t1 = -pstrSvpwm->v.Vb;
//                         t2 = -pstrSvpwm->v.Va;
                        t1 = -pstrSvpwm->Ua;//Ut1
                        t2 = -pstrSvpwm->Ub;//Ut3
                        if(Ts<(t1+t2))
                        {
                            t1 = t1*Ts/(t1+t2);
                            t2 = t2*Ts/(t1+t2);
                        }
                        pstrSvpwm->tcOn = ((Ts)-t1 - t2)/4;             //Tcon = (1-t1-t2)/4
                        pstrSvpwm->tbOn = pstrSvpwm->tcOn + t1/2;         //Tbon = Tcon + t1/2
                        pstrSvpwm->taOn = pstrSvpwm->tbOn + t2/2;         //Taon = Tbon + t2/2
                        break;//4号扇区
            case 5:
                        t1 = pstrSvpwm->Ua;//Ut2
                        t2 = pstrSvpwm->Uc;//Ut3
                        if(Ts<(t1+t2))//·防止发生过调整导致圆形电压矢量失真，所以采取比列缩小
                        {
                            t1 = t1*Ts/(t1+t2);
                            t2 = t2*Ts/(t1+t2);
                        }
                        pstrSvpwm->tbOn = ((Ts)- t1 - t2)/4;//Tbon = (1-t1-t2)/4
                        pstrSvpwm->tcOn = pstrSvpwm->tbOn + t1/2;              //Tcon = Tbon + t1/2
                        pstrSvpwm->taOn = pstrSvpwm->tcOn + t2/2;              //Taon = Tcon + t2/2
                        break;//3号扇区
            case 6:
//                         t1 = pstrSvpwm->v.Vc;
//                         t2 = pstrSvpwm->v.Vb;
                        t1 = pstrSvpwm->Uc;//Ut1
                        t2 = pstrSvpwm->Ub;//Ut5
                        if(Ts<(t1+t2))
                        {
                            t1 = t1*Ts/(t1+t2);
                            t2 = t2*Ts/(t1+t2);
                        }
                        pstrSvpwm->tcOn = ((Ts) - t1 - t2)/4;//Tcon = (1-t1-t2)/4
                        pstrSvpwm->taOn = pstrSvpwm->tcOn + t1/2;              //Taon = Tcon + t1/2
                        pstrSvpwm->tbOn = pstrSvpwm->taOn + t2/2;              //Tbon = Taon + t2/2
                        break;//5号扇区
            default:break;
        }
        
                
        /* stm32的中间对齐模式为倒三角，所以重新计算占空比 */
        u16TimeAon = Time1_Period - (UINT16)pstrSvpwm->taOn;
        u16TimeBon = Time1_Period - (UINT16)pstrSvpwm->tbOn;
        u16TimeCon = Time1_Period - (UINT16)pstrSvpwm->tcOn;
        if(t1==0 && t2==0)
        {
            u16TimeAon = Time1_Period;
            u16TimeBon = Time1_Period;
            u16TimeCon = Time1_Period;
        }
        if(u16TimeAon>=(Limit_Pluse_Max_Value))
            u16TimeAon = Limit_Pluse_Max_Value;
        if(u16TimeBon>=(Limit_Pluse_Max_Value))
            u16TimeBon = Limit_Pluse_Max_Value;
        if(u16TimeCon>=(Limit_Pluse_Max_Value))
            u16TimeCon = Limit_Pluse_Max_Value;
        TIM1->CCR1 = u16TimeAon;//Time1_Period/4;//u16TimeAon;//Time1_Period/4*3;////u16TimeAon;//下一个周期生效
        TIM1->CCR2 = u16TimeBon;//0;//u16TimeBon;//Time1_Period/4;////u16TimeBon;//下一个周期生效
        TIM1->CCR3 = u16TimeCon;//0;//u16TimeCon;//Time1_Period/4*3;////u16TimeCon;//下一个周期生效
}