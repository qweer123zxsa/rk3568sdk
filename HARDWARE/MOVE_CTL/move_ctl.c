#include "move_ctl.h"
#include "delay.h"
#include "sys.h"
#include "led.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "exti.h"
#include "timer.h"
#include "My_Define.h"

#define head            0x5A
#define device_address  0x01
#define point_address   0x14
#define pc_action       0x41
#define Run             0x01
#define Stop            0x00

//存储信息
/*
u16 Info_EC = 0;
u16 Info_tem = 0;
u16 Info_distance = 0;
u16 Info_deep = 0;
u16 Info_position = 0;
u16 Info_time = 0;
*/
volatile char send_buf_temp[16]={0x55,0xaa,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // 向上位机上发数据数组
volatile char Deepth_Second = 0;		// 下潜抽水时间
volatile int Data_Cel_Second = 200;	// 上浮放水时间

volatile u8 action_down_ticks = 0;  //  下潜时间计数器
volatile u16 action_up_ticks = 0;		//  上浮时间计数器

volatile int StatusFlag = 0;
volatile int EMER_ACT_STICKER = 0;  // 定时器中断动作计数器

void RELAY_GPIO_init(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PA端口时钟
	delay_ms(10);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12; //LED-->端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIO
	GPIO_ResetBits(GPIOA,GPIO_Pin_11|GPIO_Pin_12);			
}

 // 继电器控制  1： 2： 0： 
void Relay_CTL(char cmd)
{
	if(cmd == 1){ // 下沉
		GPIO_SetBits(GPIOA,GPIO_Pin_11);	
		delay_ms(10);
		GPIO_SetBits(GPIOA,GPIO_Pin_12);
	//	PAout(6) = ~PAout(6);
	}
	if(cmd == 2){  //上浮
		GPIO_ResetBits(GPIOA,GPIO_Pin_11);	
		delay_ms(10);		
		GPIO_ResetBits(GPIOA,GPIO_Pin_12);
	//	PAout(7) = ~PAout(7);
	}
	if(cmd == 0){
		GPIO_ResetBits(GPIOA,GPIO_Pin_11);			
		GPIO_SetBits(GPIOA,GPIO_Pin_12);				
	}
}

// ####################  下沉PB6,PB7操作  ####################
void ACT_Down(void)
{ 
		int i = 0;
		action_up_ticks = 0;
		if( action_down_ticks++ < (Deepth_Second*5) )  
		{
			Relay_CTL(1);   // 继电器控制
		}
		else	
		{
			Relay_CTL(0);
			action_down_ticks = 0;
			StatusFlag = 2; // 进入数据采集
			USART1_SEND_TO_NANO();

			// #### 延迟一会儿无刷运行 #####
			for(i=0;i<Delay_Run;i++) delay_ms(1000);
			// #### 开始运行 ####
			PWM_4_accrt(Speed);
		}
}

void ACT_Up()
{
	action_down_ticks = 0;
	//上浮PB6,PB7操作
	
	if(action_up_ticks++ <= 500) 
	{
		Relay_CTL(2);   // 继电器控制
	}
	else
	{
		//Relay_CTL(0);
		action_up_ticks = 0;
	}
	StatusFlag = 0; //进入待命
	
	ALL_PWM(1500,1500); 		// 停止所有舵机运行
	USART2_Send_Lora(); //发送数据
}


int DEEP_ADD = 0;
void ACT_Data_Collection(void)
{
	// #################### 下沉后，传感器请求  ####################
//	USART3_EC_request();	//EC传感器
				
//	USART2_Distance_request();		//距离传感器
			
//	USART2_Deep_request();  		 //深度传感器
	//StatusFlag = 0;
	if( EMER_ACT_STICKER++ > Data_Cel_Second*5)
		{
			EMER_ACT_STICKER = 0; // 每十次进行一次处理？
			StatusFlag = 3; // 进入数据采集
		}
}

void ACT_Hold(void)
{
}

u16 servoer_time_test1 = 500;
void duo_test(void)
{
	int i =0;
		servoer_time_test1 += 100;
		if(servoer_time_test1>2500)
			servoer_time_test1 = 500;
		
		for(i=0;i<4;i++) SERVOR_PW_SET[i] =servoer_time_test1; 
}
