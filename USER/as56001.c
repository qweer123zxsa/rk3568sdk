#include "My_Define.h"
# include "AS5600.H"
# include "delay.h"
# include "sys.h"

u32 angle_ini = 0; 		   //初始角度值
u32 temp0 = 0;			   //初始角度原始输出值
u32 temp1 = 0;		       //上次角度原始输出值
u32 temp2 = 0;		       //这次角度原始输出值
u32 temp_add = 0;	       //从初始角度开始的累计角度原始值
u8  buf[2] = {0};	       //用于oled显示存放变量，和本程序关系不大
int sign_angle = 0;	       //过零点标记（即从0度转到360度之后继续转又回零的那个点，正向经过一次加一，反向经过一次减一）
double True_Angle = 0.0;   //真实角度（累计角度）
int dir = 0; //0正向 1反向  //方向 
double Current_Angle = 0;   //当前角度（就是绝对位置角度，小于360度）
int Jian_Su_Bi=1;

void AS5600_Init1( void ) {
    IIC_Init();
}

u16 AS5600_Read_Len1( u8 addr, u8 reg, u8 len, u8 *buf ) {
    IIC_Start();
    IIC_Send_Byte ( ( addr << 1 ) | Write_Bit );

    if ( IIC_Wait_Ack() ) {
        IIC_Stop();
        return 1;
    }

    IIC_Send_Byte ( reg );
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte ( ( addr << 1 ) | Read_Bit ); // 发送器件地址 + 读命令
    IIC_Wait_Ack(); // 等待应答

    while(len) {
        if ( len == 1 ) {
            *buf = IIC_Read_Byte ( 0 ); // 读数据，发送nACK
        } else {
            *buf = IIC_Read_Byte ( 1 ) & 0x000f; // 读数据，发送ACK 原始1f,改为0f
        }

        len--;
        buf++;
    }

    IIC_Stop();
    return 0;
}

void Get_Ini_Val(void)//获得初始角度
{
	u8 i = 0;
	u32 transfer = 0;
	for ( i = 0; i < 20; i++ ) { // 刚开始数据可能不稳定，直接丢掉
		AS5600_Read_Len ( Slave_Addr, Angle_Hight_Register_Addr, 2, buf );
		delay_ms ( 5 );
    }
	for ( i = 0; i < 20; i++ ) { // 软件滤波
		AS5600_Read_Len ( Slave_Addr, Angle_Hight_Register_Addr, 2, buf );
		transfer += ( ( buf[0] << 8 ) | buf[1] );
		delay_ms ( 5 );
    }
	temp0 = transfer / 20;
	temp2 = temp0;//给temp2初始化为初始值
	temp1 = temp0;//给temp1初始化为初始值
}


void Get_Temp_Add(void)//计算角度增量
{
	
	if(sign_angle == 0)  //当从没经过零点时
	{
		if(temp2 >= temp0) //正转
		{	temp_add = temp2 - temp0;	
			dir = 0;}
		else    			//反转
		{	temp_add = temp0 - temp2;
			dir = 1;}
	}
	else if(sign_angle > 0)//经过一次及以上零点位置后，分两种情况，正向经过与反向经过，需分开讨论
	{

		temp_add = 4096 + temp2 - temp0 + ( sign_angle - 1)*4096;  //正向经过
		dir = 0;
	}
	else 
	{
		temp_add =4096 + temp0 - 4096*(sign_angle+1) - temp2;   //反向经过
		dir = 1;
	}
}

void Change_angle(void)//考虑减速比，计算增量实际角度
{
	True_Angle = (temp_add/(Jian_Su_Bi*4096))*360;  //4096代表as5600原始数据最大值（0~4096）
}

void Get_Current_angle(void)//获取360度角度，即计算绝对位置，小于等于360度
{
	u32 x0 = 0;
	x0 = (temp_add*100)%((u32)(Jian_Su_Bi*100)*4096);
	Current_Angle = (x0/(Jian_Su_Bi*4096))*360/100;
}

void Get_Num_sign(void) //计算过零点次数，这个函数也可以用定时器中断来调用，效果更好
{
	u32 x;
	AS5600_Read_Len ( Slave_Addr, Angle_Hight_Register_Addr, 2, buf );
	temp2 = ( ( buf[0] << 8 ) | buf[1] );
	if(temp1 >= temp2)
	{
		x = temp1 - temp2;
		if(x>2048)//正转通过零点
		{
			sign_angle++;
		}
	}
	else
	{
		x = temp2 -temp1;
		if(x>2048)//反转通过零点
		{
			sign_angle--;
		}
	}
	temp1 = temp2;//每次都把temp2赋给temp1
}



