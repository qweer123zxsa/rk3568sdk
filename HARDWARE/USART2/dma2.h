#ifndef __DMA2_H
#define	__DMA2_H	   
#include <stdint.h>

#define buf_max_size 830
extern uint8_t uart2_recv_data[buf_max_size]; // 接收数据缓冲区
extern uint8_t uart2_send_data[buf_max_size]; // 接收数据缓冲区
extern uint8_t uart2_recv_flag;     // 接收完成标志位
extern uint16_t uart2_recv_len;      // 接收的数据长度
extern uint16_t uart2_send_len;      // 接收的数据长度
extern uint8_t uart2_send_flag;     // 发送完成标志位


void uart2_init(uint32_t baud);
void usart2_send_dma(void);
void usart2_recv_dma(void);
void usart2_dma_run(void);
#endif



