#include "usb_desc.h"
#include "usbd_core.h"
#include "usbd_cdc.h"
#include "usbd_hid.h"

#include "dap_main.h"
#include "cdc_main.h"
#include "usart.h"

#define USB_IOEN 0x00000080
#define USB_PHY_V33 0x00000040
#define UDP_PUE_MASK 0x0000000C
#define UDP_PUE_DISABLE 0x00000000
#define UDP_PUE_35UA 0x00000004
#define UDP_PUE_10K 0x00000008
#define UDP_PUE_1K5 0x0000000C

#define UDM_PUE_MASK 0x00000003
#define UDM_PUE_DISABLE 0x00000000
#define UDM_PUE_35UA 0x00000001
#define UDM_PUE_10K 0x00000002
#define UDM_PUE_1K5 0x00000003

void usb_dc_low_level_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBFS, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_16;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_17;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    if (PWR_VDD_SupplyVoltage() == PWR_VDD_5V) // to be tested
        AFIO->CTLR = (AFIO->CTLR & ~(UDP_PUE_MASK | UDM_PUE_MASK | USB_PHY_V33)) | UDP_PUE_10K | USB_IOEN;
    else
        AFIO->CTLR = (AFIO->CTLR & ~(UDP_PUE_MASK | UDM_PUE_MASK)) | USB_PHY_V33 | UDP_PUE_1K5 | USB_IOEN;

    NVIC_InitTypeDef NVIC_InitStructure = {0};
    NVIC_InitStructure.NVIC_IRQChannel = USBFS_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static uint8_t DAP_Bulk_RX_Buf[DAP_BULK_SIZE] __attribute__((aligned(4)));
static uint8_t DAP_Bulk_TX_Buf[DAP_BULK_SIZE] __attribute__((aligned(4)));
static uint8_t DAP_HID_RX_Buf[DAP_HID_SIZE] __attribute__((aligned(4)));
static uint8_t DAP_HID_TX_Buf[DAP_HID_SIZE] __attribute__((aligned(4)));
static uint8_t CDC_RX_Buf[CDC_SIZE] __attribute__((aligned(4)));
static uint8_t CDC_TX_Buf[CDC_SIZE] __attribute__((aligned(4)));

static volatile uint8_t DAP_Bulk_IN_Busy = 0, DAP_HID_IN_Busy = 0, CDC_IN_Busy = 0;

#define DAP_BULK_INTERFACE_SIZE (9 + 7 + 7)
#define DAP_HID_INTERFACE_SIZE (9 + 9 + 7 + 7)
#define USB_CONFIG_SIZE (9 + DAP_BULK_INTERFACE_SIZE + DAP_HID_INTERFACE_SIZE + CDC_ACM_DESCRIPTOR_LEN)

#define HID_DESCRIPTOR_SIZE 36

