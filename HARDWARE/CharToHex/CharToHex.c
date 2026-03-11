#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "CharToHex.h"

/* ASCII字符 '0'  '1'  '2'  '3'  '4'  '5'  '6'  '7'  '8'  '9' 
 * ASCII码值 0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x38 0x39 
 */

#define MAX_DATA_SIZE 1024

typedef unsigned char uint8_t;
 
//判断是否是十六进制的基数
int isHexNum(uint8_t _hexNum)
{
	if('0'<=_hexNum && _hexNum<='9')
	{
		return 1;
	}
	else if('A'<=_hexNum && _hexNum<='F')
	{
		return 2;
	}
	else if('a'<=_hexNum && _hexNum<='f')
	{
		return 3;
	}
	return -1;
}
 
void numToHexStr(uint8_t _hexNum, uint8_t* hexStr)
{
	uint8_t tmp;
	
	if(NULL == hexStr)
	{
		return;
	}
	
	//低4bit
	tmp = (_hexNum >> 4) & 0x0f;
	if(tmp <= 9)
		*hexStr = tmp+'0';
	else
		*hexStr = tmp-0x0a + 'A';
	
	hexStr++;
	
	//高4bit
	tmp = _hexNum & 0x0f;	
	if(tmp <= 9)
		*hexStr = tmp+'0';
	else
		*hexStr = tmp-0x0a + 'A';
}
 
//将字符串转为16进制形式，以查看不可见字符 "01" ==> "3031"
int toHexStr(const uint8_t *str, uint8_t *hexStr)
{
	int i;
	int len;
	uint8_t* resultPtr;
	if(NULL == str || NULL == hexStr)
	{
		return -1;
	}
	
	len = strlen(str);
	resultPtr = hexStr;
	for(i=0; i<len; i++)
	{
		numToHexStr(str[i], resultPtr);
		resultPtr += 2;
	}
	return strlen(hexStr);
}
 
//十六进制的字符转数字
uint8_t charToHexNum(uint8_t hexChar)
{
	uint8_t tmp;
	if(1>isHexNum(hexChar))
	{
		return 0xFF;
	}	
	
	if(hexChar<='9')
	{
		tmp = hexChar-'0';
	}
	else if(hexChar<='F')
	{
		tmp = hexChar-'7';
	}
	else
	{
		tmp = hexChar-'W';
	}
	return tmp;
}
 
//将16进制形式的字符串转为文本形式 "3031" ==> "01"
int hexToStr(const uint8_t *hexStr, uint8_t *str)
{
	int i;
	int len;
	uint8_t* ptr;
	if(NULL == str || NULL == hexStr)
	{
		return -1;
	}
	
	len = strlen(hexStr);
	ptr = str;
	
	//要是单数个字符，则最后一个会被丢弃
	for(i=0; i<len-1;i++)
	{
		//是十六进制的基数才转换
		//if(0<isHexNum(hexStr[i]))
		{			
			*ptr = charToHexNum(hexStr[i])*16;			
			i++;
			*ptr += charToHexNum(hexStr[i]);
			ptr++;
		}
	}
			
	return strlen(str);
}

static int convert_string2hex(unsigned char *in_data, int in_data_len, unsigned char *out_data, int *out_data_len)
{
    int i;
    int loop_count;
    int convert_point = 0;
    int mem_point = 0;
    unsigned char convert_result;
    unsigned char temp[3] = {0}; /* 为啥是3呢？ */
	
	/* 检查参数有效性 */
    if (in_data == NULL || in_data_len <= 0 || out_data == NULL || out_data_len == NULL || (in_data_len % 2) != 0) {
        printf("invalid parameters\n");
        return -1;
    }

	/* 判断是否超过16进制范围 0 ~ F */
    for (i = 0; i < in_data_len; i++) {
        if ((in_data[i] < '0') || (in_data[i] > 'f') || ((in_data[i] > '9') &&(in_data[i] < 'A'))) {
            printf("out of range\n");
            return -1;
        }
    }

    loop_count = in_data_len / 2;
    memset(out_data, 0x00, *out_data_len);
    *out_data_len = 0;

    for (i = 0; i < loop_count; i++) {
        memset(temp, 0x00, sizeof(temp));
        memcpy(temp, in_data + convert_point, 2);
        convert_point += 2;

        convert_result = strtoul(temp, NULL, 16);

        memcpy(out_data + mem_point, &convert_result, sizeof(unsigned char));
        mem_point += sizeof(unsigned char);
        *out_data_len += sizeof(unsigned char);
    }

    return 0;
}


