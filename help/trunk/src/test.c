/// test.c
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
/// Authored by:    Adrian (22/01/2014)
/// Revised by:     JRB (Feb 01, 2014)
/// Last Version:   22/01/2014
///
/// FILE CONTENTS:
/// Various test for VibLog peripherals

#include "sam4ls4a.h"
#include "configuration.h"
#include "mcu_peripheral.h"
#include "rtcc.h"
#include "delays.h"
#include "debug.h"
#include "power_management.h"
#include "analog_utils.h"
#include "sd_mmc.h"
#include "ctrl_access.h"
#include "ff.h"
#include "conf_access.h"
#include "interrupt_sam_nvic.h"

//#define RTCC_TEST
//#define PGA_TEST
#define LIS3_TEST
//#define SD_TEST
//#define LOWPOWER_TEST
//#define TIMEWAKEUP_TEST
//#define MMC_TEST
//#define TEST_SQRT
//#define TEST_CONSUM
//#define TEST_SQRT
//#define TEST_WDT
//#define TEST_BOOT
//#define TEST_CALIBRATION

/** \brief Low power test
 *
 *  this test will check low power feature. After setting clock
 *  and alarm mcu will go to back-up mode. It wake up when RTCC rises
 *  PA5 pin.
 */

void testLowPower()
{
    SetClock(0, 10, 5);
    SetAlarm(15, 0, 0, SECONDSMATCH | ALARM_POL_HIGH);
    DebugWrite("Entering back-up mode");
    ConfigureBackUpMode();
    SetMcuLowPower();
    // --------- LOW POWER MODE ------------ //
    SetMcuFullPower();
    DebugWrite("Leaving back-up mode");
    delay_ms(100);
    char text[16];
    unsigned int hours, minutes, seconds, value;

    while (1) {
        TogglePinValue(LEDRED);
        ReadClock(&seconds, &minutes, &hours);
        TimeWrite(seconds, minutes, hours, text);
        ReadRegister(CONTROL, &value);
        delay_ms(5000);
    }
}

void testTimeWakeup()
{
#define TESTSTANDBYTICKS    10
    _STATIC int counter = TESTSTANDBYTICKS;

    SetClock(0, 10, 5);
    SetAlarm(counter, 0, 0, SECONDSMATCH | ALARM_POL_HIGH);

    ExternalInterruptConfigure(CLK_ALARM, CLK_EIC, FUNCTION_C, 1);
    EICEnable(CLK_EIC, 1);
    //ToDo change to the correct pin
    BPM->BPM_BKUPPMUX = 1 << CLK_EIC;
    BPM->BPM_BKUPWEN = 1 << BPM_BKUPWEN_EIC;
    BPM->BPM_IORET = 0;
    BPM->BPM_IORET = 1;

    //TODO: interrupt doesn't work. Maybe pin setup? flags??

    char text[16];
    unsigned int hours, minutes, seconds, value;

    while (1) {
        TogglePinValue(LEDRED);
        SetPinValue(LEDGREEN, IsUsbConnected());
        ReadClock(&seconds, &minutes, &hours);
        DebugHourWrite(seconds, minutes, hours);
        ReadRegister(CONTROL, &value);

        if (CheckAlarm()) {
            DebugWrite("Alarm!");
            counter += TESTSTANDBYTICKS;
            counter %= 60;
            SetAlarm(counter, 0, 0, SECONDSMATCH | ALARM_POL_HIGH);
        }

        delay_ms(2000);
    }

}

void testClock()
{
    unsigned int el_hour = 0, el_min = 0, el_sec = 0;
    unsigned int el_year = 10, el_month = 1, el_day = 1;
    delay_ms(200);
    DebugWrite("Analog on\r\n");
    TurnOnAnalogDevices(ON);
    delay_ms(100);

    SetClock(0, 10, 5);
    delay_ms(500);
    ReadClock(&el_sec, &el_min, &el_hour);
    ReadDate(&el_day, &el_month, &el_year);
    DebugHourWrite(el_sec, el_min, el_hour);

    while (1) {
        delay_ms(1000);
        ResetWdt();
        ReadClock(&el_sec, &el_min, &el_hour);
        ReadDate(&el_day, &el_month, &el_year);
        DebugHourWrite(el_sec, el_min, el_hour);
    }
}

