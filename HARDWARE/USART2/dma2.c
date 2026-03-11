#include "stm32f10x.h"
#include "dma.h"
#include "dma2.h"
#include "nvic.h"
#include "secserial.h"
#include "usart1.h" 
#include "usart2.h" 
#include "stm32f10x_dma.h"
#include <string.h>

uint8_t uart2_recv_data[buf_max_size] = {0};  // 接收数据缓冲区
uint8_t uart2_send_data[buf_max_size] = {0};  // 接收数据缓冲区
uint8_t uart2_recv_flag = 0;        // 接收完成标志位
uint16_t uart2_recv_len = 0;         // 接收的数据长度
uint16_t uart2_send_len = 0;				// 发送数据的长度
uint8_t uart2_send_flag = 0;        // 发送完成标志位


/**
 *  @brief  MA1 通道7, UART2_TX 中断控制器配置
 *  @param  无
 *  @retval 无
 *  @note   中断优先级分组全工程只配置一次，在 main 函数最开始进行配置  
 *  @note   中断处理函数在 CMSIS/stm32f10x_it.c 中进行处理   
 */
void DMA1_Channel7_nvic_config(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    
    // 配置串口1的中断控制器
    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel7_IRQn;   // 在 stm32f10x.h 中找 IRQn_Type 枚举
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;  // 抢占优先级
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;         // 子优先级
    NVIC_Init(&NVIC_InitStruct);
}


/**
 *  @brief  串口2 DMA初始化配置
 *  @param  无
 *  @retval 无 
 *  @note    UART2_TX -> DMA1 Channel7; UART2_RX -> DMA1 Channel6
 */
void uart2_dma_config(void)
{
    DMA_InitTypeDef DMA_InitStruct;
    
    DMA_DeInit(DMA1_Channel7);  // DMA1 通道7, UART2_TX
    DMA_DeInit(DMA1_Channel6);  // DMA1 通道6, UART2_RX
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  // 使能DMA1的时钟
    
    // 配置 DMA1 通道7, UART2_TX
    DMA_InitStruct.DMA_PeripheralBaseAddr = (USART2_BASE + 0x04);   // 数据寄存器(USART_DR) 地址偏移：0x04
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)uart2_send_data;  // 内存地址
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize = 0;      // 寄存器的内容为0时，无论通道是否开启，都不会发生任何数据传输
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel7, &DMA_InitStruct);
    
    // 配置 DMA1 通道6, UART2_RX
    DMA_InitStruct.DMA_PeripheralBaseAddr = (USART2_BASE + 0x04);   // 数据寄存器(USART_DR) 地址偏移：0x04
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)uart2_recv_data;  // 内存地址
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;                 // 外设到内存
    DMA_InitStruct.DMA_BufferSize = sizeof(uart2_recv_data);///sizeof(uart2_recv_data[0])
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel6, &DMA_InitStruct);
    
    USART_DMACmd(USART2, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);// 使能DMA串口发送和接受请求
    
   // DMA1_Channel7_nvic_config();
    
    // 配置 DMA1 通道7, UART2_TX 传输完成中断
    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
    
    DMA_Cmd(DMA1_Channel6, ENABLE);     // 开启接收
    DMA_Cmd(DMA1_Channel7, DISABLE);    // 禁止发送
}


/**
 *  @brief  串口2初始化
 *  @param  波特率
 *  @retval 无
 *  @note   UART2_TX -> PA2; UART2_RX -> PA3      
 */
void uart2_init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    
   // GPIO_DeInit(GPIOA);     // 恢复 GPIOA 的寄存器到默认值
   // USART_DeInit(USART2);   // 恢复 UART2 的寄存器到默认值
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,  ENABLE);   // 使能 GPIOA 的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);   // 使能 UART2 的时钟
    
    // 配置发送引脚 PA2
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 配置接收引脚 PA3
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 配置串口参数 收发一体、8n1、无流控
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
    USART_InitStruct.USART_BaudRate = baud;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &USART_InitStruct);
    
    //uart2_nvic_config();    // 中断控制器配置
		NVIC_Config(4);
    uart2_dma_config();     // DMA配置
    
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);  // 使能空闲中断
    
    USART_Cmd(USART2, ENABLE);  // 使能串口2
		USART_ClearFlag(USART2, USART_FLAG_TC);
}

// 串口2的中断处理函数
void USART2_IRQHandler(void)
{   
    uint8_t clear;
		uint8_t* payload;
    int ret;

	
    if( USART_GetITStatus(USART2, USART_IT_IDLE) != RESET )
    {    
				clear = clear;
        clear = USART2->SR; // 清除空闲中断, 由软件序列清除该位(先读USART_SR，然后读USART_DR)
        clear = USART2->DR; // 清除空闲中断
        
        DMA_Cmd(DMA1_Channel6, DISABLE); // 进制USART2接收
        
        // 统计收到的数据的长度
        uart2_recv_len = sizeof(uart2_recv_data) - DMA_GetCurrDataCounter(DMA1_Channel6);
			
				ret = secserial_decrypt(&ctx_a, uart2_recv_data, uart2_recv_len, &payload);		
				if(ret>0)	 
				{
					memcpy(uart1_send_data,payload,ret);
					uart1_send_len = ret;
				  uart2_recv_flag = 1;                // 接收标志置1
				}
				else
				{
					uart1_send_len = 0;
				  uart2_recv_flag = 0;                // 接收标志置0
				//	memset(uart1_send_data, '\0', sizeof(uart1_send_data));
				}
								
				clear_mem(uart2_recv_data);
				usart2_recv_dma();
    }
}


// DMA1 通道7, UART2_TX 传输完成中断
void DMA1_Channel7_IRQHandler(void)
{
    if( DMA_GetITStatus(DMA1_IT_TC7) != RESET ) // DMA1 通道7, UART2_TX 传输完成
    {
        DMA_ClearITPendingBit(DMA1_IT_TC7);     // 清除中断
        uart2_send_flag = 1;
			
			
        DMA_Cmd(DMA1_Channel7, DISABLE);        // 关闭 DMA1 通道7, UART2_TX
    }
}

void usart2_send_dma(void)
{
  DMA_Cmd(DMA1_Channel7, DISABLE);    // 关闭 DMA1 通道7, UART1_TX
  DMA_SetCurrDataCounter(DMA1_Channel7, uart2_send_len);  // ye传输数量寄存器只能在通道不工作(DMA_CCRx的EN=0)时写入
  DMA_Cmd(DMA1_Channel7, ENABLE);    // 开启 DMA1 通道7, UART1_TX
}

void usart2_recv_dma(void)
{         
  DMA_Cmd(DMA1_Channel6, DISABLE);    // DMA1 通道6, UART1_RX
  DMA_SetCurrDataCounter(DMA1_Channel6, sizeof(uart2_recv_data)); 
  DMA_Cmd(DMA1_Channel6, ENABLE);     // DMA1 通道6, UART1_RX
}	



