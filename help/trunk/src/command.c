/// command.c
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
/// Authored by:   AV (12/12/2013)
/// Revised by:     AAL (08/01/2013)
/// Last Version:
///
/// FILE CONTENTS:
/// command.c functions to parse commands

/** REVIEW:AAL
 *  lots of implicit declaration of function. Include headers of functios...
 *  ExecuteRun, ExecuteStop, SetClock ...
 */

#include "command.h"
#include "configuration.h"
#include "mcu_peripheral.h"
#include "string_utils.h"
#include "delays.h"
#include "debug.h"
#include "analog_utils.h"
#include "data_storage.h"
#include "power_management.h"

#include <string.h>

int udi_cdc_putc(int value);

char rcv_buffer[MAX_BUFFER_SIZE];
int rcv_pointer = 0;
int flag = 0;          // 0, DATA_READY, PRINT_ERROR

/** \section
 /////////////////////////////////////////////////////////////////////////////
 * PREDEFINED RESPONSES
 /////////////////////////////////////////////////////////////////////////////
 */

/** \brief
 *  writes the NACK command to serial port
 *  \return
 *  0 indicating error
 */
int ReturnError()
{
    //REVIEW:AAL
    // better if first made a string (strcpy, strcat,...)
    // and then call only one time to
    // UsbPuts
    udi_cdc_putc(RESPC);
    UsbPuts(NACKC);
    udi_cdc_putc(ENDC);
    return 0;
}

/** \brief writes the ACK command to serial port
 *
 *  \param  command: received command
 */
void ReturnResponse(char* command)
{
    //REVIEW:AAL
    // better if first made a string (strcpy, strcat,...)
    // and then call only one time to
    // UsbPuts
    udi_cdc_putc(RESPC);
    UsbPuts(ACKC);
    UsbPuts(command);
    udi_cdc_putc(ENDC);
    UsbPuts("\r\n");
}

void ReturnResponseVal(const char* command, int value)
{
    char text[10];
    //REVIEW:AAL
    // better if first made a string
    // and then call only one time to
    // UsbPuts
    udi_cdc_putc(RESPC);
    UsbPuts(ACKC);
    UsbPuts(command);
    UsbPuts("=");
    UIntToDecString(value, text);
    UsbPuts(text);
    udi_cdc_putc(ENDC);
    UsbPuts("\r\n");
}

/** \section
 /////////////////////////////////////////////////////////////////////////////
 * Incomming text buffering
 *
 /////////////////////////////////////////////////////////////////////////////
 */

/** \brief Stores the incoming data into a buffer. When buffer is full or end
 *      character founded, active the flag
 *
 *  \param  text: string received
 *          size: size of the string received
 *          my_params: pointer to struct to store params
 *
 *  \return 1: command right
 *          0: command wrong
 */
void IncomingRda(char* text, int size)
{
    _STATIC int valid_init_char = 0;

    for ( int i = 0; i < size ; i++) {
        if (valid_init_char) {
            if (rcv_pointer == MAX_BUFFER_SIZE) {
                rcv_pointer = 0;
                flag = PRINT_ERROR;
                return;
            }

            rcv_buffer[rcv_pointer] = text[i];
            rcv_pointer++;

            if (rcv_buffer[rcv_pointer - 1] == ENDC) {
                valid_init_char = 0;
                flag = DATA_READY;
                return;
            }
        } else {
            if ( text[i] == INITC) {
                rcv_buffer[0] = INITC;
                valid_init_char = 1;
                rcv_pointer = 1;
            }
        }

    }

    flag = 0;
}

/** \section
 /////////////////////////////////////////////////////////////////////////////
 * Processing of the command
 /////////////////////////////////////////////////////////////////////////////
 */

//REVIEW: WHY 3 FUNCTIONS?!
//And, they are not working fine.
//Characters not enclosed within INITC and ENDC should be just ignored,
//but they raise a NACK response.
int ProcessIncomingRda(Params* my_params)
{

    if (flag == DATA_READY) {
        flag = 0;
        return ParseCommand(my_params);
    }

    if (flag == PRINT_ERROR) {
        flag = 0;
        return ReturnError();
    }

    return 0;
}

