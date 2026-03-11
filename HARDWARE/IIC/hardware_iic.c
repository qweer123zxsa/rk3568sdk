#include "hardware_iic.h"
#include "My_Define.h"
#include "stm32f10x_i2c.h"

void I2C1_Configuration()
{
        GPIO_InitTypeDef GPIO_InitStructure;
        I2C_InitTypeDef I2C_InitStructure;
        
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//SCL和SDA
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//频率参考“中文参考手册”，标准模式下必须至少为2MHz
        GPIO_Init(GPIOB,&GPIO_InitStructure);
        
        I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
        I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
        I2C_InitStructure.I2C_ClockSpeed = 100000;//标准速度100k
        I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;//标准速度（100K）下无意义,但还是设置一下
        I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
        I2C_InitStructure.I2C_OwnAddress1 = 0x01;//I2C1自身7位地址.因为可以用另一个函数设置双地址，所以此处为Address1
        I2C_Init(I2C1, &I2C_InitStructure);
        I2C_Cmd(I2C1, ENABLE);     
}

void HmcByteWrite(u8 addr,u8 dataValue)
{
	I2C_GenerateSTART(I2C1,ENABLE);

	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1,HMC_ADDR,I2C_Direction_Transmitter);

	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1,addr);

	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_SendData(I2C1,dataValue);

	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTOP(I2C1,ENABLE);

}

u8 HmcByteRead(u8 addr)
{
	u8 dataValue;

	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));

	I2C_GenerateSTART(I2C1,ENABLE);//起始信号

	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1,HMC_ADDR,I2C_Direction_Transmitter);//发送设备地址+写信号

	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//

	I2C_Cmd(I2C1,ENABLE);

	I2C_SendData(I2C1,addr);//发送存储单元地址，从0开始

	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTART(I2C1,ENABLE);//起始信号

	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1,HMC_ADDR,I2C_Direction_Receiver);//发送设备地址+读信号

	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	I2C_AcknowledgeConfig(I2C1,DISABLE);

	I2C_GenerateSTOP(I2C1,ENABLE);

	while(!(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)));

	dataValue=I2C_ReceiveData(I2C1);//读出寄存器数据

	return dataValue;
}