static uint8_t dap_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_1, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0100, 0x01),

    // bNumInterfaces: 1(dap hid)+1(dap bulk)+2(cdc)=4
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 4, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),

    // bInterfaceClass:0xFF,Vendor Specific
    // bInterfaceSubClass:0x00
    // bInterfaceProtocol:0x00
    // iInterface:0x04,string4
    USB_INTERFACE_DESCRIPTOR_INIT(0x00, 0x00, 0x02, 0xFF, 0x00, 0x00, 0x04),

    USB_ENDPOINT_DESCRIPTOR_INIT(DAP_BULK_OUT_EP, USB_ENDPOINT_TYPE_BULK, DAP_BULK_SIZE, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(DAP_BULK_IN_EP, USB_ENDPOINT_TYPE_BULK, DAP_BULK_SIZE, 0x00),

    // bInterfaceClass:0x03,HID
    // bInterfaceSubClass:0x00
    // bInterfaceProtocol:0x00
    // iInterface:0x05,string5
    USB_INTERFACE_DESCRIPTOR_INIT(0x01, 0x00, 0x02, 0x03, 0x00, 0x00, 0x05),
    /******************** Descriptor of Custom HID ********************/
    0x09,                    /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE_HID, /* bDescriptorType: HID */
    0x10,                    /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,                /* bCountryCode: Hardware target country */
    0x01,                /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,                /* bDescriptorType */
    HID_DESCRIPTOR_SIZE, /* wItemLength: Total length of Report descriptor */
    0x00,

    USB_ENDPOINT_DESCRIPTOR_INIT(DAP_HID_OUT_EP, USB_ENDPOINT_TYPE_INTERRUPT, DAP_HID_SIZE, 0x01),
    USB_ENDPOINT_DESCRIPTOR_INIT(DAP_HID_IN_EP, USB_ENDPOINT_TYPE_INTERRUPT, DAP_HID_SIZE, 0x01),

    // str_idx:0x06,string6
    CDC_ACM_DESCRIPTOR_INIT(0x02, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_SIZE, 0x06),

    /* String 0 (LANGID) */
    USB_LANGID_INIT(USBD_LANGID_STRING),
    /* String 1 (Manufacturer) */
    12,                         /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'H', 0x00,                  /* wcChar0 */
    'o', 0x00,                  /* wcChar1 */
    'r', 0x00,                  /* wcChar2 */
    'c', 0x00,                  /* wcChar3 */
    'o', 0x00,                  /* wcChar4 */
    /* String 2 (Product) */
    32,                         /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'H', 0x00,                  /* wcChar0 */
    'o', 0x00,                  /* wcChar1 */
    'r', 0x00,                  /* wcChar2 */
    'c', 0x00,                  /* wcChar3 */
    'o', 0x00,                  /* wcChar4 */
    ' ', 0x00,                  /* wcChar5 */
    'C', 0x00,                  /* wcChar6 */
    'M', 0x00,                  /* wcChar7 */
    'S', 0x00,                  /* wcChar8 */
    'I', 0x00,                  /* wcChar9 */
    'S', 0x00,                  /* wcChar10 */
    '-', 0x00,                  /* wcChar11 */
    'D', 0x00,                  /* wcChar12 */
    'A', 0x00,                  /* wcChar13 */
    'P', 0x00,                  /* wcChar14 */
    /* String 3 (Serial Number) */
    0x1A,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '1', 0x00,                  /* wcChar0 */
    '3', 0x00,                  /* wcChar1 */
    '1', 0x00,                  /* wcChar2 */
    '4', 0x00,                  /* wcChar3 */
    '0', 0x00,                  /* wcChar4 */
    '5', 0x00,                  /* wcChar5 */
    '2', 0x00,                  /* wcChar6 */
    '1', 0x00,                  /* wcChar7 */
    '3', 0x00,                  /* wcChar8 */
    '3', 0x00,                  /* wcChar9 */
    '3', 0x00,                  /* wcChar10 */
    '3', 0x00,                  /* wcChar11 */
    /* String 4 (Interface) */
    38,                         /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'H', 0x00,                  /* wcChar0 */
    'o', 0x00,                  /* wcChar1 */
    'r', 0x00,                  /* wcChar2 */
    'c', 0x00,                  /* wcChar3 */
    'o', 0x00,                  /* wcChar4 */
    ' ', 0x00,                  /* wcChar5 */
    'C', 0x00,                  /* wcChar6 */
    'M', 0x00,                  /* wcChar7 */
    'S', 0x00,                  /* wcChar8 */
    'I', 0x00,                  /* wcChar9 */
    'S', 0x00,                  /* wcChar10 */
    '-', 0x00,                  /* wcChar11 */
    'D', 0x00,                  /* wcChar12 */
    'A', 0x00,                  /* wcChar13 */
    'P', 0x00,                  /* wcChar14 */
    ' ', 0x00,                  /* wcChar15 */
    'v', 0x00,                  /* wcChar16 */
    '2', 0x00,                  /* wcChar17 */
                                //     /* String 5 (Interface) */
    38,                         /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'H', 0x00,                  /* wcChar0 */
    'o', 0x00,                  /* wcChar1 */
    'r', 0x00,                  /* wcChar2 */
    'c', 0x00,                  /* wcChar3 */
    'o', 0x00,                  /* wcChar4 */
    ' ', 0x00,                  /* wcChar5 */
    'C', 0x00,                  /* wcChar6 */
    'M', 0x00,                  /* wcChar7 */
    'S', 0x00,                  /* wcChar8 */
    'I', 0x00,                  /* wcChar9 */
    'S', 0x00,                  /* wcChar10 */
    '-', 0x00,                  /* wcChar11 */
    'D', 0x00,                  /* wcChar12 */
    'A', 0x00,                  /* wcChar13 */
    'P', 0x00,                  /* wcChar14 */
    ' ', 0x00,                  /* wcChar15 */
    'v', 0x00,                  /* wcChar16 */
    '1', 0x00,                  /* wcChar17 */
                                //     /* String 6 (Interface) */
    20,                         // bLength
    USB_DESCRIPTOR_TYPE_STRING, // bDescriptorType
    'H', 0x00,                  /* wcChar0 */
    'o', 0x00,                  /* wcChar1 */
    'r', 0x00,                  /* wcChar2 */
    'c', 0x00,                  /* wcChar3 */
    'o', 0x00,                  /* wcChar4 */
    ' ', 0x00,                  /* wcChar5 */
    'C', 0,                     /* wcChar6 */
    'D', 0,                     /* wcChar7 */
    'C', 0,                     /* wcChar8 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0A,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0xFF,
    0xFF,
    0xFF,
    0x40,
    0x01,
    0x00,
#endif
    0x00};

#define USBD_WINUSB_NUM 1
#define FUNCTION_SUBSET_LEN 160
#define DEVICE_INTERFACE_GUIDS_FEATURE_LEN 132
#define USBD_WINUSB_DESC_SET_LEN (WINUSB_DESCRIPTOR_SET_HEADER_SIZE + USBD_WINUSB_NUM * FUNCTION_SUBSET_LEN)

static const uint8_t dap_winusb_descriptor[] = {
    WBVAL(WINUSB_DESCRIPTOR_SET_HEADER_SIZE), /* wLength */
    WBVAL(WINUSB_SET_HEADER_DESCRIPTOR_TYPE), /* wDescriptorType */
    0x00, 0x00, 0x03, 0x06, /* >= Win 8.1 */  /* dwWindowsVersion*/
    WBVAL(USBD_WINUSB_DESC_SET_LEN),          /* wDescriptorSetTotalLength */

    WBVAL(WINUSB_FUNCTION_SUBSET_HEADER_SIZE), /* wLength */
    WBVAL(WINUSB_SUBSET_HEADER_FUNCTION_TYPE), /* wDescriptorType */
    0,                                         /* bFirstInterface USBD_BULK_IF_NUM*/
    0,                                         /* bReserved */
    WBVAL(FUNCTION_SUBSET_LEN),                /* wSubsetLength */

    WBVAL(WINUSB_FEATURE_COMPATIBLE_ID_SIZE),  /* wLength */
    WBVAL(WINUSB_FEATURE_COMPATIBLE_ID_TYPE),  /* wDescriptorType */
    'W', 'I', 'N', 'U', 'S', 'B', 0, 0,        /* CompatibleId*/
    0, 0, 0, 0, 0, 0, 0, 0,                    /* SubCompatibleId*/
    WBVAL(DEVICE_INTERFACE_GUIDS_FEATURE_LEN), /* wLength */
    WBVAL(WINUSB_FEATURE_REG_PROPERTY_TYPE),   /* wDescriptorType */
    WBVAL(WINUSB_PROP_DATA_TYPE_REG_MULTI_SZ), /* wPropertyDataType */
    WBVAL(42),                                 /* wPropertyNameLength */
    'D', 0, 'e', 0, 'v', 0, 'i', 0, 'c', 0, 'e', 0,
    'I', 0, 'n', 0, 't', 0, 'e', 0, 'r', 0, 'f', 0, 'a', 0, 'c', 0, 'e', 0,
    'G', 0, 'U', 0, 'I', 0, 'D', 0, 's', 0, 0, 0,
    WBVAL(80), /* wPropertyDataLength */
    '{', 0,
    'C', 0, 'D', 0, 'B', 0, '3', 0, 'B', 0, '5', 0, 'A', 0, 'D', 0, '-', 0,
    '2', 0, '9', 0, '3', 0, 'B', 0, '-', 0,
    '4', 0, '6', 0, '6', 0, '3', 0, '-', 0,
    'A', 0, 'A', 0, '3', 0, '6', 0, '-',
    0, '1', 0, 'A', 0, 'A', 0, 'E', 0, '4', 0, '6', 0, '4', 0, '6', 0, '3', 0, '7', 0, '7', 0, '6', 0,
    '}', 0, 0, 0, 0, 0};

