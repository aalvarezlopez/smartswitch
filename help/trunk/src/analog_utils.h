/*
 * analog_utils.h
 *
 *  Created on: 10/12/2013
 *      Author: JRB
 */

#ifndef ANALOG_UTILS_H_
#define ANALOG_UTILS_H_

/** \section ADC
 /////////////////////////////////////////////////////////////////////////////
 * ADC functions and utilities
 /////////////////////////////////////////////////////////////////////////////
 */

int16_t ReadADC();
int16_t ReadNADC(int nsamples);
int16_t ReadAnalogIn(int nsamples);

/** \section PGA113
 /////////////////////////////////////////////////////////////////////////////
 *  PGA functions and utilities
 /////////////////////////////////////////////////////////////////////////////
 */

//PGA113 channel options:
#define CH0     0
#define CAL0    0       //Vcal
#define CH1     1
#define CAL1    0xC     //GND
#define CAL2    0xD     //0.9*Vcal

#define CAL3    0xE     //0.1*Vcal
#define CAL4    0xF     //Vref
#define PGA_WR_CONST    0x2A

void ConfigAnalogGain(uint8_t gain);
void ConfigAnalogChannel(uint8_t chp);
int GetAnalogGain();
int GetAnalogChannel();
void SetPGA113();
void SetupPGA113(uint8_t gain, uint8_t chp);

/** \section
 /////////////////////////////////////////////////////////////////////////////
 * LIS3DH device use and configure
 /////////////////////////////////////////////////////////////////////////////
 */
enum LIS3DREGS {
    STATUS_REG_AUX = 0x07,
    OUT_ADC1_L,
    OUT_ADC1_H,
    OUT_ADC2_L,
    OUT_ADC2_H,
    OUT_ADC3_L,
    OUT_ADC3_H,
    INT_COUNTER_REG,
    WHO_AM_I,

    TEMP_CFG_REG = 0x1F,
    CTRL_REG1,
    CTRL_REG2,
    CTRL_REG3,
    CTRL_REG4,
    CTRL_REG5,
    CTRL_REG6,
    REFERENCE,
    STATUS_REG2,
    OUT_X_L,
    OUT_X_H,
    OUT_Y_L,
    OUT_Y_H,
    OUT_Z_L,
    OUT_Z_H,
    FIFO_CTRL_REG,
    FIFO_SRC_REG,
    INT1_CFG,
    INT1_SOURCE,
    INT1_THS,
    INT1_DURATION,

    CLICK_CFG = 0x38,
    CLICK_SRC,
    CLICK_THS,
    TIME_LIMIT,
    TIME_LATENCY,
    TIME_WINDOW
};

#define WHO_AM_I_REPLY  0x33
#define ADDRESS_POS     8
#define SREAD           0x80
#define MREAD           0xC0
#define WRITE           0
#define INCREMENT       0x40
#define NOINCREMENT     0

#define X_AXIS  0
#define Y_AXIS  1
#define Z_AXIS  2

#define YH_INT_BIT      3
#define GAIN_BIT        4

#define GAIN2G  0
#define GAIN4G  1
#define GAIN8G  2
#define GAIN16G 3

#define LSV 15

void ConfIntLis3dh(unsigned int value_mg);
int InitLis3dh();
int16_t GetLisValue(int axis);
unsigned int GetStatusValue();
unsigned int GetInt1StValue();

unsigned int mGtoRawrmsConvert(unsigned int mg);

#endif /* ANALOG_UTILS_H_ */
