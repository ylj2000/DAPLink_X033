#ifndef _DAP_MAIN_H_
#define _DAP_MAIN_H_

#include "main.h"

extern volatile uint8_t DAP_USB_IsBulk;
extern volatile uint8_t DAP_Is_Active;
extern volatile uint8_t DIR_IsEN;

void DAP_RX_CallBack(uint8_t *buf, uint32_t len);
void DAP_TX_CallBack(void);
void DAP_Init(void);
void DAP_Task(void);

#endif
