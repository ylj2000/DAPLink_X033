#include "cdc_main.h"
#include "buffer.h"
#include "usb_desc.h"
#include "usart.h"

volatile uint8_t CDC_Is_Active = 0;

buffer_t usb2uart_buffer;
buffer_t uart2usb_buffer;

uint8_t uart2usb_buf[CDC_EP_SIZE];
unsigned int uart2usb_txlen = 0;
uint8_t usb2uart_buf[CDC_EP_SIZE];
unsigned int usb2uart_txlen = 0;

void CDC_RX_CallBack(uint8_t *buf, uint32_t len)
{
    buffer_in(&usb2uart_buffer, buf, len);
}

void CDC_TX_CallBack(void)
{
    buffer_out(&uart2usb_buffer, NULL, &uart2usb_txlen, uart2usb_txlen);
}

void UART_RX_CallBack(uint8_t *buf, uint32_t len)
{
    buffer_in(&uart2usb_buffer, buf, len);
}

void UART_TX_CallBack(void)
{
    buffer_out(&usb2uart_buffer, NULL, &usb2uart_txlen, usb2uart_txlen);
    CDC_RX_Start();
}

void CDC_Init(void)
{
    buffer_init(&usb2uart_buffer, CDC_BUFFER_SIZE);
    buffer_init(&uart2usb_buffer, CDC_BUFFER_SIZE);
}

void CDC_Task(void)
{
    if (!CDC_IN_IsBusy())
    {
        if (buffer_out_without_decrease(&uart2usb_buffer, uart2usb_buf, &uart2usb_txlen, sizeof(uart2usb_buf)) == 0)
        {
            CDC_IN(uart2usb_buf, uart2usb_txlen);

            CDC_Is_Active = 1;
        }
    }

    if (!USART4_Send_IsBusy())
    {
        if (buffer_out_without_decrease(&usb2uart_buffer, usb2uart_buf, &usb2uart_txlen, sizeof(usb2uart_buf)) == 0)
        {
            USART4_DMA_Send(usb2uart_buf, usb2uart_txlen);

            CDC_Is_Active = 1;
        }
    }
}