#define USBD_WINUSB_VENDOR_CODE 0x01 //

#define USBD_WINUSB_DESC_LEN 28
#define USBD_BOS_WTOTALLENGTH (0x05 + USBD_WINUSB_DESC_LEN * USBD_WINUSB_NUM)

static const uint8_t dap_bos_descriptor[] = {
    0x05,                         /* bLength */
    0x0f,                         /* bDescriptorType */
    WBVAL(USBD_BOS_WTOTALLENGTH), /* wTotalLength */
    USBD_WINUSB_NUM,              /* bNumDeviceCaps */

    USBD_WINUSB_DESC_LEN,           /* bLength */
    0x10,                           /* bDescriptorType */
    USB_DEVICE_CAPABILITY_PLATFORM, /* bDevCapabilityType */
    0x00,                           /* bReserved */
    0xDF, 0x60, 0xDD, 0xD8,         /* PlatformCapabilityUUID */
    0x89, 0x45, 0xC7, 0x4C,
    0x9C, 0xD2, 0x65, 0x9D,
    0x9E, 0x64, 0x8A, 0x9F,
    0x00, 0x00, 0x03, 0x06, /* >= Win 8.1 */ /* dwWindowsVersion*/
    WBVAL(USBD_WINUSB_DESC_SET_LEN),         /* wDescriptorSetTotalLength */
    USBD_WINUSB_VENDOR_CODE,                 /* bVendorCode */
    0,                                       /* bAltEnumCode */
};

