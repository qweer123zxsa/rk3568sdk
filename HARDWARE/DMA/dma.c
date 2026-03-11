#include "stm32f10x.h"
#include "dma.h"
#include "dma2.h"
#include "nvic.h"
#include "secserial.h"
#include "usart1.h" 
#include "usart2.h" 
#include "stm32f10x_dma.h"
#include <string.h>

 secserial_t ctx_a;
 uint8_t key[32]={0};
 int pkt_buffer_len = 850;
 int ret;
 
uint8_t uart1_recv_data[buf_max_size] = {0};  // 接收数据缓冲区
uint8_t uart1_send_data[buf_max_size] = {0};  // 接收数据缓冲区起始地址
uint8_t uart1_recv_flag = 0;        // 接收完成标志位
uint16_t uart1_recv_len = 0;         // 接收的数据长度
uint16_t uart1_send_len = 0;				// 发送数据的长度
uint8_t uart1_send_flag = 0;        // 发送完成标志位


/**
 *  @brief  串口1中断控制器配置
 *  @param  无
 *  @retval 无
 *  @note   中断优先级分组全工程只配置一次，在 main 函数最开始进行配置  
 *  @note   中断处理函数在 CMSIS/stm32f10x_it.c 中进行处理   
 */
void uart1_nvic_config(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    
    // 配置串口1的中断控制器
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;          // 在 stm32f10x.h 中找 IRQn_Type 枚举
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;  // 抢占优先级
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;         // 子优先级
    NVIC_Init(&NVIC_InitStruct);
}



/**
 *  @brief  MA1 通道4, UART1_TX 中断控制器配置
 *  @param  无
 *  @retval 无
 *  @note   中断优先级分组全工程只配置一次，在 main 函数最开始进行配置  
 *  @note   中断处理函数在 CMSIS/stm32f10x_it.c 中进行处理   
 */
void DMA1_Channel4_nvic_config(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    
    // 配置串口1的中断控制器
    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel4_IRQn;   // 在 stm32f10x.h 中找 IRQn_Type 枚举
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;  // 抢占优先级
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;         // 子优先级
    NVIC_Init(&NVIC_InitStruct);
}


/**
 *  @brief  串口1 DMA初始化配置
 *  @param  无
 *  @retval 无 
 *  @note    UART1_TX -> DMA1 Channel4; UART1_RX -> DMA1 Channel5
 */
void uart1_dma_config(void)
{
    DMA_InitTypeDef DMA_InitStruct;
    
    DMA_DeInit(DMA1_Channel4);  // DMA1 通道4, UART1_TX
    DMA_DeInit(DMA1_Channel5);  // DMA1 通道5, UART1_RX
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  // 使能DMA1的时钟
    
    // 配置 DMA1 通道4, UART1_TX
    DMA_InitStruct.DMA_PeripheralBaseAddr = (USART1_BASE + 0x04);   // 数据寄存器(USART_DR) 地址偏移：0x04
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)uart1_send_data;  // 内存地址
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize = 0;      // 寄存器的内容为0时，无论通道是否开启，都不会发生任何数据传输
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStruct);
    
    // 配置 DMA1 通道5, UART1_RX
    DMA_InitStruct.DMA_PeripheralBaseAddr = (USART1_BASE + 0x04);   // 数据寄存器(USART_DR) 地址偏移：0x04
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)uart1_recv_data;  // 内存地址
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;                 // 外设到内存
    DMA_InitStruct.DMA_BufferSize = sizeof(uart1_recv_data);				//   /sizeof(uart1_recv_data[0])
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStruct);
    
    USART_DMACmd(USART1, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);// 使能DMA串口发送和接受请求
    
   // DMA1_Channel4_nvic_config();
    
    // 配置 DMA1 通道4, UART1_TX 传输完成中断
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
    
    DMA_Cmd(DMA1_Channel5, ENABLE);     // 开启接收
    DMA_Cmd(DMA1_Channel4, DISABLE);    // 禁止发送
		
		NVIC_Config(3);
}


