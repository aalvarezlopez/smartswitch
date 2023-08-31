/// debug.c
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
/// Authored by:   Adrian (03/12/2013)
/// Revised by:    JRB (16/12/2013)
/// Last Version:  18/12/2013
///
/// FILE CONTENTS:
/// Debug utilities using USB as a serial port

#include "sam4lc4c.h"
#include "configuration.h"
#include "mcu_peripheral.h"
#include "interrupt_sam_nvic.h"
#include "usb_protocol_cdc.h"
#define GLOBALSFORME
#include "debug.h"

/** \section brief
 * Application debuger through USB serial port
 *
 */

//REVIEW: many empty and uncommented functions...

/** \brief Configure serial port USB for write/read
 *
 */

void InitDebug()
{
#ifndef DEBUG
    return;
#endif
    char_received_flag = 0;
    sd_mmc_init();
    // Start USB stack to authorize VBus monitoring
    udc_start();
    cpu_irq_enable();

    while (char_received_flag == 0) {
        //udi_cdc_putc(char_received_flag);
        TogglePinValue(SAMPLINGPIN);

        if (main_b_msc_enable) {
            if (!udi_msc_process_trans()) {
                ResetWdt();
            }
        }
    }
}

void DebugPrintInit()
{
#ifndef DEBUG
    return;
#endif
    DebugWrite(STRING_HEADER);
}


void WaitOpenSerial()
{
#ifndef DEBUG
    return;
#endif

    while (char_received_flag == 0) {
        //udi_cdc_putc(char_received_flag);
        TogglePinValue(SAMPLINGPIN);

        for (int i = 0; i < 1000; i++) {
            ResetWdt();
        }
    }
}

/** \brief
 *  Writes a debug string
 *  \param
 *  string pointer to a string null finish
 */
void DebugWrite(const char* string)
{
#ifdef DEBUG
    UsbPuts(string);
#endif
}

/** \brief
 *  Writes number string (only in debug mode)
 *  \param
 *  integer value
 */
void DebugPrintDec(int value)
{
#ifdef DEBUG
    PrintDec(value);
#endif
}

/** \brief
 *  Writes number string
 *  \param
 *  integer value
 */
void PrintDec(int value)
{
    char text[12];
    SIntToDecString(value, text);
    UsbPuts(text);
}

/** \brief
 *  Writes a string using USB CDC port
 *  \param
 *  string pointer to a string null finish
 */
void UsbPuts(const char* string)
{
    int i = 0;

    while (string[i] != 0) {
        udi_cdc_putc(string[i]);
        i++;

        if (i > MAXSTRINGSIZE) { return; }
    }
}

/** \brief
 *  Writes a debug string indicating a number in hexadecimal
 *  \param
 *  string pointer and number value
 */
void DebugHexWrite(const char* string, int value)
{
#ifdef DEBUG
    char text[12];
    Int16ToHexString(value, text);
    DebugWrite(string);
    DebugWrite(text);
    DebugWrite("\r\n");
#endif
}

/** \brief
 *  Writes a debug string indicating a number in decimal
 *  \param
 *  string pointer and number value
 */
void DebugDecWrite(const char* string, int value)
{
#ifdef DEBUG
    char text[12];
    SIntToDecString(value, text);
    DebugWrite(string);
    DebugWrite(text);
    DebugWrite("\r\n");
#endif
}

/** \brief This function write pretty format for hour
 *
 *      \param seconds
 *             minutes
 *             hour
 */
void DebugHourWrite(int seconds, int minutes, int hour)
{
#ifdef DEBUG
    char int_in_ascci[MAXDIGITLNG];
    TimeWrite(seconds, minutes, hour, int_in_ascci);
    DebugWrite(int_in_ascci);
    DebugWrite("\r\n");
#endif
}

/** \brief This function write pretty format for date
 *
 *      \param seconds
 *             minutes
 *             hour
 */
void DebugDateWrite(int day, int month, int year)
{
#ifdef DEBUG
    char int_in_ascci[MAXDIGITLNG];
    DateWrite(day, month, year, int_in_ascci);
    DebugWrite(int_in_ascci);
    DebugWrite("\r\n");
#endif
}