static struct usb_bos_descriptor bos_desc = {
    .string = (uint8_t *)&dap_bos_descriptor[0],
    .string_len = USBD_BOS_WTOTALLENGTH};

static struct usb_msosv2_descriptor msosv2_desc = {
    .vendor_code = USBD_WINUSB_VENDOR_CODE,
    .compat_id = (uint8_t *)&dap_winusb_descriptor[0],
    .compat_id_len = USBD_WINUSB_DESC_SET_LEN,
};

static const uint8_t dap_hid_descriptor[] = {
    0x06, 0x00, 0xff,
    0x09, 0x01, 0xa1, 0x01, 0x15, 0x00, 0x26, 0xff, 0x00, 0x75, 0x08, 0x96, WBVAL(DAP_HID_SIZE),
    0x09, 0x01, 0x81, 0x02, 0x96, WBVAL(DAP_HID_SIZE),
    0x09, 0x01, 0x91, 0x02, 0x96, WBVAL(DAP_HID_SIZE),
    0x09, 0x01, 0xb1, 0x02,
    0xC0};

static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    switch (event)
    {
    case USBD_EVENT_RESET:
        break;
    case USBD_EVENT_CONNECTED:
        break;
    case USBD_EVENT_DISCONNECTED:
        break;
    case USBD_EVENT_RESUME:
        break;
    case USBD_EVENT_SUSPEND:
        break;
    case USBD_EVENT_CONFIGURED:
        /* setup first out ep read transfer */
        usbd_ep_start_read(0, DAP_BULK_OUT_EP, DAP_Bulk_RX_Buf, DAP_BULK_SIZE);
        usbd_ep_start_read(0, DAP_HID_OUT_EP, DAP_HID_RX_Buf, DAP_HID_SIZE);
        usbd_ep_start_read(0, CDC_OUT_EP, CDC_RX_Buf, CDC_SIZE);
        break;
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;

    default:
        break;
    }
}

static void DAP_Bulk_IN_CallBack(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    DAP_Bulk_IN_Busy = false;
    if (DAP_USB_IsBulk)
        DAP_TX_CallBack();
}

static void DAP_Bulk_OUT_CallBack(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    DAP_USB_IsBulk = true;
    DAP_RX_CallBack(DAP_Bulk_RX_Buf, nbytes);
    usbd_ep_start_read(0, DAP_BULK_OUT_EP, DAP_Bulk_RX_Buf, DAP_BULK_SIZE);
}

void DAP_Bulk_IN(uint8_t *buf, uint32_t len)
{
    if (DAP_Bulk_IN_Busy)
        return;
    DAP_Bulk_IN_Busy = true;
    memcpy(DAP_Bulk_TX_Buf, buf, len);
    usbd_ep_start_write(0, DAP_BULK_IN_EP, DAP_Bulk_TX_Buf, len);
}

uint8_t DAP_Bulk_IN_IsBusy(void)
{
    return DAP_Bulk_IN_Busy;
}

static void DAP_HID_IN_CallBack(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    DAP_HID_IN_Busy = false;
    if (!DAP_USB_IsBulk)
        DAP_TX_CallBack();
}

static void DAP_HID_OUT_CallBack(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    DAP_USB_IsBulk = false;
    DAP_RX_CallBack(DAP_HID_RX_Buf, nbytes);
    usbd_ep_start_read(0, DAP_HID_OUT_EP, DAP_HID_RX_Buf, DAP_HID_SIZE);
}

void DAP_HID_IN(uint8_t *buf, uint32_t len)
{
    if (DAP_HID_IN_Busy)
        return;
    DAP_HID_IN_Busy = true;
    memcpy(DAP_HID_TX_Buf, buf, len);
    usbd_ep_start_write(0, DAP_HID_IN_EP, DAP_HID_TX_Buf, DAP_HID_SIZE);
}

uint8_t DAP_HID_IN_IsBusy(void)
{
    return DAP_HID_IN_Busy;
}

static void CDC_IN_CallBack(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    CDC_IN_Busy = false;
    CDC_TX_CallBack();
}

