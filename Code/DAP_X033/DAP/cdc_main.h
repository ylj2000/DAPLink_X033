#ifndef _CDC_MAIN_H
#define _CDC_MAIN_H

#include "main.h"

#define CDC_BUFFER_SIZE 1024
#define CDC_EP_SIZE 64

extern volatile uint8_t CDC_Is_Active;

void CDC_RX_CallBack(uint8_t *buf, uint32_t len);
void CDC_TX_CallBack(void);
void UART_RX_CallBack(uint8_t *buf, uint32_t len);
void UART_TX_CallBack(void);
void CDC_Init(void);
void CDC_Task(void);

#endif