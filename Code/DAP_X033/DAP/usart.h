#ifndef _USART_H
#define _USART_H

#include "main.h"

	// Usart2_DMA_Init();
    // Usart2_Init();
    // USART2_DMA_Recv_Data(U2_Rxbuf,U2_DATA_BUF_LEN);

void USART4_Init(void);
void USART4_DMA_Init(void);
void USART4_DMA_Recv(void);
void USART4_DMA_Send(uint8_t *data, uint32_t datalen);
uint8_t USART4_Send_IsBusy(void);
void USART4_Config_Linecoding(uint8_t LineCoding[7]);

#endif
