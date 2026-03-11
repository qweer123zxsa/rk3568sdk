#include "sys.h"	 
#ifndef __CHARTOHEX_H
#define __CHARTOHEX_H	 

int getIndexOfSigns(char ch);
long hexToDec(char *source);

int isHexNum(uint8_t _hexNum);
void numToHexStr(uint8_t _hexNum, uint8_t* _hexStr);
int toHexStr(const uint8_t* _str, uint8_t *_hexStr);
uint8_t charToHexNum(uint8_t hexChar);
int hexToStr(const uint8_t* _hexStr, uint8_t* _str);
static int convert_string2hex(unsigned char *in_data, int in_data_len, unsigned char *out_data, int *out_data_len);

#endif
