/// status_machine.c
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
/// Authored by:   Adrian (16/12/2013)
/// Revised by:    JRB (Feb 01, 2013)
/// Last Version:  16/12/2013
///
/// FILE CONTENTS:
/// Status Machine and TIMER interrupt functions to handle periodic actions in the device

#include "sam4ls4a.h"
#include "configuration.h"
#include "mcu_peripheral.h"
#include "status_machine.h"
#include "params.h"
#include "ctrl_access.h"
#include "ff.h"
#include "rtcc.h"
#include "conf_access.h"
#include "debug.h"
#include "delays.h"
#include "power_management.h"
#include "interrupt_sam_nvic.h"
#include "analog_utils.h"
#include "make_wav.h"
#include "string_utils.h"
#include "sd_mmc.h"
#include "spisw.h"
#include "my_math.h"
#include "conf_usb.h"

void InStandBy();
void InReady();
void InDelayArmed();
void InMeasuring();
void InStopping();
void InIdle();
void InDelayOff();

extern unsigned int start_up;
//#define TEST_MEASUREMENT
#define LOWPOWER
#ifdef TEST_MEASUREMENT
//2h 1h 30m 10m 5m 1m 25s
unsigned int current_test = 0;
const unsigned int kTestNumber = 7;
const unsigned int kSamplesToMeasured[] = {320000, 843600, 4218000, 25308000,
                                           25308000, 101232000, 50616000
                                          };
#endif

struct Measuring {
    int on_measuring;
    int buffer_reset;
    unsigned int samples_to_measure;
    unsigned int samples_measured;
    short int buffer[BUFFERSIZE];
    short int* pointer_to_write;
    short int* pointer_read_buffer;
    int buffer_overrun_flag;
    int number_overruns;
    int nstab;
    int srate;
    FIL file_object;
    FATFS fs;
} measuring;

const Params* my_params;
int status = STAND_BY;
int status_alarm = 1;

/** \section
 /////////////////////////////////////////////////////////////////////////////
 * TIMER
 * timer0 interrupt routines
 /////////////////////////////////////////////////////////////////////////////
 */

//timer0 interrupt callback
void TC00_Handler(void)
{
    int temp_status_reg __attribute__((unused));
    int value_adc;
    unsigned int bytes_buffer;
    _STATIC int count = 0;
    // Clear status bit to acknowledge interrupt
    temp_status_reg = TC0->TC_CHANNEL[0].TC_SR;

    //To fix sample-rate return number of cycles
    count++;

    if (count < measuring.srate) {
        return;
    }

    count = 0;

    TogglePinValue(SAMPLINGPIN);

    if (measuring.on_measuring != true) { return; }

    if (measuring.samples_measured >= measuring.samples_to_measure) { return; }

    //TODO: use sample rate (only take samples if counter%srate==0)
    value_adc = ReadADC();
    *(measuring.pointer_to_write) = value_adc;
    measuring.pointer_to_write++;
    measuring.samples_measured++;

    if (measuring.pointer_to_write == measuring.pointer_read_buffer) {
        measuring.buffer_overrun_flag = true;
        measuring.pointer_to_write--;
        measuring.number_overruns++;
    }

    bytes_buffer = (measuring.pointer_to_write - measuring.buffer);

    if (bytes_buffer >= BUFFERSIZE) {
        measuring.buffer_reset = true;
        measuring.pointer_to_write = measuring.buffer;
    }

    return;
}

/** \section
 /////////////////////////////////////////////////////////////////////////////
 * COMMAND ACTIONS
 * status machine changes motivated by user commands
 /////////////////////////////////////////////////////////////////////////////
 */

//REVIEW: missing function briefs
void ExecuteStop()
{
    if (measuring.on_measuring) {
        measuring.on_measuring = false;
        status = STOPPING;
    } else {
        TimeCountReset();
        status = STAND_BY;
        ConfIntLis3dh(my_params->lwakeup);
    }
}

