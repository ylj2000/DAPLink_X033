#include "usart.h"
#include "cdc_main.h"

#define U4_DATA_BUF_LEN CDC_EP_SIZE

volatile uint8_t USART4_TX_Flag = 0; // 0:IDLE,1:TX
uint8_t U4_Rxbuf[U4_DATA_BUF_LEN];

void USART4_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART4, &USART_InitStructure);

    USART_ITConfig(USART4, USART_IT_IDLE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART4, ENABLE);
}

void USART4_Config_Linecoding(uint8_t LineCoding[7])
{
    USART_InitTypeDef USART_InitStructure = {0};

    uint32_t dataRat;
    uint8_t stopBit, parityType, dataBit;

    dataRat = (LineCoding[3] << 24) | (LineCoding[2] << 16) | (LineCoding[1] << 8) | LineCoding[0];
    stopBit = LineCoding[4];
    parityType = LineCoding[5];
    dataBit = LineCoding[6];

    if(dataRat == 0) return;

    USART_InitStructure.USART_BaudRate = dataRat;

    if (stopBit == 0)
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
    else if (stopBit == 1)
        USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
    else if (stopBit == 2)
        USART_InitStructure.USART_StopBits = USART_StopBits_2;
    else
        USART_InitStructure.USART_StopBits = USART_StopBits_1;

    if (parityType == 0)
        USART_InitStructure.USART_Parity = USART_Parity_No;
    else if (parityType == 1)
        USART_InitStructure.USART_Parity = USART_Parity_Odd;
    else if (parityType == 2)
        USART_InitStructure.USART_Parity = USART_Parity_Even;
    else
        USART_InitStructure.USART_Parity = USART_Parity_No;
    
    if(dataBit == 8)
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    else if(dataBit == 9)
        USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    else
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;

    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART4, &USART_InitStructure);
    USART4_DMA_Recv();
}

void USART4_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART4->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = 0;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_ClearFlag(DMA1_FLAG_TC1);
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

    DMA_DeInit(DMA1_Channel8);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART4->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)U4_Rxbuf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = U4_DATA_BUF_LEN;
    DMA_Init(DMA1_Channel8, &DMA_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel8_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_ClearFlag(DMA1_FLAG_TC8);
    DMA_ITConfig(DMA1_Channel8, DMA_IT_TC, ENABLE);
}

void USART4_DMA_Recv(void)
{
    DMA_Cmd(DMA1_Channel8, DISABLE);

    DMA1_Channel8->MADDR = (u32)U4_Rxbuf;
    DMA1_Channel8->CNTR = U4_DATA_BUF_LEN;

    DMA_Cmd(DMA1_Channel8, ENABLE);
    USART_DMACmd(USART4, USART_DMAReq_Rx, ENABLE);
}

void USART4_DMA_Send(uint8_t *data, uint32_t datalen)
{
    while (USART4_TX_Flag == 1)
        ;
    USART4_TX_Flag = 1;

    DMA_Cmd(DMA1_Channel1, DISABLE);

    DMA1_Channel1->MADDR = (uint32_t)data;
    DMA1_Channel1->CNTR = datalen;

    DMA_Cmd(DMA1_Channel1, ENABLE);
    USART_DMACmd(USART4, USART_DMAReq_Tx, ENABLE);
}

uint8_t USART4_Send_IsBusy(void)
{
    return USART4_TX_Flag;
}

void USART4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART4_IRQHandler(void)
{
    if (USART_GetITStatus(USART4, USART_IT_IDLE) == SET)
    {
        UART_RX_CallBack(U4_Rxbuf, U4_DATA_BUF_LEN - DMA1_Channel8->CNTR);

        USART4_DMA_Recv();

        // Clear IDLE IT
        volatile uint8_t temp = 0;
        temp = USART4->STATR;
        temp = USART4->DATAR;
        if (temp != 0)
            temp = 0;
    }
}

void DMA1_Channel1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel1_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC1) == SET)
    {
        UART_TX_CallBack();

        USART4_TX_Flag = 0;

        DMA_ClearITPendingBit(DMA1_IT_TC1);
    }
}

void DMA1_Channel8_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel8_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC8) == SET)
    {
        UART_RX_CallBack(U4_Rxbuf, U4_DATA_BUF_LEN - DMA1_Channel8->CNTR);

        USART4_DMA_Recv();

        DMA_ClearITPendingBit(DMA1_IT_TC8);
    }
}
