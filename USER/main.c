/*
############  author Fu Z  ############
Usart3 Ϊ��λ������
TIMER1������״̬��
*/
#include "My_Define.h"
extern double Current_Angle;
extern double True_Angle;
extern int dir;
extern u32 temp_add;
extern u32 temp2;
extern u32 temp0;
uint8_t sign = 0;
extern int sign_angle;

int main(void)
{
	// ************* system init ************
	SystemInit();
	delay_init(); // ��ʱ������ʼ��
	// ************* device init ************

	// ##### init structs #####
	// 1��10000�μ��㣬һ���ж�����Ts = 1/Fs�롣����Ƶ��Ϊfn��һ��������ʱ1/fn�롣���Ƶ��ʱ�������ۻ���fn/Fs
	ctl_struc_init();
	LED_INIT(); // LED�˿ڳ�ʼ��
	EXTI_KEY_Config();
	Adc_Init();			// ADC��ʼ��
	TIM3_Init(300, 72); //  10ms
	IIC_Init();
	// ************* communication init  **************
	// USART2_Init(921600);
	uart2_init(921600);
	USART1_Init(115200);
	// ************* main interrupt init *************
	pwm_init();
	AS5600_Init();
	while (1)
	{
		char strff[21];
	    char strff2[21];
		char f2[21];
		Get_Temp_Add();
		Change_angle();
		Get_Current_angle();
		Get_Num_sign();
		//sprintf(strff, "%.2f True_Angle\n", True_Angle); //%0.2f  angle
		sprintf(strff2,"%.2f Current_Angle\n",Current_Angle);//%0.2f  angle
		sprintf(f2,"%d num\n",sign_angle);
		//sprintf(strff2, "%.2f", Current_Angle);

		// u16 current_angle;
		// float angle_degree;
		// char buffer[32];
		// int len;

		// //usart2_dma_run();
		// delay_ms(1000);
		// current_angle = get_angle_u16();
		// angle_degree = (float)current_angle * 360.0f / 4096.0f;
		// len = snprintf(buffer, sizeof(buffer), "Angle: %.3f deg\n", angle_degree);
		// if (len > 0 && len < sizeof(buffer)) {
		//USART1_Send_Data((u8 *)strff, strlen(strff));
		USART1_Send_Data((u8 *)strff2, strlen(strff2));
		USART1_Send_Data((u8 *)f2, strlen(f2));
		//USART1_Send_Data((u8 *)"\r\n", 2);
		//}
		delay_ms(1000);
		
	}
}
