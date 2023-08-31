/// main.c
///
/// Copyright (C) 2013 INGEN10 Ingenieria SL
/// http://www.ingen10.com
///
/// LEGAL NOTICE:
/// All information contained herein is, and remains property of INGEN10 Ingenieria SL.
/// Dissemination of this information or reproduction of this material is strictly
/// forbidden unless prior written permission is obtained from its owner.
/// ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC PERFORMANCE, OR PUBLIC DISPLAY
/// OF, OR THROUGH USE OF THIS SOURCE CODE IS STRICTLY PROHIBITED, AND IT IS A VIOLATION
/// OF INTERNATIONAL TRADE TREATIES AND LAWS.
/// THE RECEIPT OR POSSESSION OF THIS DOCUMENT DOES NOT CONVEY OR IMPLY ANY RIGHTS.
///
/// Authored by:   Adrian (Dec 16, 2013)
/// Revised by:    JRB (Feb 1, 2014)
/// Last Version:
///
/// FILE CONTENTS:
/// main loop and board initialitation for the VIBLOG project

/** \REVIEW: AAL
 * implicit delcaration of udi_cdc_get_nb_received_data and udi_cdc_getc
 * include file header or function header
 */

//#define RUN_TEST

#include "sam4ls4a.h"
#include "configuration.h"
#include "mcu_peripheral.h"
#include "debug.h"
#include "delays.h"
#include "rtcc.h"
#include "interrupt_sam_nvic.h"
#include "usb_protocol_cdc.h"
#include "command.h"
#include "params.h"
#include "analog_utils.h"
#include "status_machine.h"
#include "ctrl_access.h"
#include "ff.h"
#include "power_management.h"
#include "data_storage.h"
#include "spisw.h"
#include "adc.h"
#include "test.h"
#include "conf_usb.h"
#include "usb_protocol.h"
#include "udd.h"

//this global variable is used to wait until port will be open.
//It's a temporal variable and used only for debug uses.
extern int char_received_flag;

int sd_fail = 0;
unsigned int start_up;

void InitBoard();


/** \section INTERRUPTS
 /////////////////////////////////////////////////////////////////////////////
 * EXT interrupts from LIS3DH and CLOCK
 /////////////////////////////////////////////////////////////////////////////
 */

int vib1_flag = 0;
int vib2_flag = 0;

/** \brief
 *  External interrupt handler for INT3.
 *  This interrupt corresponds to clock alarm
 */
void EIC_3_Handler(void)
{
    if (EIC->EIC_ISR & (1 << CLK_EIC)) {
        EIC->EIC_ICR = 1 << CLK_EIC;
        EIC->EIC_ISR;
        TogglePinValue(LEDGREEN);
    }
}

/** \brief
 *  External interrupt handler for INT3.
 *  This interrupt corresponds to
 *  lis3dh vibration alarm 2
 */
void EIC_1_Handler(void)
{
    if (EIC->EIC_ISR & (1 << VIB2_EIC)) {
        EIC->EIC_ICR = 1 << VIB2_EIC;
        EIC->EIC_ISR;
        vib2_flag = 1;
    }
}

/** \brief
 *  External interrupt handler for INT3.
 *  This interrupt corresponds to
 *  lis3dh vibration alarm 1
 */
void EIC_4_Handler(void)
{
    if (EIC->EIC_ISR & (1 << VIB1_EIC)) {
        EIC->EIC_ICR = 1 << VIB1_EIC;
        EIC->EIC_ISR;
        vib1_flag = 1;
        TogglePinValue(LEDGREEN);
        GetInt1StValue();
    }
}


/** \section MAIN
 /////////////////////////////////////////////////////////////////////////////
 * Main initialisation and Loop
 /////////////////////////////////////////////////////////////////////////////
 */
Params myParams;

