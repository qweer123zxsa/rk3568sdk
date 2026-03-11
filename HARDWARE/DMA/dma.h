#ifndef __DMA_H
#define	__DMA_H	   
#include <stdint.h>


#define buf_max_size 830
extern uint8_t uart1_recv_data[buf_max_size]; // 接收数据缓冲区
extern uint8_t uart1_send_data[buf_max_size];	// 发送数据缓冲区起始地址
extern uint8_t uart1_recv_flag;     // 接收完成标志位
extern uint16_t uart1_recv_len;      // 接收的数据长度
extern uint16_t uart1_send_len;      // 接收的数据长度
extern uint8_t uart1_send_flag;     // 发送完成标志位


void uart1_init(uint32_t baud);
void usart1_send_dma(void);
void usart1_recv_dma(void);
void clear_mem(uint8_t* buf);
#endif



