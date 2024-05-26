#ifndef _USB_DESC_H
#define _USB_DESC_H

#include "main.h"

#define DAP_BULK_IN_EP 0x81
#define DAP_BULK_OUT_EP 0x01
#define DAP_BULK_SIZE 64

#define DAP_HID_IN_EP 0x82
#define DAP_HID_OUT_EP 0x02
#define DAP_HID_SIZE 64

#define CDC_IN_EP 0x83
#define CDC_OUT_EP 0x03
#define CDC_INT_EP 0x84
#define CDC_SIZE 64

#define USBD_VID 0xFAED
#define USBD_PID 0x4873

#define USBD_MAX_POWER 500
#define USBD_LANGID_STRING 1033

void DAP_USB_Init(void);
void DAP_Bulk_IN(uint8_t *buf, uint32_t len);
uint8_t DAP_Bulk_IN_IsBusy(void);
void DAP_HID_IN(uint8_t *buf, uint32_t len);
uint8_t DAP_HID_IN_IsBusy(void);
void CDC_IN(uint8_t *buf, uint32_t len);
uint8_t CDC_IN_IsBusy(void);
void CDC_RX_Start(void);

#endif
