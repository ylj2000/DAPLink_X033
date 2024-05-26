#include "main.h"

#include "dap_main.h"
#include "cdc_main.h"
#include "usb_desc.h"
#include "usart.h"
#include "led.h"

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();

    DAP_Init();
    DAP_USB_Init();

    CDC_Init();
    USART4_DMA_Init();
    USART4_Init();
    USART4_DMA_Recv();

    LED_Init();

    while (1)
    {
        DAP_Task();
        CDC_Task();
    }
}
