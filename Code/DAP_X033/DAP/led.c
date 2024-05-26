#include "led.h"
#include "dap_main.h"
#include "cdc_main.h"

static void TIM3_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM3, ENABLE);
}

void LED_Init(void)
{
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOA;
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;

    GPIOA->CFGHR &= ~(0x0F << (4 * 1)); // LED1,GPIOA9
    GPIOA->CFGHR |= 0x03 << (4 * 1);
    GPIOA->BSHR = 1 << 9;

    GPIOC->CFGLR &= ~(0x0F << (4 * 3)); // LED2,GPIOC3
    GPIOC->CFGLR |= 0x03 << (4 * 3);
    GPIOC->BSHR = 1 << 3;

    TIM3_Init(1000 - 1, 48 - 1);
}

static void LED_Task(void)
{
    static uint32_t LED_Task_Cnt = 0;
    static uint32_t DAP_In_Active_Cnt = 0;
    static uint32_t CDC_In_Active_Cnt = 0;

    if (DAP_Is_Active == 1)
        DAP_In_Active_Cnt = 0;
    else if (DAP_In_Active_Cnt < 500)
        DAP_In_Active_Cnt++;
    DAP_Is_Active = 0;

    if (DAP_In_Active_Cnt >= 500)
    {
        GPIOA->BCR = 1 << 9; // LED1
    }
    else
    {
        if (LED_Task_Cnt % 100 == 0)
        {
            if (GPIOA->OUTDR & (1 << 9))
                GPIOA->BCR = 1 << 9;
            else
                GPIOA->BSHR = 1 << 9;
        }
    }

    if (CDC_Is_Active == 1)
        CDC_In_Active_Cnt = 0;
    else if (CDC_In_Active_Cnt < 500)
        CDC_In_Active_Cnt++;
    CDC_Is_Active = 0;

    if (CDC_In_Active_Cnt >= 500)
    {
        GPIOC->BCR = 1 << 3; // LED2
    }
    else
    {
        if (LED_Task_Cnt % 100 == 0)
        {
            if (GPIOC->OUTDR & (1 << 3))
                GPIOC->BCR = 1 << 3;
            else
                GPIOC->BSHR = 1 << 3;
        }
    }

    LED_Task_Cnt++;
}

void TIM3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

        LED_Task();
    }
}
