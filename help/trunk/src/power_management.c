/// power_management.c
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
/// Authored by:   JRB (21/12/2013)
/// Revised by:    JRB (Feb 01, 2014)
/// Last Version:
///
/// FILE CONTENTS:
/// Power control & battery level management functions

#include "power_management.h"

#include "sam4lc4c.h"
#include "configuration.h"
#include "mcu_peripheral.h"
#include "debug.h"
#include "adc.h"
#include "interrupt_sam_nvic.h"
#include "delays.h"
#include "analog_utils.h"

#define PINSNUMBER 23
unsigned int kPinsToDisable[] = { LEDRED, LEDGREEN, AVDD_ON_PIN,
                                  MISOPIN, MSCKPIN, SOFTCS0, SOFTCS1, SOFTCS2, SOFTCS3,
                                  SAMPLINGPIN, BAT_MEAS, WAKEUPPIN, SDAPIN, SCLPIN,
                                  CHRG_DET, VBUSDET, USBDMPIN, USBDPPIN, MOSIPIN, SDPWR, BBMISO,
                                  BBMOSI, BBCLK
                                };

/** \section MCU_CONTROL
 ///////////////////////////////////////////////////////////////////
 * Set different states of MCU to minimize power consumption
 ///////////////////////////////////////////////////////////////////
 */

/** \brief Shutwdown system
 *
 */
void Shutdown()
{
    udc_stop();
    delay_ms(200);
    TurnOnAnalogDevices(OFF);
    SetPinValue(SDPWR, LOW);
    SetPinDirection(VBUSDET, OUTPUTPIN, 0);
    SetPinValue(VBUSDET, LOW);
    SetCircuit(OFF);
    SetMcuLowPower();
}

/** \brief Set wdt and wait overload
 *
 */
void Reboot()
{
    ConfigureWdt(16);

    while (1) { continue; }
}

void SetMcuLowPower()
{
    //Making settings and GPIO for backing up
    TurnOnAnalogDevices(OFF);

    for ( int i = 0; i < PINSNUMBER; i++) {
        SetPinDirection(kPinsToDisable[i], INPUTPIN, PULLDISABLE);
    }

    DisableADC();

    cpu_irq_disable();

    unsigned int pmcon __attribute__((unused));
    pmcon = BPM->BPM_PMCON;
    pmcon &= ~BPM_PMCON_BKUP;
    pmcon &= ~BPM_PMCON_RET;
    pmcon &= ~BPM_PMCON_SLEEP_Msk;

    pmcon |= BPM_PMCON_BKUP;
    BPM->BPM_PMCON = pmcon;
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    /* Wait until vreg is ok. */
    while (!(BSCIF->BSCIF_PCLKSR & BSCIF_PCLKSR_VREGOK));

    asm volatile ("wfi");
    /* ensure sleep request propagation to flash. */
    asm volatile ("nop");

    cpu_irq_enable();

}

/** \brief configure system for backup mode
 *
 */
void ConfigureBackUpMode()
{
    //clear LIS3DH previous alarms
    GetInt1StValue();
    //configure interrupts
    ExternalInterruptConfigure(VIB_ALARM1, VIB1_EIC, FUNCTION_C, 1);
    EICEnable(VIB1_EIC, 1);
    //configure interrupts
    ExternalInterruptConfigure(CLK_ALARM, CLK_EIC, FUNCTION_C, 1);
    EICEnable(CLK_EIC, 1);
    BPM->BPM_BKUPPMUX = (1 << VIB1_EIC) & (1 << CLK_EIC);
    BPM->BPM_BKUPWEN = 1 << BPM_BKUPWEN_EIC;
    BPM->BPM_IORET = 0;
    BPM->BPM_IORET = 1;
}

void SetMcuFullPower()
{
    InitBoard();
    TurnOnAnalogDevices(OFF);
    EnableI2C(6);
}

/** \section BATTERY
 ///////////////////////////////////////////////////////////////////
 * Battery voltage monitoring and status control
 ///////////////////////////////////////////////////////////////////
 */

///battery_level
#define N_BAT_MEASUREMENT   20
int level_histogram[N_BAT_MEASUREMENT] = { NORMAL_LEVEL, NORMAL_LEVEL,
                                           NORMAL_LEVEL, NORMAL_LEVEL, NORMAL_LEVEL
                                         };
int battery_voltage[N_BAT_MEASUREMENT];
int battery_level = NORMAL_LEVEL;
unsigned int battery_mv = 3700;     //battery voltage in millivolts
const unsigned int kBatteryLevel[] = {3800, 3600};

/** \brief Get level of battery and save in internal variable
 *
 */
void GetBatteryLevel()
{
    if (IsADCFinished()) {
        battery_mv = GetmvADC();
        TriggerADC();

        if (battery_mv < kBatteryLevel[CRITICAL_LEVEL]) {
            //ToDo: performance action for critical level
            battery_level = CRITICAL_LEVEL;
        } else if (battery_mv < kBatteryLevel[WARNING_LEVEL]) {
            //ToDo: performance action for warning level
            battery_level = WARNING_LEVEL;
        } else {
            battery_level = NORMAL_LEVEL;
        }

        for ( int i = 0; i < N_BAT_MEASUREMENT; i++) {
            level_histogram[i] = level_histogram[i + 1];
            battery_voltage[i] = battery_voltage[i + 1];
        }

        level_histogram[N_BAT_MEASUREMENT - 1] = battery_level;
        battery_voltage[N_BAT_MEASUREMENT - 1] = battery_mv;
    }
}