void ExecuteRun()
{
    status = DELAY_ARMED;

    measuring.srate =
        BASE_SAMPLE_RATE / my_params->srate;

    if ( measuring.srate  == 0) {
        measuring.srate  = 1;
    }

    measuring.buffer_reset = false;
    measuring.number_overruns = 0;
    measuring.buffer_overrun_flag = false;
    measuring.samples_to_measure = (my_params->tmeas * my_params->srate);
#ifdef TEST_MEASUREMENT
    measuring.samples_to_measure = kSamplesToMeasured[current_test % kTestNumber];
    current_test++;
    DebugHexWrite("SAMPLES", measuring.samples_to_measure);
#endif
    measuring.samples_measured = 0;

    TimeCountReset();

}

void InitWavFile()
{
    unsigned int h, m, s;
    unsigned int y, t, d;
    char text[25];
    char aux[15];
    int status_sd;

    ReadClock(&s, &m, &h);
    ReadDate(&d, &t, &y);
    TimestampWrite(y, t, d, h, m, s, aux);
    UsbPuts(aux);
    //REVIEW: year month and date are not being written to file name
    //REVIEW: 0:VL should be const char *
    strcpy(text, "0:VL_");
    strcpy(text + 5, aux);
    //REVIEW: *.wav should be const char *
    strcpy(text + 17, ".wav");
    DebugWrite("NEW FILE NAME:\r\n");
    DebugWrite(text);
    DebugWrite("\r\n");

    FRESULT res;
    memset(&measuring.fs, 0, sizeof(FATFS));
    res = f_mount(LUN_ID_SD_MMC_0_MEM, &measuring.fs);

    if (FR_INVALID_DRIVE == res) {
        DebugWrite("INVALID DRIVE!\r\n");

        while (1) {
            SetPinValue(LEDRED, LOW);
            SetPinValue(LEDGREEN, LOW);

            for (int i = 0; i < 5; i++) {
                TogglePinValue(LEDRED);
                delay_ms(500);
            }

            SetPinValue(LEDRED, LOW);
            SetPinValue(LEDGREEN, HIGH);
            delay_ms(2000);
        }
    }

    Configure_fattime(y, t, d, h, m, s);
    res = f_open(&measuring.file_object, (char const*) text,
                 FA_CREATE_ALWAYS | FA_WRITE);

    if (res != FR_OK) {
        DebugWrite("FILE COULD NOT BE CREATED\r\n");

        while (1) {
            SetPinValue(LEDRED, LOW);
            SetPinValue(LEDGREEN, LOW);

            for (int i = 0; i < 10; i++) {
                TogglePinValue(LEDRED);
                delay_ms(1000);
            }

            SetPinValue(LEDRED, LOW);
            SetPinValue(LEDGREEN, HIGH);
            delay_ms(2000);
        }
    }

    WriteWavHeader(&measuring.file_object, measuring.samples_to_measure,
                   my_params->srate);
    DebugWrite("WAV header written\r\n");
}

/** \section
 /////////////////////////////////////////////////////////////////////////////
 * STATUS MACHINE
 * status machine initialize and main loop
 /////////////////////////////////////////////////////////////////////////////
 */

/** \brief
 *  This function is called to initialitate
 *  some status machine variables
 */
void InitStatusMachine(Params* p_params)
{
    my_params = p_params;
    measuring.on_measuring = false;
    measuring.buffer_reset = false;
    measuring.buffer_overrun_flag = false;
    measuring.number_overruns = 0;

    status = STAND_BY;

    measuring.samples_to_measure = my_params->tmeas;
    measuring.samples_measured = 0;
    measuring.pointer_to_write = measuring.buffer;
    measuring.pointer_read_buffer = measuring.buffer;

    TimerEnableInterrupt(TIMER0, TMRINTENABLE);
}

/** \brief Main function for status machine
 *
 * \param pointer to param structure that include
 *          some configuration parameters
 *  \return current status
 */
