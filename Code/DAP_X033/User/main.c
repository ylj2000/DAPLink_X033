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

    RCC->APB2PCENR |= RCC_APB2Periph_GPIOA;
    GPIOA->CFGLR &= ~(0x0F << (4 * 4)); // RST,GPIOA4
    GPIOA->CFGLR |= 0x08 << (4 * 4);
    GPIOA->BSHR = 1 << 4;

    for (volatile uint32_t i = 0; i < 100; i++)
    {
        __NOP();
    }

    uint8_t temp = (GPIOA->INDR >> 4) & 0x01;
    if (temp == 0)
    {
        SystemReset_StartMode(Start_Mode_BOOT);
        NVIC_SystemReset();
    }

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
