/// spisw.c
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
/// Authored by:   JRB (05/12/2013)
/// Revised by:     AAL (16/12/2013)
/// Last Version:  09/12/2013
///
/// FILE CONTENTS:
/// Low level functions for bit bang SPI control

#include "sam4lc4c.h"
#include "configuration.h"
#include "mcu_peripheral.h"

#include "spisw.h"

/** \section brief
 *  Global variables to be used just under the scope of this file:
 *  CPOL: clock polarity (state on idle)
 *  CPHA: clock phase (data valid on leading:0 or trailing:1 clock edges)
 */

int bb_cpol = 0;
int bb_cpha = 1;

/** \brief
 *  Set the value of the internal configuration params: CPOL, CPHA
 */
void SpiSWConfigure(uint8_t my_cpol, uint8_t my_cpha)
{
    if (my_cpol > 0) { bb_cpol = 1; }
    else { bb_cpol = 0; }

    if (my_cpha > 0) { bb_cpha = 1; }
    else { bb_cpha = 0; }
}

/** \brief
 *  Execute a bit-bang SPI transfer: send+receive
 *
 *  Using direct register access to optimize transfer time
 *
 *  \param
 *  data: the byte of data to send
 *  \return
 *  The byte of data received
 */
uint8_t SpiSWTransfer(uint8_t data)
{
    uint8_t recv_data = 0;

    GPIO->GPIO_PORT[0].GPIO_OVRC = (1 << BBMOSI);

    if (bb_cpol > 0) {
        GPIO->GPIO_PORT[0].GPIO_OVRS = (1 << BBCLK);
    } else {
        GPIO->GPIO_PORT[0].GPIO_OVRC = (1 << BBCLK);
    }

    for (int i = 0; i < 8; i++) {
        recv_data = recv_data << 1;

        if (data > 127) {
            GPIO->GPIO_PORT[0].GPIO_OVRS = (1 << BBMOSI);
        } else {
            GPIO->GPIO_PORT[0].GPIO_OVRC = (1 << BBMOSI);
        }

        GPIO->GPIO_PORT[0].GPIO_OVRT = (1 << BBCLK);

        if (bb_cpha == 0) recv_data |= (((GPIO->GPIO_PORT[0].GPIO_PVR)
                                             & (1 << BBMISO)) > 0);

        data = data << 1;
        GPIO->GPIO_PORT[0].GPIO_OVRT = (1 << BBCLK);

        if (bb_cpha == 1) recv_data |= (((GPIO->GPIO_PORT[0].GPIO_PVR)
                                             & (1 << BBMISO)) > 0);

    }

    return recv_data;
}

/** \brief
 *  Configure pins to execute a bit-bang SPI communication
 */
void SpiSWInit()
{
    SetPinDirection(BBCLK, OUTPUTPIN, 0);
    SetPinDirection(BBMOSI, OUTPUTPIN, 0);
    SetPinDirection(BBMISO, INPUTPIN, 0);
    SetPinValue(BBCLK, bb_cpol);
    SetPinValue(BBMOSI, LOW);
}