int StatusMachine()
{
    Ctrl_status status_sd;

    //restart buffer and overrunflag
    //common task for every status
    //ToDo: here should be the code to be
    //execute for all methods
    switch (status) {
        case STAND_BY:
            InStandBy();
            break;

        case READY:
            InReady();
            break;

        case DELAY_ARMED:
            InDelayArmed();
            break;

        case MEASURING:
            InMeasuring();
            break;

        case STOPPING:
            InStopping();
            break;

        case DELAY_OFF:
            InDelayOff();
            break;

        case IDLE:
        default:
            InIdle();
            break;
    }

    //check common triggers for every status
    //ToDo  battery limit level, usb connected, ...
    if ((GPIO->GPIO_PORT[VBUSPORT].GPIO_PVR & (1 << VBUSPIN)) && status != IDLE) {
        //USB connected: change to idle status
        // if a measuring is in process stop it, and
        //wait until all data are stored
        if ( status == MEASURING) {
            status = STOPPING;
        } else if ( status != STOPPING) {
            if (!start_up) {
                Reboot();
            }

            start_up = 0;
            ResetWdt();
            TurnOnAnalogDevices(ON);
            delay_ms(100);
            TurnOnSD(ON);
            delay_ms(100);

            for (int i = 0; i < 10; i++) {
                delay_ms(100);
                TogglePinValue(LEDGREEN);
                ResetWdt();
            }

            sd_mmc_init();
            sd_mmc_wait4SD();
            udc_start();
            cpu_irq_enable();

            status = IDLE;
        }
    }

    return status;
}

/** \section
 /////////////////////////////////////////////////////////////////////////////
 * IN-STATE functions
 * Handlers for the different status options
 /////////////////////////////////////////////////////////////////////////////
 */


//REVIEW: Lots of debug messages. Most of them should be eliminated after
//implementation, leaving only the most important

#define COUNTS_FOR_MEASURE_BAT  1

/** \brief
 *  Actions to do in Stand-by status mode
 */
void InStandBy()
{
    unsigned int hours, minutes, seconds, value;
    _STATIC int count_battery = 0;

    if (CheckAlarm()) {
        status_alarm = 1;
    }

    if (!status_alarm) {
        return;
    }

    if (IsBatteryCritical()) {
        Shutdown();
    }

    status_alarm = 0;

    ReadClock(&seconds, &minutes, &hours);
    DebugWrite("\r\n");
    DebugHourWrite(seconds, minutes, hours);
    SetAlarm((seconds + STANDBYTICKS) % 60, 0, 0, SECONDSMATCH | ALARM_POL_HIGH);

    if (count_battery > COUNTS_FOR_MEASURE_BAT) {
        TurnOnAnalogDevices(ON);
        delay_ms(500);
        TriggerADC();

        while (!IsADCFinished()) { continue; }

        UpdateBatteryLevel(GetmvADC());
        TurnOnAnalogDevices(OFF);

        count_battery = 0;
    }

    count_battery++;

    BlinkStatusLed();
    DebugPrintDec(status);
    DebugWrite(": \r\n");

    if ((GetInt1StValue() & (1 << YH_INT_BIT)) != 0) {
        status = READY;

        ConfIntLis3dh(my_params->lstab);

        DebugWrite("Wake-up->ARMED!\r\n");
        measuring.srate =
            BASE_SAMPLE_RATE / my_params->srate;

        if ( measuring.srate  == 0) {
            measuring.srate  = 1;
        }

        measuring.nstab = 0;
        TimeCountReset();
    } else if (!IsUsbConnected()) {
        //this function enter in low power and wake-up for interrupt in EIC
#ifdef  LOWPOWER
        ConfigureBackUpMode();
        SetMcuLowPower();
        SetMcuFullPower();
#endif
    }
}

#define MAXITERATIONS 50
/** \brief Actions to do in Armed status mode
 *
 *  \return
 */
