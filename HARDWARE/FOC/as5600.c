#include "My_Define.h"
# include "AS5600.H"
# include "delay.h"
# include "sys.h"


void AS5600_Init(void)
{
	IIC_Init();
}

u16 AS5600_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
	//SDA_IN();
	
	IIC_Start();
	
	IIC_Send_Byte((addr << 1) | Write_Bit);
	
	if (IIC_Wait_Ack())
	{
		IIC_Stop();
		
		return 1;
	}
	
	IIC_Send_Byte(reg);
	
	IIC_Wait_Ack();
	
	IIC_Start();
	
	IIC_Send_Byte((addr<<1) | Read_Bit);//发送器件地址+读命令
	
	IIC_Wait_Ack();		//等待应答 
	
	while(len)
	{
		if(len==1)*buf=IIC_Read_Byte(0);//读数据,发送nACK 
		else *buf=IIC_Read_Byte(1);		//读数据,发送ACK  
		len--;
		buf++; 
	}    
	
	IIC_Stop();	//产生一个停止条件 
	
	return 0;	
		
}

u16 get_angle_u16(void)
{
	u8 buf[2];
	AS5600_Read_Len(Slave_Addr,Angle_Hight_Register_Addr,2,buf);
	
	return ((u16)buf[0]<<8) + buf[1];
}


_iq get_el_angle_iq(u16 temp)
{
	temp = _IQ(temp/585.1429) ;

	return temp;
}


