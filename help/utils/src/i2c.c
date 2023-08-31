/// i2c.c
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
/// Authored by:   Adrian (10/12/2013)
/// Revised by:    JRB (16/12/2013)
/// Last Version:  10/12/2013
///
/// FILE CONTENTS:
/// this file include functions and defines to configure
/// and use hardware I2c

#include "sam4lc4c.h"
#include "configuration.h"
#include "i2c.h"
#include "mcu_peripheral.h"
#include "debug.h"

/** \section brief
 *
 * Basic function to manage i2c for sam4l
 *
 */

/** \brief I2C clock enable and configuration
 *
 */
void EnableI2CPeripheralClock()
{
    //REVIEW: defines??
    PM->PM_UNLOCK = (0xAA << 24) | 0x24;
    PM->PM_HSBMASK |= 1 << 5;
    PM->PM_UNLOCK = (0xAA << 24) | 0x28;
    PM->PM_PBAMASK |= (PM_PBAMASK_TWIM2);
}

/** \brief I2C enable and configure I2c
 *
 */
void EnableI2C(unsigned int clockDivisor)
{
    EnableI2CPeripheralClock();
    //configure pins SDA,SCL
    SetPinFunction(SDAPIN, FUNCTION_E);
    SetPinFunction(SCLPIN, FUNCTION_E);

    /* Enable master transfer */
    TWIM2->TWIM_CR = TWIM_CR_MEN;
    /* Reset TWI */
    TWIM2->TWIM_CR = TWIM_CR_SWRST;

    /* Select the speed */
    TWIM2->TWIM_CWGR = TWIM_CWGR_LOW(239 / 2)          //48 MHz/239/2 = 100 kHz
                       | TWIM_CWGR_HIGH(239 - 239 / 2)
                       | TWIM_CWGR_EXP(0)
                       | TWIM_CWGR_DATA(0)
                       | TWIM_CWGR_STASTO(239);
}

/** \brief I2C write data
 *
 *  \param address: device to send data
 *         data: array of datas to send
 *         len:  number of bytes to send
 *
 *  \return 0 data not send
 *          >1 n data send successfuly
 */
int I2CWriteData(unsigned int address, const unsigned int* data,
                 unsigned int n_bytes)
{
    volatile int timeout = I2CTIMEOUT;
    int i;
    /* Reset the TWIM module to clear the THR register */
    TWIM2->TWIM_CR = TWIM_CR_MEN;
    TWIM2->TWIM_CR = TWIM_CR_SWRST;
    TWIM2->TWIM_CR = TWIM_CR_MDIS;
    /* Initiate the transfer to send the data */
    TWIM2->TWIM_CMDR = TWIM_CMDR_SADR(address) | TWIM_CMDR_NBYTES(n_bytes)
                       | TWIM_CMDR_VALID
                       | TWIM_CMDR_START
                       | TWIM_CMDR_STOP;

    /* Enable master transfer */
    TWIM2->TWIM_CR = TWIM_CR_MEN;

    for (i = 0; i < n_bytes; i++) {
        while ((TWIM2->TWIM_SR & TWIM_SR_TXRDY) == 0) {
            timeout--;

            if (timeout <= 0) { return i; }
        }

        TWIM2->TWIM_THR = data[i];
        timeout = I2CTIMEOUT;
    }

    //wait transfer ends
    timeout = I2CTIMEOUT;

    while ((TWIM2->TWIM_SR & TWIM_SR_STOP) == 0) {
        timeout--;

        if (timeout <= 0) { return i; }
    }

    return i;
}

/** \brief I2C read data
 *
 *  \param address: device to communicate with
 *         data: array of data read
 *         len:  number of bytes to read
 *
 *  \return 0 data not read
 *          >1 n data read successfuly
 */