void InReady()
{
    int elapsed = ReadTimeCountElapsed();
    _STATIC int last_time = 0;
    _STATIC unsigned int window_max_value = 0;

    if (IsBatteryCritical()) {
        Shutdown();
    }

    int16_t vib_level = GetLisValue(Y_AXIS);//ReadNADC(20);

    if (vib_level < 0) { vib_level = -vib_level; }

    if (vib_level > window_max_value) { window_max_value = vib_level; }


    if (elapsed == last_time) { return; }

    last_time = elapsed;


    if (elapsed % ARMED_NSECS == 0) {
        DebugPrintDec(status);
        DebugWrite(": ");
        DebugPrintDec(elapsed);
        DebugWrite("/");
        DebugPrintDec(my_params->tstab);
        BlinkStatusLed();
        delay_ms(200);
        BlinkStatusLed();
        delay_ms(500);
    }

    if (elapsed >= my_params->tstab) {
        if (window_max_value < mGtoRawConvert(my_params->lstab)) {
            status = STAND_BY;
            ConfIntLis3dh(my_params->lwakeup);

            for (int i = 0; i < 10; i++) {
                delay_ms(100);
                TogglePinValue(LEDRED);
                ResetWdt();
            }

            DebugWrite("->standby\r\n");
            status_alarm = 1;
            //Read status for remove previous interruption
            GetInt1StValue();
        } else {
            measuring.nstab++;

            if ( measuring.nstab > my_params->nstab) {
                status = DELAY_ARMED;
                ExecuteRun();
                DebugWrite("->delay armed\r\n");
#if BOARD_VERSION!= 1
                SetPinFunction(MOSIPIN, FUNCTION_A);
#endif
            }
        }

        TimeCountReset();
        window_max_value = 0;
    }
}

/** \brief Actions to do in delay after armed status mode
 *
 *  \return
 */
void InDelayArmed()
{
    int elapsed = ReadTimeCountElapsed();
    _STATIC int last_time = 0;

    if (IsBatteryCritical()) {
        Shutdown();
    }

    if (elapsed == last_time) { return; }

    last_time = elapsed;

    if (elapsed % ARMED_NSECS == 0) {
        DebugPrintDec(status);
        DebugWrite(": ");
        DebugPrintDec(elapsed);
        DebugWrite("/");
        DebugPrintDec(my_params->tdelay);
        DebugWrite("\r\n");

        BlinkStatusLed();
        delay_ms(500);
        BlinkStatusLed();
        delay_ms(1000);
    }

    if (elapsed >= (my_params->tdelay - 10)) {
        TurnOnAnalogDevices(ON);
        TurnOnSD(ON);
        ConfigAnalogGain(my_params->gain);
        SpiSWInit();
        SetPGA113();
    }

    if (elapsed >= my_params->tdelay) {

        sd_mmc_init();
        sd_mmc_wait4SD();

        InitWavFile();
#if BOARD_VERSION != 1
        SetPinFunction(MOSIPIN, FUNCTION_A);
#endif
        DebugWrite("...start measuring...\r\n");
        measuring.on_measuring = true;
        measuring.pointer_read_buffer = measuring.buffer;
        measuring.pointer_to_write = measuring.buffer;
        status = MEASURING;
        measuring.samples_measured = 0;

    }
}

/** \brief Actions to do in Measuring status mode
 *      +check for available data save in timer interruption
 *      +save it in sdcard
 *
 *  \return
 */
void InMeasuring()
{
    unsigned int bytes_written = 0 ;
    unsigned int bytes_to_write = (measuring.pointer_to_write
                                   - measuring.pointer_read_buffer);
    ToggleStatusLed();
    GetBatteryLevel();

    if (bytes_to_write > NBYTESTOWRITE) { bytes_to_write = NBYTESTOWRITE; }

    //Every loop check for save files in the internal circular buffer
    //to SD card file.
    if (measuring.buffer_overrun_flag) {
        DebugWrite("Buffer overrun happened");
        DebugHexWrite("ADD", measuring.samples_measured);
        DebugHexWrite("NUM", measuring.number_overruns);
        measuring.number_overruns = 0;
        measuring.buffer_overrun_flag = false;
    }

    if (measuring.buffer_reset) {
        bytes_to_write = BUFFERSIZE
                         - (measuring.pointer_read_buffer - measuring.buffer);
        bytes_written = WriteWavData(&measuring.file_object, bytes_to_write,
                                     measuring.pointer_read_buffer);
        measuring.pointer_read_buffer = measuring.buffer;
        measuring.buffer_reset = 0;
    } else if (bytes_to_write > 0) {
        bytes_written = WriteWavData(&measuring.file_object, bytes_to_write,
                                     measuring.pointer_read_buffer);
        measuring.pointer_read_buffer += bytes_written;
    }

    if (measuring.samples_measured >= measuring.samples_to_measure) {
        //making status change
        DebugWrite("->stopping\r\n");
        measuring.on_measuring = false;
        status = STOPPING;
    }

    if (IsBatteryCritical()) {
        DebugWrite("BAT WARNING ->stopping\r\n");
        measuring.on_measuring = false;
        status = STOPPING;
    }
}