void testPGA()
{
    int16_t value;
    delay_ms(200);
    DebugWrite("Analog on\r\n");
    TurnOnAnalogDevices(ON);
    delay_ms(100);
    ResetWdt();

    while (1) {
        ConfigAnalogChannel(CAL4);
        DebugWrite("PGA: VREF *");
        SetPGA113();
        delay_ms(2000);
        ResetWdt();
        value = ReadADC();
        DebugDecWrite("ADC (VREF): ", value);
        ConfigAnalogChannel(CAL1);
        DebugWrite("PGA: GND *");
        SetPGA113();
        delay_ms(2000);
        ResetWdt();
        value = ReadADC();
        DebugDecWrite("ADC (GND): ", value);

        DebugWrite("PGA: CH0 x1");
        ConfigAnalogChannel(CH0);
        ConfigAnalogGain(0);
        SetPGA113();
        delay_ms(2000);

        DebugWrite("PGA: CH1 x1");
        ConfigAnalogChannel(CH1);
        ConfigAnalogGain(0);
        SetPGA113();
        delay_ms(2000);
        ResetWdt();

    }
}

void testLIS3()
{
    int16_t value;
    delay_ms(200);
    DebugWrite("Analog on\r\n");
    TurnOnAnalogDevices(ON);
    delay_ms(100);
    ResetWdt();

    while (InitLis3dh() != 1) {
        DebugWrite("Re-trying LIS3DH initation");
        delay_ms(1000);
    }

    ResetWdt();
    ConfIntLis3dh(900);

    while (1) {
        delay_ms(1000);
        value = GetLisValue(X_AXIS);
        DebugDecWrite("X : ", value);
        ResetWdt();
        delay_ms(1000);
        ResetWdt();
        value = GetLisValue(Y_AXIS);
        DebugDecWrite("Y : ", value);
        delay_ms(1000);
        ResetWdt();
        value = GetLisValue(Z_AXIS);
        DebugDecWrite("Z : ", value);
        ResetWdt();

        if ((GetInt1StValue() & (1 << YH_INT_BIT)) != 0) {
            DebugWrite("Alarm\r\n");
        }
    }
}

void testSD()
{
    FIL file_object;
    FATFS fs;
    int status;
    FRESULT res;
    char text[25];
    TurnOnSD(ON);
    delay_ms(2000);
    strcpy(text, "test.wav");
    DebugWrite("uSD init...  ");
    sd_mmc_init();

    // Wait for card present and ready
    do {
        status = sd_mmc_test_unit_ready(0);

        if (CTRL_FAIL == status) {
            while (CTRL_NO_PRESENT != sd_mmc_check(0)) {
            }
        }
    } while (CTRL_GOOD != status);

    memset(&fs, 0, sizeof(FATFS));
    res = f_mount(LUN_ID_SD_MMC_0_MEM, &fs);

    if (FR_INVALID_DRIVE == res) {
        DebugWrite("INVALID DRIVE!\r\n");
        //ToDo: some action should be made in this case
        //while(1);
    }

    res = f_open(&file_object, (char const*) text,
                 FA_CREATE_ALWAYS | FA_WRITE);

    if (res != FR_OK) {
        DebugWrite("FILE COULD NOT BE CREATED\r\n");
        //ToDo: some action should be made in this case
        //while(1);
    }

    f_close(&file_object);
    DebugWrite("uSD finish...  ");

    while (1) {
        TogglePinValue(LEDGREEN);
        TogglePinValue(LEDRED);
        delay_ms(500);
    }
}

void testMMC()
{
    FIL file_object;
    FATFS fs;
    int status;
    FRESULT res;
    char text[25];
    strcpy(text, "test11.wav");
    SetPinValue(LEDRED, HIGH);
    SetPinValue(LEDGREEN, LOW);
    sd_mmc_init();

    // Wait for card present and ready
    do {
        status = sd_mmc_test_unit_ready(0);

        if (CTRL_FAIL == status) {
            while (CTRL_NO_PRESENT != sd_mmc_check(0)) {
            }
        }
    } while (CTRL_GOOD != status);

    memset(&fs, 0, sizeof(FATFS));
    res = f_mount(LUN_ID_SD_MMC_0_MEM, &fs);

    if (FR_INVALID_DRIVE == res) {
        //ToDo: some action should be made in this case
        while (1) {
            TogglePinValue(LEDRED);
            delay_ms(100);
        }
    }

    res = f_open(&file_object, (char const*) text,
                 FA_CREATE_ALWAYS | FA_WRITE);

    if (res != FR_OK) {
        //ToDo: some action should be made in this case
        while (1) {
            TogglePinValue(LEDRED);
            delay_ms(2000);
        }
    }

    f_close(&file_object);
    SetPinValue(LEDRED, LOW);
    SetPinValue(LEDGREEN, HIGH);


    udc_start();
    cpu_irq_enable();

    while (1) {
        if (main_b_msc_enable) {
            if (!udi_msc_process_trans()) {

            }
        }
    }
}