/** \brief Get level of battery and save in internal variable
 *
 */
void UpdateBatteryLevel(unsigned int mv)
{
    battery_mv = mv;

    if (battery_mv < kBatteryLevel[CRITICAL_LEVEL]) {
        //ToDo: performance action for critical level
        battery_level = CRITICAL_LEVEL;
    } else if (battery_mv < kBatteryLevel[WARNING_LEVEL]) {
        //ToDo: performance action for warning level
        battery_level = WARNING_LEVEL;
    } else {
        battery_level = NORMAL_LEVEL;
    }

    for ( int i = 0; i < N_BAT_MEASUREMENT; i++) {
        level_histogram[i] = level_histogram[i + 1];
        battery_voltage[i] = battery_voltage[i + 1];
    }

    level_histogram[N_BAT_MEASUREMENT - 1] = battery_level;
    battery_voltage[N_BAT_MEASUREMENT - 1] = battery_mv;
}

/** \brief Check if battery level is critical
 *
 */
int IsBatteryCritical()
{
    for ( int i = 0; i < 5; i++) {
        if ( level_histogram[i] != CRITICAL_LEVEL) {
            return 0;
        }
    }

    return 1;
}

/** \brief
 *  Read level of the battery
 *  \return:
 *  current battery level in millivolts
 */
unsigned int ReadBatteryLevel()
{
    return battery_mv;
}

/** \section CIRCUIT SWITCH
 ///////////////////////////////////////////////////////////////////
 * Circuit power status enabling and disabling
 ///////////////////////////////////////////////////////////////////
 */

/** \brief Turn on/off analog devices
 *
 *  \param on: switch on or off
 */
void TurnOnAnalogDevices(int state)
{
    SetPinValue(AVDD_ON_PIN, state);
}

/** \brief Turn on/off general circuit
 *
 *  \param on: switch circuit on or off
 */
void SetCircuit(int state)
{
    SetPinValue(CRCT_ON_PIN, state > 0);
}


/** \brief
 *  Check if USB is connected
 *  \return
 *  1: USB is connected
 */
int IsUsbConnected()
{
    return (((GPIO->GPIO_PORT[0].GPIO_PVR) & (1 << VBUSDET)) > 0);
}

/** \section STATUS LED
 ///////////////////////////////////////////////////////////////////
 * Status led management functions (color depends on battery status)
 ///////////////////////////////////////////////////////////////////
 */

/** \brief
 *  turn on/off status led.
 *  Color depends on battery_level value:
 *  - 4/5: battery full, led green
 *  - 2/3: battery medium, led orange
 *  - 0/1: battery low, led red
 *  \param
 *  on: switch on or off
 */
void SetStatusLed(int state)
{
    if (state > 0) {
        if (battery_level == NORMAL_LEVEL) {
            SetPinValue(LEDGREEN, HIGH);
            SetPinValue(LEDRED, LOW);
        } else {
            SetPinValue(LEDRED, HIGH);
            SetPinValue(LEDGREEN, LOW);
        }
    } else {
        SetPinValue(LEDGREEN, LOW);
        SetPinValue(LEDRED, LOW);
    }

}

/** \brief
 *  Blink status led for 10000 cycles
 */
void BlinkStatusLed()
{
    int i;
    SetStatusLed(ON);

    for (i = 0; i < 200000; i++) {
    }

    SetStatusLed(OFF);
}

/** \brief
 *  Toggle status led
 */
void ToggleStatusLed()
{
    TogglePinValue(LEDGREEN);
}

void TurnOnSD(int state)
{
    if (state) {
        SetPinValue(SDPWR, HIGH);
        SetPinFunction(MISOPIN, FUNCTION_A);
#if BOARD_VERSION == 1
        SetPinFunction(MOSIPIN, FUNCTION_A);
#endif
        SetPinFunction(MSCKPIN, FUNCTION_A);
        SetPinFunction(SOFTCS1, FUNCTION_A);

    } else {
        SetPinValue(SDPWR, LOW);
        SetPinDirection(MISOPIN, INPUTPIN, PULLDISABLE);
#if BOARD_VERSION == 1
        SetPinDirection(MOSIPIN, INPUTPIN, PULLDISABLE);
#endif
        SetPinDirection(MSCKPIN, INPUTPIN, PULLDISABLE);
        SetPinDirection(SOFTCS1, INPUTPIN, PULLDISABLE);
    }
}

/** \brief
 * Configure watchdog
 */
void ConfigureWdt(unsigned int prescaler)
{
    WDT->WDT_CTRL = WDT_CTRL_KEY(0x55)
                    | (prescaler << WDT_CTRL_PSEL_Pos)
                    | (WDT_CTRL_CEN)
                    | (WDT_CTRL_EN);
    WDT->WDT_CTRL = WDT_CTRL_KEY(0xAA)
                    | (prescaler << WDT_CTRL_PSEL_Pos)
                    | (WDT_CTRL_CEN)
                    | (WDT_CTRL_EN);

    while ( (WDT->WDT_CTRL & WDT_CTRL_EN) == 0) { continue; }
}


void ResetWdt()
{
    unsigned int test;
    WDT->WDT_CLR = WDT_CLR_KEY(0x55)
                   | WDT_CLR_WDTCLR;
    WDT->WDT_CLR = WDT_CLR_KEY(0xAA)
                   | WDT_CLR_WDTCLR;

    if ( (WDT->WDT_SR & WDT_SR_CLEARED) != 0 ) {
        return;
    }

    while ((WDT->WDT_SR & WDT_SR_CLEARED) == 0) { continue; }
}