/** \brief Actions to do in stopping status mode
 *
 *  \return
 */
void InStopping()
{
    unsigned int bytes_written = 0 ;
    unsigned int bytes_to_write = (measuring.pointer_to_write
                                   - measuring.pointer_read_buffer);
    SetPinValue(LEDRED, HIGH);

    if (bytes_to_write > NBYTESTOWRITE) { bytes_to_write = NBYTESTOWRITE; }

    //Every loop check for save files in the internal circular buffer
    //to SD card file.
    if (measuring.buffer_overrun_flag) {
        DebugWrite("Buffer overrun happened");
        DebugHexWrite("ADD", measuring.samples_measured);
        DebugHexWrite("NUM", measuring.number_overruns);
        measuring.number_overruns = 0;
        measuring.buffer_overrun_flag = false;
    }

    if (measuring.buffer_reset) {
        DebugWrite("Reset");
        bytes_to_write = BUFFERSIZE
                         - (measuring.pointer_read_buffer - measuring.buffer);
        bytes_written = WriteWavData(&measuring.file_object, bytes_to_write,
                                     measuring.pointer_read_buffer);
        measuring.pointer_read_buffer = measuring.buffer;
        measuring.buffer_reset = 0;
    } else if (bytes_to_write > 0) {
        bytes_written = WriteWavData(&measuring.file_object, bytes_to_write,
                                     measuring.pointer_read_buffer);
        measuring.pointer_read_buffer += bytes_written;
    }

    if (measuring.pointer_read_buffer == measuring.pointer_to_write) {
        //making status change
        WriteWavMetadata(&measuring.file_object, GetAnalogGain(),
                         my_params->calibration, my_params->id);
        f_close(&measuring.file_object);

        DebugWrite("->delay off\r\n");

        TimeCountReset();
        status = DELAY_OFF;
        SetStatusLed(OFF);
        TurnOnAnalogDevices(OFF);
        TurnOnSD(OFF);
    }
}

/** \brief Actions to do in idle status mode
 *
 *  \return
 */
void InIdle()
{
    GetBatteryLevel();
    SetStatusLed(ON);

    if (!(GPIO->GPIO_PORT[VBUSPORT].GPIO_PVR & (1 << VBUSPIN))) {
        status = STAND_BY;
        ConfIntLis3dh(my_params->lwakeup);
        status_alarm = 1;
        TurnOnAnalogDevices(OFF);
        TurnOnSD(OFF);
    }
}
//TODO: idle mode is not implemented.
//It should be entered on USB plug detection.
//Not necessary while doing the status machine implementation

/** \brief Actions to do in delay after stop status mode
 *
 *  \return
 */
void InDelayOff()
{
    int elapsed = ReadTimeCountElapsed();
    _STATIC int last_time = 0;

    if (IsBatteryCritical()) {
        Shutdown();
    }

    if (elapsed == last_time) { return; }

    last_time = elapsed;

    if (elapsed % ARMED_NSECS == 0) {
        DebugPrintDec(status);
        DebugWrite(": ");
        DebugPrintDec(elapsed);
        DebugWrite("/");
        DebugPrintDec(my_params->toff);
        DebugWrite("\r\n");
        delay_ms(500);
        delay_ms(500);
    }

    if (elapsed >= my_params->toff) {
        TimeCountReset();
        status = STAND_BY;
        ConfIntLis3dh(my_params->lwakeup);
        status_alarm = 1;
        //Read status for remove previous interruption
        GetInt1StValue();
    }

}