int main(void)
{
    unsigned int el_hour = 0, el_min = 0, el_sec = 0;
    unsigned int el_year = 10, el_month = 1, el_day = 1;

    Ctrl_status status __attribute__((unused));

    start_up = 1;

    InitBoard();
    SetPinValue(LEDGREEN, HIGH);
    SetPinValue(LEDRED, HIGH);
    InitDebug();
    SetPinValue(LEDGREEN, LOW);
    SetPinValue(LEDRED, LOW);
    DebugPrintInit();
    EnableI2C(6);
    delay_ms(500);

    DebugWrite("Loading parameters\r\n");
    LoadParams(&myParams);
    DebugWrite("Loading status machine\r\n");
    InitStatusMachine(&myParams);

#ifdef RUN_TEST
    runTest();
#endif
    ConfIntLis3dh(myParams.lwakeup);
    ConfigAnalogChannel(CH1);
    ConfigAnalogGain(myParams.gain);
    DebugDecWrite("PGA:", myParams.gain);
    SetPGA113();

    ReadClock(&el_sec, &el_min, &el_hour);
    ReadDate(&el_day, &el_month, &el_year);
    DebugHourWrite(el_sec, el_min, el_hour);

    while (InitLis3dh(myParams) != 1) {
        DebugWrite("Re-trying LIS3DH initation");
        delay_ms(5000);
    }

    DebugWrite("LIS3DH OK\r\n");

    if (!IsRTCCEnabled()) {
        SetClock(0, 0, 0);
        SetDate(1, 1, 2014);
    }

    TimeCountReset();
    TriggerADC();
    //Read status for remove previous interruption
    GetInt1StValue();

    while (1) {
        ProcessIncomingRda(&myParams);
        status = StatusMachine();

        if (main_b_msc_enable) {
            if (!udi_msc_process_trans()) {

            }
        }

        if (sd_fail != 0) {
            TurnOnSD(OFF);
            ConfigureWdt(16);
            SetPinDirection(MISOPIN, INPUTPIN, 0);
#if BOARD_VERSION == 1
            SetPinDirection(MOSIPIN, INPUTPIN, 0);
#endif
            SetPinDirection(MSCKPIN, INPUTPIN, 0);
            SetPinDirection(SOFTCS1, INPUTPIN, 0);

            while (1) {
                switch (sd_fail) {
                    case 1:
                        SetPinValue(LEDGREEN, LOW);
                        SetPinValue(LEDRED, HIGH);

                        for (int i = 0; i < 4; i++) {
                            delay_ms(100);
                            TogglePinValue(LEDRED);
                        }

                        break;

                    case 2:
                        SetPinValue(LEDGREEN, LOW);
                        SetPinValue(LEDRED, HIGH);

                        for (int i = 0; i < 8; i++) {
                            delay_ms(50);
                            TogglePinValue(LEDRED);
                        }

                        break;

                    case 3:
                        SetPinValue(LEDGREEN, LOW);
                        SetPinValue(LEDRED, HIGH);

                        for (int i = 0; i < 4; i++) {
                            delay_ms(1000);
                            TogglePinValue(LEDRED);
                        }

                        break;
                }

                SetPinValue(LEDGREEN, HIGH);
                SetPinValue(LEDRED, LOW);
                delay_ms(500);
            }
        } else {
            ResetWdt();
        }
    }
}

/** \section BOARD-INIT
 /////////////////////////////////////////////////////////////////////////////
 * Pins and peripherals initialisation
 /////////////////////////////////////////////////////////////////////////////
 */

/** \brief
 *  This function configure all pins use in the App,
 *  besides clocks and other peripheral setttings
 */
void InitBoard()
{
    ConfigureOsc();
    ConfigureWdt(18);
    SetPinFunction(BAT_MEAS, FUNCTION_A);
    EnableADC(ADC_CLOCK_DIVISOR);

    SetPinFunction(MISOPIN, FUNCTION_A);
#if BOARD_VERSION == 1
    SetPinFunction(MOSIPIN, FUNCTION_A);
#endif
    SetPinFunction(MSCKPIN, FUNCTION_A);
    SetPinFunction(SOFTCS1, FUNCTION_A);

    PM->PM_UNLOCK = (0xAA << 24) | 0x24;
    PM->PM_HSBMASK |=  1 << 5;
    PM->PM_UNLOCK = (0xAA << 24) | 0x28;
    PM->PM_PBAMASK |=  1 << 9;
    //sysclk_enable_pba_divmask(PBA_DIVMASK_CLK_USART);
    PM->PM_UNLOCK = (0xAA << 24) | 0x40;
    PM->PM_PBADIVMASK |=  1 << 2;
    SCIF->SCIF_GCCTRL[7].SCIF_GCCTRL = 0x001001;        //source PLL0 and enable

    //Set pin of vbus detection as output
    SetPinDirection(VBUSDET, INPUTPIN, PULLDISABLE);
    //Setting DM DP as peripheral function A
    SetPinFunction(USBDMPIN, FUNCTION_A);
    SetPinFunction(USBDPPIN, FUNCTION_A);

    //set circuit power enabling pins
    SetPinDirection(CRCT_ON_PIN, OUTPUTPIN, 0);
    SetCircuit(ON);
    SetPinDirection(AVDD_ON_PIN, OUTPUTPIN, 0);
    TurnOnAnalogDevices(OFF);

    SetPinDirection(SDPWR, OUTPUTPIN, 0);
    TurnOnSD(OFF);

    //Set Leds as a output
    SetPinDirection(LEDRED, OUTPUTPIN, 0);
    SetPinValue(LEDRED, LOW);
    SetPinDirection(LEDGREEN, OUTPUTPIN, 0);
    SetPinValue(LEDGREEN, LOW);

    //Set pin of sampling timer as output
    SetPinDirection(SAMPLINGPIN, OUTPUTPIN, 0);

    //configure and active sampling timer
    TimerCaptureConfigure(TIMER0, TIMER_CLOCK2, COUNTER4TMRCLK2,
                          SAMPLEPRIORITY);

    SetPinDirection(SOFTCS0, OUTPUTPIN, 0);
    SetPinValue(SOFTCS0, HIGH);
    SetPinDirection(SOFTCS2, OUTPUTPIN, 0);
    SetPinValue(SOFTCS2, HIGH);
    SetPinDirection(SOFTCS3, OUTPUTPIN, 0);
    SetPinValue(SOFTCS3, HIGH);
    delay_ms(100);

    SpiSWConfigure(0, 0);
    SpiSWInit();

    delay_ms(100);
}