#include "my_math.h"
void testSqrt()
{
    unsigned int values[] = {39672, 50430, 16176, 29996, 48472, 7886, 58208, 38438, 51782,
                             57076, 19116, 21402, 63594, 24462, 60484, 14098, 23998,
                             50896, 40256, 27242, 348, 56212, 45836, 12014, 39750,
                             33198, 27148, 1168, 18188, 41698, 39262, 48206, 21290,
                             55030, 7244, 23668, 25700, 35012, 9976, 27324, 812,
                             21008, 53926, 26488, 51754, 36866, 134, 17722, 40774,
                             55300
                            };
    int correct_result;
    int res;

    for ( int i = 0; i < 50 ; i++) {
        res = sqrt_newtoon(values[i], 50);
        DebugDecWrite("x", values[i]);
        DebugDecWrite("y", res);
        DebugDecWrite("y*y", res * res);
    }
}

void TestConsum()
{
    int i = 0;
    TurnOnAnalogDevices(OFF);
    TurnOnSD(OFF);

    while (1) {
        SetPinValue(LEDGREEN, HIGH);
        SetPinValue(LEDRED, LOW);
        TurnOnAnalogDevices(ON);
        TurnOnSD(OFF);

        for (i = 0; i < 10; i++) {
            delay_ms(1000);
            TogglePinValue(LEDGREEN);
        }

        SetPinValue(LEDGREEN, LOW);
        SetPinValue(LEDRED, HIGH);
        TurnOnSD(ON);

        for (i = 0; i < 10; i++) {
            delay_ms(1000);
            TogglePinValue(LEDRED);
        }

        SetPinValue(LEDGREEN, HIGH);
        SetPinValue(LEDRED, HIGH);
        TurnOnAnalogDevices(OFF);
        delay_ms(100);

        for (i = 0; i < 10; i++) {
            delay_ms(1000);
            TogglePinValue(LEDGREEN);
            TogglePinValue(LEDRED);
        }

        TurnOnAnalogDevices(OFF);
        TurnOnSD(OFF);
        SetPinValue(LEDGREEN, LOW);
        SetPinValue(LEDRED, LOW);

        for (i = 0; i < 10; i++) {
            delay_ms(1000);
            TogglePinValue(LEDGREEN);
        }

        SetClock(0, 10, 5);
        SetAlarm(50, 0, 0, SECONDSMATCH | ALARM_POL_HIGH);
        ConfigureBackUpMode();
        SetMcuLowPower();
        // --------- LOW POWER MODE ------------ //
        SetMcuFullPower();
        TurnOnAnalogDevices(OFF);
        TurnOnSD(OFF);
        SetPinValue(LEDGREEN, HIGH);
        SetPinValue(LEDRED, LOW);

        for (i = 0; i < 10; i++) {
            delay_ms(1000);
            TogglePinValue(LEDGREEN);
            TogglePinValue(LEDRED);
        }
    }
}

void testWDT()
{
    int count = 0;
    ConfigureWdt();
    SetPinValue(LEDGREEN, LOW);
    SetPinValue(LEDRED, LOW);

    while (1) {
        TogglePinValue(LEDGREEN);

        if (count > 100) {
            TogglePinValue(LEDRED);
        } else {
            ResetWdt();
            count++;
        }

        count = 0;
        delay_ms(100);
    }
}

void TestBoot()
{
    UpdateFirmware();
    SetPinValue(LEDGREEN, LOW);
    SetPinValue(LEDRED, LOW);

    for (int i = 0; i < 3; i++) {
        TogglePinValue(LEDRED);
        delay_ms(2000);
    }

    Reboot();
}

void TestCal()
{
    unsigned int raw_to_g;
    int value_adc;
    TurnOnAnalogDevices(ON);
    delay_ms(1000);

    for (int i = 0; i < 10; i++) {
        raw_to_g = AdcCalibration(45100);
        delay_ms(300);
    }

    ConfigAnalogChannel(CAL1);
    SetPGA113();

    while (1) {
        value_adc = ReadADC();
        DebugDecWrite("value_adc", value_adc);
        DebugDecWrite("div", raw_to_g);
        ResetWdt();
        delay_ms(2000);
    }
}

void runTest()
{
#ifdef RTCC_TEST
    //while bucle
    testClock();
#endif
#ifdef PGA_TEST
    testPGA();
#endif
#ifdef LIS3_TEST
    testLIS3();
#endif
#ifdef SD_TEST
    testSD();
#endif
#ifdef LOWPOWER_TEST
    testLowPower();
#endif
#ifdef TIMEWAKEUP_TEST
    testTimeWakeup();
#endif

#ifdef MMC_TEST
    testMMC();
#endif

#ifdef TEST_SQRT
    testSqrt();
#endif
#ifdef TEST_CONSUM
    TestConsum();
#endif
#ifdef TEST_WDT
    testWDT();
#endif
#ifdef TEST_BOOT
    TestBoot();
#endif
#ifdef TEST_CALIBRATION
    TestCal();
#endif
}

