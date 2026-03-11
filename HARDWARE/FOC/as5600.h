# ifndef __AS5600_H
# define __AS5600_H
# include "myiic.H"
# include "hardware_iic.h"

# define Slave_Addr 0x36
# define Write_Bit 0
# define Read_Bit 1
# define Angle_Hight_Register_Addr 0x0C
# define Angle_Low_Register_Addr 0x0D

void AS5600_Init(void);
u16 AS5600_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf);
u16 get_angle_u16(void);
_iq get_el_angle_iq(u16 temp);

void AS5600_Init1(void);  //初始化
u16 AS5600_Read_Len1(u8 addr, u8 reg, u8 len, u8 *buf );//从AS5600读取一次数据
void Get_Ini_Val(void); //得到上电后角度初始值
void Get_Temp_Add(void); //等到角度增量（原始值表示的）
void Change_angle(void); //将原始增量数据转化为角度
void Get_Num_sign(void); //用于过零点计数

# endif