/** \REVIEW: AAL
 * TODO->MOVE ALL THIS STUFF TO A DIFFERENT FILE!!
 */

void main_suspend_action(void)
{
}

void main_resume_action(void)
{
}

void main_sof_action(void)
{
}

bool main_cdc_enable(uint8_t port)
{
    main_b_cdc_enable = true;
    return true;
}

void main_cdc_disable(uint8_t port)
{
}

void main_cdc_set_dtr(uint8_t port, bool b_enable)
{
}

void uart_config(uint8_t port, usb_cdc_line_coding_t* cfg)
{
}

void uart_rx_notify(uint8_t port)
{
    char c;
    int pointer = 0, numBytes;
    char text[10];
    // REVIEW:AAL unused variable result
    int result;
    UNUSED(port);

    char_received_flag = 1;
    numBytes = udi_cdc_get_nb_received_data();

    while (numBytes != pointer) {
        c = udi_cdc_getc();
        //udi_cdc_putc(c);
        text[pointer] = c;
        pointer++;
    }

    IncomingRda(text, numBytes);
}

void ui_start_read(void)
{
}

void ui_stop_read(void)
{
}

void ui_start_write(void)
{
}

void ui_stop_write(void)
{
}


bool main_msc_enable(void)
{
    main_b_msc_enable = true;
    return true;
}

void main_msc_disable(void)
{
    main_b_msc_enable = false;
}


/*! \brief Example of extra USB string management
 * This feature is available for single or composite device
 * which want implement additional USB string than
 * Manufacture, Product and serial number ID.
 *
 * return true, if the string ID requested is know and managed by this functions
 */
bool main_extra_string(void)
{
    _STATIC uint8_t udi_cdc_name[] = "CDC interface";
    _STATIC uint8_t udi_msc_name[] = "MSC interface";

    struct extra_strings_desc_t {
        usb_str_desc_t header;
        le16_t string[Max(sizeof(udi_cdc_name) - 1, sizeof(udi_msc_name) - 1)];
    };
    _STATIC UDC_DESC_STORAGE struct extra_strings_desc_t extra_strings_desc = {
        .header.bDescriptorType = USB_DT_STRING
    };

    uint8_t i;
    uint8_t* str;
    uint8_t str_lgt = 0;

    // Link payload pointer to the string corresponding at request
    switch (udd_g_ctrlreq.req.wValue & 0xff) {
        case UDI_CDC_IAD_STRING_ID:
            str_lgt = sizeof(udi_cdc_name) - 1;
            str = udi_cdc_name;
            break;

        case UDI_MSC_STRING_ID:
            str_lgt = sizeof(udi_msc_name) - 1;
            str = udi_msc_name;
            break;

        default:
            return false;
    }

    if (str_lgt != 0) {
        for ( i = 0; i < str_lgt; i++) {
            extra_strings_desc.string[i] = cpu_to_le16((le16_t)str[i]);
        }

        extra_strings_desc.header.bLength = 2 + (str_lgt) * 2;
        udd_g_ctrlreq.payload_size = extra_strings_desc.header.bLength;
        udd_g_ctrlreq.payload = (uint8_t*) &extra_strings_desc;
    }

    // if the string is larger than request length, then cut it
    if (udd_g_ctrlreq.payload_size > udd_g_ctrlreq.req.wLength) {
        udd_g_ctrlreq.payload_size = udd_g_ctrlreq.req.wLength;
    }

    return true;
}

