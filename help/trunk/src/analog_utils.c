/// analog_utils.c
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
/// Authored by:   JRB (10/12/2013)
/// Revised by:    JRB (Feb 2, 2014)
/// Last Version:
///
/// FILE CONTENTS:
/// Utilities for manage external analog devices as ADC converter (ADS8321), PGA (PGA113) and accelerometer (LIS3DH).

#include <stdint.h>
#include "sam4lc4c.h"
#include "configuration.h"
#include "spisw.h"
#include "debug.h"
#include "mcu_peripheral.h"
#include "analog_utils.h"
#include "params.h"

/** \section ADC
 /////////////////////////////////////////////////////////////////////////////
 * ADC functions and utilities
 /////////////////////////////////////////////////////////////////////////////
 */

/** \brief
 *  Read ADS8317 using spisw routines
 *
 *  \return
 *  ADC raw value (signed int -32768...+32767)
 */
int16_t ReadADC()
{
    uint32_t result;
    char aux;

    SetPinValue(SOFTCS0, LOW);

    aux = SpiSWTransfer(0);
    result = aux;

    aux = SpiSWTransfer(0);
    result = (result << 8) + aux;

    aux = SpiSWTransfer(0);
    result = (result << 8) + aux;

    SetPinValue(SOFTCS0, HIGH);

    result >>= 2;

    return (int16_t) result & 0xFFFF;
}

/** \brief
 *  Read ADC n times and return the mean
 *
 *  \param
 *  nsamples: number of samples to take
 *  \return
 *  Mean of all the returned values
 */
int16_t ReadNADC(int nsamples)
{
    uint8_t i = 0;
    int32_t sum = 0;

    do {
        sum += ReadADC();
        i++;
    } while (i <= nsamples);

    sum /= nsamples + 1;

    return (signed int) sum;
}

/** \brief
 *  Return value in volts for ADC reading
 *  \param
 *  nsamples: number of samples to make the mean
 *  \return
 *  Voltage value in millivolts (non-calibrated)
 */
int16_t ReadAnalogIn(int nsamples)
{
    int32_t result;

    result = ReadNADC(nsamples);

    //REVIEW: use defines
    result *= -153;
    result /= 1000;

    return (signed int) result;
}

/** \section PGA113
 /////////////////////////////////////////////////////////////////////////////
 *  PGA functions and utilities
 /////////////////////////////////////////////////////////////////////////////
 */

//PGA113 gain options:
const int kPgaGainValues[] = { 1, 2, 5, 10, 20, 50, 100, 200 };

//Global variables, local access, to return values in "Get" functions:
int pga_gain = 0;
int pga_channel = 0;

/** \brief
 *  Set PGA gain value (define internal value)
 *  \param
 *  gain: Gain setting (1 to 8). See const kPgaGainValues
 */
void ConfigAnalogGain(uint8_t gain)
{
    if (gain < 8) { pga_gain = gain; }
    else { pga_gain = 0; }
}

/** \brief
 *  Get PGA gain value
 *  \return
 *  Returns actual gain multiplier. See const kPgaGainValues
 */
int GetAnalogGain()
{
    return kPgaGainValues[pga_gain];
}


/** \brief
 *  Set PGA channel setting (define internal value)
 *  \param
 *  chp: channel setting. Only valid settings are applied
 */
void ConfigAnalogChannel(uint8_t chp)
{
    if (chp == CH1) { pga_channel = CH1; }
    else if (chp == CAL1) { pga_channel = CAL1; }
    else if (chp == CAL2) { pga_channel = CAL2; }
    else if (chp == CAL3) { pga_channel = CAL3; }
    else if (chp == CAL4) { pga_channel = CAL4; }
    else { pga_channel = CH0; }
}


/** \brief
 *  Get PGA channel setting (define internal value)
 *  \return
 *  channel setting
 */
int GetAnalogChannel()
{
    return pga_channel;
}


/** \brief
 *  Load PGA113 with the selected values (internal values)
 *  \param
 *  None
 */
void SetPGA113()
{
    char aux = 0;

    aux = pga_gain << 4;
    aux |= pga_channel;
    SetPinValue(SOFTCS3, LOW);

    SpiSWTransfer(PGA_WR_CONST);
    SpiSWTransfer(aux);

    SetPinValue(SOFTCS3, HIGH);
}

/** \brief
 *  Set internal PGA values and load them to the device
 *  \param
 *  gain: gain setting
 *  chp: channel setting
 */
void SetupPGA113(uint8_t gain, uint8_t chp)
{
    ConfigAnalogChannel(chp);
    ConfigAnalogGain(gain);
    SetPGA113();
}

/**
 *  \section LIS3DH
 /////////////////////////////////////////////////////////////////////////////
 * LIS3DH accelerometer device use and configure
 /////////////////////////////////////////////////////////////////////////////
 */

int current_gain = 0;

/** \brief
 *  Configures LIS3DH chip for the desired settings.
 *  \return
 *  1 if successful error code in other case
 */
