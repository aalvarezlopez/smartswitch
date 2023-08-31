/// calibration.c
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
/// Revised by:
/// Last Version:  16/12/2013
///
/// FILE CONTENTS:
/// sam4lc4c.h include macros and defines for register address and
///     values for this microcontroller.
/// configuration.h include definitios of the current board
/// mcu_peripheral.h function for manage features of the SAM4L microcontroller
/// debug.h this file include functions and defines to configure usb as VCP and
///             send debug message through it   external EEPROM

#include "sam4lc4c.h"
#include "configuration.h"
#include "mcu_peripheral.h"
#include "analog_utils.h"
#include "debug.h"
#include "delays.h"


#define VREF 1600

/** \section brief
 * Set, configure and save calibration parameters for
 *  each device and sensor
 */

/** \brief Set pga value and calibrate ADC converter
 *          whith VREF.
 *
 *   \params: sensor calibration in uv/g
 *   \return: value calibration to convert ADC Value
 *              to kG
 *
 */
unsigned int AdcCalibration(unsigned int mv_to_g)
{
    int value_adc;
    int sum = 0;
    int average, offset;

    ConfigAnalogChannel(CAL1);
    SetPGA113();
    delay_ms(200);
    ReadNADC(10);

    for (int i = 0; i < 10; i++) {
        value_adc = ReadNADC(100);
        sum += value_adc;
    }

    average = (-sum / 10);
    DebugDecWrite("avg: ", average);

    unsigned int  gain = average;
    gain = (gain * mv_to_g) / (VREF);

    DebugDecWrite("gain: ", gain);

    ConfigAnalogChannel(CH1);
    SetPGA113();
    return gain;
}
