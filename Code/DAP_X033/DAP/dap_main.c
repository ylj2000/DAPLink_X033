#include "dap_main.h"
#include "string.h"

#include "usb_desc.h"

#include "DAP_config.h"
#include "DAP.h"

volatile uint8_t DAP_USB_IsBulk = 0;
volatile uint8_t DAP_Is_Active = 0;
volatile uint8_t DIR_IsEN = 0;

static volatile uint32_t DAP_RequestIndexI = 0; // Request  Index In
static volatile uint32_t DAP_RequestIndexO = 0; // Request  Index Out
static volatile uint32_t DAP_RequestCountI = 0; // Request  Count In
static volatile uint32_t DAP_RequestCountO = 0; // Request  Count Out

static volatile uint32_t DAP_ResponseIndexI = 0; // Response Index In
static volatile uint32_t DAP_ResponseIndexO = 0; // Response Index Out
static volatile uint32_t DAP_ResponseCountI = 0; // Response Count In
static volatile uint32_t DAP_ResponseCountO = 0; // Response Count Out

static uint8_t DAP_Request[DAP_PACKET_COUNT][DAP_PACKET_SIZE + 2];  // Request  Buffer,last 2 is length
static uint8_t DAP_Response[DAP_PACKET_COUNT][DAP_PACKET_SIZE + 2]; // Response Buffer,last 2 is length

void DAP_RX_CallBack(uint8_t *buf, uint32_t len)
{
    if (DAP_RequestCountI - DAP_RequestCountO == DAP_PACKET_COUNT)
    {
        return;
    }

    memcpy(DAP_Request[DAP_RequestIndexI], buf, len);
    *(uint16_t *)(DAP_Request[DAP_RequestIndexI] + DAP_PACKET_SIZE - 1) = len;

    if (DAP_Request[DAP_RequestIndexI][0] == ID_DAP_TransferAbort)
    {
        DAP_TransferAbort = 1U;
        return;
    }

    DAP_RequestIndexI++;
    if (DAP_RequestIndexI == DAP_PACKET_COUNT)
    {
        DAP_RequestIndexI = 0U;
    }
    DAP_RequestCountI++;
}

void DAP_TX_CallBack(void)
{
    DAP_ResponseIndexO++;
    if (DAP_ResponseIndexO == DAP_PACKET_COUNT)
    {
        DAP_ResponseIndexO = 0;
    }
    DAP_ResponseCountO++;
}

void DAP_Init(void)
{
    DAP_Setup();

    RCC->APB2PCENR |= RCC_APB2Periph_GPIOA;
    GPIOA->CFGLR &= ~(0x0F << (4 * 5)); // DIR,GPIOA5
    GPIOA->CFGLR |= 0x08 << (4 * 5);
    GPIOA->BCR = 1 << 5;

    for (volatile uint32_t i = 0; i < 100; i++)
    {
        __NOP();
    }

    uint8_t temp = (GPIOA->INDR >> 5) & 0x01;
    if (temp == 0)
    {
        DIR_IsEN = 1;
    }

    if (DIR_IsEN == 0)
    {
        GPIOA->CFGLR &= ~(0x0F << (4 * 5)); // DIR,GPIOA5
    }
    else
    {
        GPIOA->CFGLR &= ~(0x0F << (4 * 5)); // DIR,GPIOA5
        GPIOA->CFGLR |= 0x03 << (4 * 5);
        GPIOA->BCR = 1 << 5;
    }
}

void DAP_Task(void)
{
    if (DAP_RequestCountI != DAP_RequestCountO)
    {
        // Handle Queue Commands
        if (DAP_Request[DAP_RequestIndexO][0] == ID_DAP_QueueCommands)
            DAP_Request[DAP_RequestIndexO][0] = ID_DAP_ExecuteCommands;

        uint32_t num = 0;
        num = DAP_ExecuteCommand(DAP_Request[DAP_RequestIndexO], DAP_Response[DAP_ResponseIndexI]);
        *(uint16_t *)(DAP_Response[DAP_ResponseIndexI] + DAP_PACKET_SIZE - 1) = num & 0x0000FFFF;

        DAP_RequestIndexO++;
        if (DAP_RequestIndexO == DAP_PACKET_COUNT)
        {
            DAP_RequestIndexO = 0U;
        }
        DAP_RequestCountO++;

        DAP_ResponseIndexI++;
        if (DAP_ResponseIndexI == DAP_PACKET_COUNT)
        {
            DAP_ResponseIndexI = 0U;
        }
        DAP_ResponseCountI++;

        DAP_Is_Active = 1;
    }

    if (DAP_USB_IsBulk)
    {
        if (DAP_Bulk_IN_IsBusy())
            return;
        if (DAP_ResponseCountI != DAP_ResponseCountO)
        {
            DAP_Bulk_IN(DAP_Response[DAP_ResponseIndexO], *(uint16_t *)(DAP_Response[DAP_ResponseIndexO] + DAP_PACKET_SIZE - 1));
        }
    }
    else
    {
        if (DAP_HID_IN_IsBusy())
            return;
        if (DAP_ResponseCountI != DAP_ResponseCountO)
        {
            DAP_HID_IN(DAP_Response[DAP_ResponseIndexO], *(uint16_t *)(DAP_Response[DAP_ResponseIndexO] + DAP_PACKET_SIZE - 1));
        }
    }
}