int InitLis3dh(Params myParams)
{
    int16_t k = 0;

    SetPinValue(SOFTCS2, LOW);
    SpiSWTransfer(SREAD | WHO_AM_I);
    //REVIEW 0xFF could be defined as DUMMY_BYTE OR BYTE_4_READ
    k = SpiSWTransfer(0xFF);
    SetPinValue(SOFTCS2, HIGH);

    //TODO: debug lines. Should return just error (0)
    //if who-am-i didn't execute correctly
    if (k != WHO_AM_I_REPLY) { return k; }

    //REVIEW: use defines for register values
    SetPinValue(SOFTCS2, LOW);
    SpiSWTransfer(CTRL_REG1);          //CTRL_REG1
    k = SpiSWTransfer(
            0x52);          //Enable Y-axis, ODR= 100Hz (WARNING: x52 NOT xA2 AS THE APPNOTE SAYS!!)
    SetPinValue(SOFTCS2, HIGH);

    SetPinValue(SOFTCS2, LOW);
    SpiSWTransfer(CTRL_REG2);          //CTRL_REG2
    k = SpiSWTransfer(
            0x09);          //High-pass filter enabled on data and interrupt1
    SetPinValue(SOFTCS2, HIGH);

    SetPinValue(SOFTCS2, LOW);
    SpiSWTransfer(CTRL_REG3);          //CTRL_REG3
    k = SpiSWTransfer(0x40);          //Interrupt driven to INT1 pad
    SetPinValue(SOFTCS2, HIGH);

    SetPinValue(SOFTCS2, LOW);
    SpiSWTransfer(CTRL_REG5);          //CTRL_REG5
    k = SpiSWTransfer(0x08);          // Interrupt latched
    SetPinValue(SOFTCS2, HIGH);

    ConfIntLis3dh(myParams.lwakeup);

    SetPinValue(SOFTCS2, LOW);
    SpiSWTransfer(INT1_DURATION);          //INT1_DURATION
    k = SpiSWTransfer(0x00);          // 0ms
    SetPinValue(SOFTCS2, HIGH);

    SetPinValue(SOFTCS2, LOW);
    SpiSWTransfer(INT1_CFG);          //INT1_CFG
    k = SpiSWTransfer(0x08);          // Enable YH interrupt generation
    SetPinValue(SOFTCS2, HIGH);

    return 1;
}

/** \brief
 *  Returns raw acceleration value (-128 to 128)
 *
 *  \param
 *  axis: 0-X, 1-Y, 2-Z
 */
int16_t GetLisValue(int axis)
{
    int16_t value = 0;
    int axis_reference = OUT_X_L + 2 * axis;

    SetPinValue(SOFTCS2, LOW);
    SpiSWTransfer(MREAD | axis_reference);
    value = SpiSWTransfer(0xFF);
    value |= SpiSWTransfer(0xFF) << 8;
    SetPinValue(SOFTCS2, HIGH);

    //REVIEW 256 should be defined as macro
    return (value / 256);
}

/** \brief
 *  Returns status1 and status2
 *
 */
unsigned int GetStatusValue()
{
    unsigned int value;
    SetPinValue(SOFTCS2, LOW);
    SpiSWTransfer(SREAD | STATUS_REG_AUX);
    value = SpiSWTransfer(0xFF);
    SetPinValue(SOFTCS2, HIGH);
    value <<= 8;

    SetPinValue(SOFTCS2, LOW);
    SpiSWTransfer(SREAD | STATUS_REG2);
    value |= SpiSWTransfer(0xFF);
    SetPinValue(SOFTCS2, HIGH);
    return value;
}

/** \brief
 *  Returns interruption 1 status register
 *
 */
unsigned int GetInt1StValue()
{
    unsigned int value;
    SetPinValue(SOFTCS2, LOW);
    SpiSWTransfer(SREAD | INT1_SOURCE);
    value = SpiSWTransfer(0xFF);
    SetPinValue(SOFTCS2, HIGH);
    return (value & 0xFF);
}

/** \brief
 *  Configure interruption value for LIS3DH
 *
 */
void ConfIntLis3dh(unsigned int value_mg)
{
    int value;
    int gain;

    // Threshold value_mg = value LSB * mg/LSB * GAIN
    if (value_mg > 8000) {
        gain = GAIN16G;
        value = value_mg / (LSV * 8);
    } else if (value_mg > 4000) {
        gain = GAIN8G;
        value = value_mg / (LSV * 4);
    } else if (value_mg > 2000) {
        gain = GAIN4G;
        value = value_mg / (LSV * 2);
    } else {
        gain = GAIN2G;
        value = value_mg / (LSV);
    }

    current_gain = gain;

    SetPinValue(SOFTCS2, LOW);
    SpiSWTransfer(CTRL_REG4);          //CTRL_REG4
    SpiSWTransfer(0x80 | (gain << GAIN_BIT));          // BDU
    SetPinValue(SOFTCS2, HIGH);

    SetPinValue(SOFTCS2, LOW);
    SpiSWTransfer(INT1_THS);          //INT1_THS
    SpiSWTransfer(value);
    SetPinValue(SOFTCS2, HIGH);
}


/** \brief
 *  Convert mg to lisd3h output value and return rms to compare with
 */
unsigned int mGtoRawConvert(unsigned int mg)
{
    unsigned int raw;
    unsigned int full_scale_value;

    if ( current_gain == GAIN2G) {
        full_scale_value = 2000;
    } else if (current_gain == GAIN4G) {
        full_scale_value = 4000;
    } else if (current_gain == GAIN8G) {
        full_scale_value = 8000;
    } else {
        full_scale_value = 16000;
    }

    raw = (mg * 128) / full_scale_value;

    return (raw);
}