int ParseCommand(Params* my_params)
{
    int pointer = 1;
    char command[MAX_BUFFER_SIZE];

    //Test init char
    if (rcv_buffer[0] != INITC) { return ReturnError(); }

    //Extract command
    while (rcv_buffer[pointer] != ENDC) {
        command[pointer - 1] = rcv_buffer[pointer];
        pointer++;
    }

    //Test end char
    if (rcv_buffer[pointer] != ENDC) { return ReturnError(); }

    command[pointer - 1] = END_STRING_CHAR;
    return GetCommand(command, my_params);
}

unsigned int AdcCalibration();

//REVIEW: function is too long.
//Some refactoring should be done.

/** REVIEW:AAL
 * All command should return reply command so
 * the function ReturnResponse should be call
 * at the end outside if-else block
 *
 * ReturnResponse arguments could be:
 *  command, error, values
 */
int GetCommand(char* command, Params* my_params)
{
    char* ptr;
    char text[16];
    int hours, minutes, seconds;
    int day, month, year;

    if (!strncmp(command, RUN, strlen(RUN))) {
        ExecuteRun();
        ReturnResponse(RUN);
    } else if (!strncmp(command, STOP, strlen(STOP))) {
        ExecuteStop();
        ReturnResponse(STOP);

    } else if (!strncmp(command, TIME, strlen(TIME))) {
        // REVIEW: AAL
        //this could be done in a function
        ReturnResponse(TIME);
        ptr = strchr(command, '=');

        if (ptr != NULL) {
            hours = command[5] - BLANK_CHAR;
            hours = hours * 10 + (command[6] - BLANK_CHAR);
            minutes = command[7] - BLANK_CHAR;
            minutes = minutes * 10 + (command[8] - BLANK_CHAR);
            seconds = command[9] - BLANK_CHAR;
            seconds = seconds * 10 + (command[10] - BLANK_CHAR);

            SetClock(seconds, minutes, hours);
            TimeWrite(seconds, minutes, hours, text);
            UsbPuts(text);
            UsbPuts("\r\n");
        } else {
            ReadClock(&seconds, &minutes, &hours);
            TimeWrite(seconds, minutes, hours, text);
            UsbPuts(text);
            UsbPuts("\r\n");
        }
    } else if (!strncmp(command, DATE, strlen(DATE))) {
        ReturnResponse(DATE);
        ptr = strchr(command, '=');

        if (ptr != NULL) {
            year = command[5] - BLANK_CHAR;
            year = year * 10 + (command[6] - BLANK_CHAR);
            month = command[7] - BLANK_CHAR;
            month = month * 10 + (command[8] - BLANK_CHAR);
            day = command[9] - BLANK_CHAR;
            day = day * 10 + (command[10] - BLANK_CHAR);

            SetDate(day, month, year);
            DateWrite(day, month, year, text);
            UsbPuts(text);
            UsbPuts("\r\n");
        } else {
            ReadDate(&day, &month, &year);
            DateWrite(day, month, year, text);
            UsbPuts(text);
            UsbPuts("\r\n");
        }
    } else if (!strncmp(command, TSTAB, strlen(TSTAB))) {
        ptr = strchr(command, '=');

        if (ptr != NULL) {
            my_params->tstab = DecStrtoSlong(ptr + 1);
            SaveParams(my_params);
        }

        ReturnResponseVal(TSTAB, my_params->tstab);
    } else if (!strncmp(command, LSTAB, strlen(LSTAB))) {
        ptr = strchr(command, '=');

        if (ptr != NULL) {
            my_params->lstab = DecStrtoSlong(ptr + 1);
            SaveParams(my_params);
        }

        ReturnResponseVal(LSTAB, my_params->lstab);
    } else if (!strncmp(command, NSTAB, strlen(NSTAB))) {
        ptr = strchr(command, '=');

        if (ptr != NULL) {
            my_params->nstab = DecStrtoSlong(ptr + 1);
            SaveParams(my_params);
        }

        ReturnResponseVal(NSTAB, my_params->nstab);
    } else if (!strncmp(command, LWAKEUP, strlen(LWAKEUP))) {
        ptr = strchr(command, '=');

        if (ptr != NULL) {
            my_params->lwakeup = DecStrtoSlong(ptr + 1);
            SaveParams(my_params);
        }

        ReturnResponseVal(LWAKEUP, my_params->lwakeup);
    } else if (!strncmp(command, TMEAS, strlen(TMEAS))) {
        ptr = strchr(command, '=');

        if (ptr != NULL) {
            my_params->tmeas = DecStrtoSlong(ptr + 1);
            SaveParams(my_params);
        }

        ReturnResponseVal(TMEAS, my_params->tmeas);
    } else if (!strncmp(command, TDELAY, strlen(TDELAY))) {
        ptr = strchr(command, '=');

        if (ptr != NULL) {
            my_params->tdelay = DecStrtoSlong(ptr + 1);

            if (my_params->tdelay < 11) {
                my_params->tdelay = 11;
            }

            SaveParams(my_params);
        }

        ReturnResponseVal(TDELAY, my_params->tdelay);
    } else if (!strncmp(command, TOFF, strlen(TOFF))) {
        ptr = strchr(command, '=');

        if (ptr != NULL) {
            my_params->toff = DecStrtoSlong(ptr + 1);
            SaveParams(my_params);
        }

        ReturnResponseVal(TOFF, my_params->toff);
    } else if (!strncmp(command, SRATE, strlen(SRATE))) {
        ptr = strchr(command, '=');

        if (ptr != NULL) {
            unsigned int n_base_sample =
                BASE_SAMPLE_RATE / DecStrtoSlong(ptr + 1);

            if ( n_base_sample == 0) {
                n_base_sample = 1;
            }

            my_params->srate = BASE_SAMPLE_RATE / n_base_sample;
            SaveParams(my_params);
        }

        ReturnResponseVal(SRATE, my_params->srate);
    } else if (!strncmp(command, ID, strlen(ID))) {
        ptr = strchr(command, '=');

        if (ptr != NULL) {
            my_params->id = DecStrtoSlong(ptr + 1);
            SaveParams(my_params);
        }

        ReturnResponseVal(ID, my_params->id);
    } else if (!strncmp(command, GAIN, strlen(GAIN))) {
        ptr = strchr(command, '=');

        if (ptr != NULL) {
            my_params->gain = DecStrtoSlong(ptr + 1);
            ConfigAnalogGain(my_params->gain);
            SaveParams(my_params);
        }

        ReturnResponseVal(GAIN, GetAnalogGain());
    } else if (!strncmp(command, ACEL, strlen(ACEL))) {
        ptr = strchr(command, '=');

        if (ptr != NULL) {
            my_params->acel = DecStrtoSlong(ptr + 1);
            SaveParams(my_params);
        }

        ReturnResponseVal(ACEL, my_params->acel);
    } else if (!strncmp(command, CALIB, strlen(CALIB))) {
        ptr = strchr(command, '=');

        if (ptr != NULL) {
            my_params->calibration = DecStrtoSlong(ptr + 1);
            SaveParams(my_params);
        }

        ReturnResponseVal(CALIB, my_params->calibration);

    } else if (!strncmp(command, CALIBRATE, strlen(CALIBRATE))) {
        int8_t pga = 0;
        ptr = strchr(command, '=');

        if (ptr != NULL) {
            unsigned int g_to_uv;
            unsigned int raw_to_g;
            g_to_uv = DecStrtoUnsigned(ptr + 1);
            raw_to_g = AdcCalibration(g_to_uv);
            my_params->calibration = raw_to_g;
            SaveParams(my_params);
            ReturnResponseVal(CALIBRATE, my_params->calibration);
        } else {
            return ReturnError();
        }
    } else if (!strncmp(command, READADC, strlen(READADC))) {
        ReturnResponseVal(READADC, ReadNADC(20));

    } else if (!strncmp(command, PGA, strlen(PGA))) {
        int8_t pga = 0;
        ptr = strchr(command, '=');

        if (ptr != NULL) {
            pga = HexStrToInt8(ptr + 1);
            SpiSWInit();
            ConfigAnalogGain(pga);
            SetPGA113();
        }

        ReturnResponseVal(PGA, pga);

    } else if (!strncmp(command, GETBAT, strlen(GETBAT))) {
        ReturnResponseVal(GETBAT, ReadBatteryLevel());

    } else if (!strncmp(command, CSHUTDOWN, strlen(CSHUTDOWN))) {
        Shutdown();
    } else if (!strncmp(command, UPDATE, strlen(UPDATE))) {
        UpdateFirmware();
        ReturnResponse("rebooting");
        SetPinValue(LEDGREEN, LOW);
        SetPinValue(LEDRED, LOW);

        for (int i = 0; i < 3; i++) {
            TogglePinValue(LEDRED);
            delay_ms(2000);
        }

        Reboot();
    } else if (!strncmp(command, REBOOT, strlen(UPDATE))) {
        Reboot();
    } else {
        return ReturnError();
    }

    return 1;          //Command OK

}