static void CDC_OUT_CallBack(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    CDC_RX_CallBack(CDC_RX_Buf, nbytes);
}

void CDC_RX_Start(void)
{
    usbd_ep_start_read(0, CDC_OUT_EP, CDC_RX_Buf, CDC_SIZE);
}

void CDC_IN(uint8_t *buf, uint32_t len)
{
    if (CDC_IN_Busy)
        return;
    CDC_IN_Busy = true;
    memcpy(CDC_TX_Buf, buf, len);
    usbd_ep_start_write(0, CDC_IN_EP, CDC_TX_Buf, len);
}

uint8_t CDC_IN_IsBusy(void)
{
    return CDC_IN_Busy;
}

static struct usbd_interface DAP_Bulk_Interface;
static struct usbd_endpoint DAP_Bulk_OUT_Endpoint = {
    .ep_addr = DAP_BULK_OUT_EP,
    .ep_cb = DAP_Bulk_OUT_CallBack,
};
static struct usbd_endpoint DAP_Bulk_IN_Endpoint = {
    .ep_addr = DAP_BULK_IN_EP,
    .ep_cb = DAP_Bulk_IN_CallBack,
};

static struct usbd_interface DAP_HID_Interface;
static struct usbd_endpoint DAP_HID_OUT_Endpoint = {
    .ep_addr = DAP_HID_OUT_EP,
    .ep_cb = DAP_HID_OUT_CallBack,
};
static struct usbd_endpoint DAP_HID_IN_Endpoint = {
    .ep_addr = DAP_HID_IN_EP,
    .ep_cb = DAP_HID_IN_CallBack,
};

static struct usbd_interface CDC_Interface1;
static struct usbd_interface CDC_Interface2;
static struct usbd_endpoint CDC_OUT_Endpoint = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb = CDC_OUT_CallBack,
};
static struct usbd_endpoint CDC_IN_Endpoint = {
    .ep_addr = CDC_IN_EP,
    .ep_cb = CDC_IN_CallBack,
};

uint8_t line_coding_now[7];
void usbd_cdc_acm_set_line_coding(uint8_t busid, uint8_t intf, struct cdc_line_coding *line_coding)
{
    memcpy(line_coding_now, (uint8_t *)line_coding, 7);
    USART4_Config_Linecoding(line_coding_now);
}

void usbd_cdc_acm_get_line_coding(uint8_t busid, uint8_t intf, struct cdc_line_coding *line_coding)
{
    memcpy(line_coding, line_coding_now, 7);
}

void usbd_cdc_acm_set_dtr(uint8_t busid, uint8_t intf, bool dtr)
{
}

void usbd_cdc_acm_set_rts(uint8_t busid, uint8_t intf, bool rts)
{
}

void usbd_cdc_acm_send_break(uint8_t busid, uint8_t intf)
{
}

#define ESIG_ADDR ((uint32_t)0x1FFFF7E8)
void DAP_USB_Init(void)
{
    uint8_t unique_id[0x1A - 2];
    uint8_t uid_buf[12];
    memcpy(uid_buf, (uint8_t *)ESIG_ADDR, 12);
    for (uint8_t i = 0; i < 12; i++)
    {
        unique_id[2 * i] = uid_buf[i] % 9 + '0';
        unique_id[2 * i + 1] = 0;
    }
    memcpy(dap_descriptor + 198, unique_id, 0x1A - 2);

    usbd_desc_register(0, dap_descriptor);
    usbd_bos_desc_register(0, &bos_desc);
    usbd_msosv2_desc_register(0, &msosv2_desc);

    usbd_add_interface(0, &DAP_Bulk_Interface);
    usbd_add_endpoint(0, &DAP_Bulk_OUT_Endpoint);
    usbd_add_endpoint(0, &DAP_Bulk_IN_Endpoint);

    usbd_add_interface(0, usbd_hid_init_intf(0, &DAP_HID_Interface, dap_hid_descriptor, HID_DESCRIPTOR_SIZE));
    usbd_add_endpoint(0, &DAP_HID_OUT_Endpoint);
    usbd_add_endpoint(0, &DAP_HID_IN_Endpoint);

    usbd_add_interface(0, usbd_cdc_acm_init_intf(0, &CDC_Interface1));
    usbd_add_interface(0, usbd_cdc_acm_init_intf(0, &CDC_Interface2));
    usbd_add_endpoint(0, &CDC_OUT_Endpoint);
    usbd_add_endpoint(0, &CDC_IN_Endpoint);

    usbd_initialize(0, USBFS_BASE, &usbd_event_handler);
}