/**
 *  @brief  串口1初始化
 *  @param  波特率
 *  @retval 无
 *  @note   UART1_TX -> PA9; UART1_RX -> PA10      
 */
void uart1_init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    
    GPIO_DeInit(GPIOA);     // 恢复 GPIOA 的寄存器到默认值
    USART_DeInit(USART1);   // 恢复 UART1 的寄存器到默认值
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,  ENABLE);   // 使能 GPIOA 的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);   // 使能 UART1 的时钟
    
    // 配置发送引脚 PA9 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 配置接收引脚 PA10 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 配置串口参数 收发一体、8n1、无流控
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
    USART_InitStruct.USART_BaudRate = baud;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStruct);
    
    //uart1_nvic_config();    // 中断控制器配置
		NVIC_Config(3);
    uart1_dma_config();     // DMA配置
    
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);  // 使能空闲中断
    
    USART_Cmd(USART1, ENABLE);  // 使能串口1  
		USART_ClearFlag(USART1, USART_FLAG_TC);
}

// 串口1的中断处理函数
void USART1_IRQHandler(void)
{   
    uint8_t clear;
    int ret;
    if( USART_GetITStatus(USART1, USART_IT_IDLE) != RESET )
    {    
				clear = clear;
        clear = USART1->SR; // 清除空闲中断, 由软件序列清除该位(先读USART_SR，然后读USART_DR)
        clear = USART1->DR; // 清除空闲中断
        
        DMA_Cmd(DMA1_Channel5, DISABLE);

        
        // 统计收到的数据的长度
        uart1_recv_len = sizeof(uart1_recv_data) - DMA_GetCurrDataCounter(DMA1_Channel5);
			
				ret = secserial_encrypt(&ctx_a, uart1_recv_data, uart1_recv_len, pkt_buffer_len);
				if(ret>0)	 
				{
					memcpy(uart2_send_data,uart1_recv_data,ret);		// dst  src  num
					uart2_send_len = ret;
		      uart1_recv_flag = 1;                // 接收标志置1
				}
				else
				{
					uart2_send_len = 0;
					uart1_recv_flag = 0;                // 接收标志置0
				//	memset(uart1_send_data,'\0', sizeof(uart1_send_data));
				}
				
				clear_mem(uart1_recv_data);
				usart1_recv_dma();
    }
}


// DMA1 通道4, UART1_TX 传输完成中断
void DMA1_Channel4_IRQHandler(void)
{
    if( DMA_GetITStatus(DMA1_IT_TC4) != RESET ) // DMA1 通道4, UART1_TX 传输完成
    {
        DMA_ClearITPendingBit(DMA1_IT_TC4);     // 清除中断
        
        uart1_send_flag = 1;
        DMA_Cmd(DMA1_Channel4, DISABLE);        // 关闭 DMA1 通道4, UART1_TX
    }
}

void usart1_send_dma(void)
{
   DMA_Cmd(DMA1_Channel4, DISABLE);    // 关闭 DMA1 通道4, UART1_TX
   DMA_SetCurrDataCounter(DMA1_Channel4, uart1_send_len);  // 传输数量寄存器只能在通道不工作(DMA_CCRx的EN=0)时写入
   DMA_Cmd(DMA1_Channel4, ENABLE);    // 开启 DMA1 通道4, UART1_TX
}

void usart1_recv_dma(void)
{
          
   DMA_Cmd(DMA1_Channel5, DISABLE);    // DMA1 通道5, UART1_RX
   DMA_SetCurrDataCounter(DMA1_Channel5, sizeof(uart1_recv_data)); 
   DMA_Cmd(DMA1_Channel5, ENABLE);     // DMA1 通道5, UART1_RX
}	

void clear_mem(uint8_t* buf)
{
		memset(&buf, '\0', sizeof(&buf));  // 清空接收缓冲区 /sizeof(uart1_recv_data[0])
	//	memset(uart2_recv_data, '\0', sizeof(uart2_recv_data));  // 清空接收缓冲区 /sizeof(uart1_recv_data[0])
}
