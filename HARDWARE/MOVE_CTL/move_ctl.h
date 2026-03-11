#ifndef __MOVE_CTL_H
#define __MOVE_CTL_H			 
 				

void RELAY_GPIO_init(void);
	
void ACT_Up(void);
void ACT_Down(void);
void ACT_Data_Collection(void);
void ACT_Hold(void);
void Relay_CTL(char cmd);
void duo_test(void);
#endif