int I2CReadData(unsigned int address, unsigned int* data, unsigned int n_bytes)
{
    volatile int timeout = I2CTIMEOUT;
    int i;
    /* Reset the TWIM module to clear the THR register */
    TWIM2->TWIM_CR = TWIM_CR_MEN;
    TWIM2->TWIM_CR = TWIM_CR_SWRST;
    TWIM2->TWIM_CR = TWIM_CR_MDIS;
    /* Initiate the transfer to read the data */
    TWIM2->TWIM_CMDR = TWIM_CMDR_SADR(address)
                       | TWIM_CMDR_NBYTES(n_bytes)
                       | TWIM_CMDR_VALID
                       | TWIM_CMDR_START
                       | TWIM_CMDR_READ
                       | TWIM_CMDR_STOP;

    /* Enable master transfer */
    TWIM2->TWIM_CR = TWIM_CR_MEN;

    for (i = 0; i < n_bytes; i++) {
        while ((TWIM2->TWIM_SR & TWIM_SR_RXRDY) == 0) {
            timeout--;

            if (timeout <= 0) { return i; }
        }

        data[i] = TWIM2->TWIM_RHR;
        timeout = I2CTIMEOUT;
    }

    return i;
}

/** \brief I2C write and read data
 *
 *  \param address: device to communicate with
 *         dataToSend: array of data to send
 *         bytesToSend: number or bytes to write
 *         dataRead: array of data read
 *         bytesToRead:  number of bytes to read
 *
 *  \return 0 fail
 *          1 data write/read successfuly
 */
int I2CWriteReadData(unsigned int address, const unsigned int* dataToSend,
                     int bytes_to_send, unsigned int* data_read, unsigned int bytes_to_read)
{
    volatile int timeout = I2CTIMEOUT;
    int i;
    /* Reset the TWIM module to clear the THR register */
    TWIM2->TWIM_CR = TWIM_CR_MEN;
    TWIM2->TWIM_CR = TWIM_CR_SWRST;
    TWIM2->TWIM_CR = TWIM_CR_MDIS;
    /* Initiate the transfer to send the data */
    TWIM2->TWIM_CMDR = TWIM_CMDR_SADR(address) | TWIM_CMDR_NBYTES(bytes_to_send)
                       | TWIM_CMDR_VALID
                       | TWIM_CMDR_START;
    TWIM2->TWIM_NCMDR = TWIM_NCMDR_SADR(
                            address) | TWIM_NCMDR_NBYTES(bytes_to_read)
                        | TWIM_NCMDR_VALID
                        | TWIM_NCMDR_START
                        | TWIM_NCMDR_READ
                        | TWIM_NCMDR_STOP;

    /* Enable master transfer */
    TWIM2->TWIM_CR = TWIM_CR_MEN;

    //Send data
    for (i = 0; i < bytes_to_send; i++) {
        while ((TWIM2->TWIM_SR & TWIM_SR_TXRDY) == 0) {
            timeout--;

            if (timeout <= 0) { return 0; }
        }

        TWIM2->TWIM_THR = dataToSend[i];
        timeout = I2CTIMEOUT;
    }

    //Read data
    for (i = 0; i < bytes_to_read; i++) {
        while ((TWIM2->TWIM_SR & TWIM_SR_RXRDY) == 0) {
            timeout--;

            if (timeout <= 0) { return i; }
        }

        data_read[i] = TWIM2->TWIM_RHR;
        timeout = I2CTIMEOUT;
    }

    return 1;
}


/** \brief I2C check device is listening
 *
 *  \param address: device to communicate with
 *
 *  \return 0 fail
 *          1 device available
 */
int CheckDevice(unsigned int address)
{
    int timeout = I2CTIMEOUT;
    int i;
    /* Reset the TWIM module to clear the THR register */
    TWIM2->TWIM_CR = TWIM_CR_MEN;
    TWIM2->TWIM_CR = TWIM_CR_SWRST;
    TWIM2->TWIM_CR = TWIM_CR_MDIS;
    /* Initiate the transfer to send the data */
    TWIM2->TWIM_CMDR =  TWIM_CMDR_SADR(address)
                        | TWIM_CMDR_NBYTES(0)
                        | TWIM_CMDR_VALID
                        | TWIM_CMDR_START
                        | TWIM_CMDR_STOP;

    /* Enable master transfer */
    TWIM2->TWIM_CR = TWIM_CR_MEN;

    while ((TWIM2->TWIM_SR & TWIM_SR_CCOMP) == 0) {
        timeout--;

        if (timeout <= 0) {
            return 0;
        }

        if ((TWIM2->TWIM_SR & TWIM_SR_ANAK) != 0 ||
            (TWIM2->TWIM_SR & TWIM_SR_DNAK) != 0) {
            return 0;
        }
    }

    return 1;
}
