/**
 * @file i2c.c
 * @brief I2C driver for SAM4S4A
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-14
 */

#include "stdint.h"
#include <stdio.h>
#include "i2c.h"
#include "i2c_cfg.h"
#include "sam4s4a.h"
#include "component_twi.h"
#include "errorlog.h"


/**
 * @brief I2C driver initialitation
 *
 * I2C peripheral configuration
 */
void I2C_Init(void)
{
    /* Main clock is 96 MHz. We're going to divice it into roughly 1280 
     * CKDIV = 7 => 2^7=128
     * CHDIV/CLDIV = 10
     */
    TWI0->TWI_CR = TWI_CR_SWRST;
    TWI0->TWI_CWGR = TWI_CWGR_CLDIV(8) |
                     TWI_CWGR_CHDIV(8) |
                     TWI_CWGR_CKDIV(3);
    TWI0->TWI_MMR = 0x0000 |
                    TWI_MMR_IADRSZ_NONE |
                    TWI_MMR_DADR(0x3C);
    TWI0->TWI_CR = TWI_CR_SVDIS;
    TWI0->TWI_CR = TWI_CR_MSEN;
    TWI0->TWI_CR = TWI_CR_START;
    TWI0->TWI_CR = TWI_CR_STOP;
}



/**
 * @brief I2C task to monitor driver
 */
void I2C_Task(void)
{
}


void I2C_sync_transmission(const uint8_t *data, uint8_t len)
{
    ISR_disableAllInterrupts();
    TWI0->TWI_MMR = 0x0000 |
                    TWI_MMR_IADRSZ_NONE |
                    TWI_MMR_DADR(0x3C);
    for(uint8_t i = 0; i < len; i++){
        while( (TWI0->TWI_SR & TWI_SR_TXRDY) == 0){
            continue;
        }
        TWI0->TWI_THR = data[i];
    }
    while( (TWI0->TWI_SR & TWI_SR_TXCOMP) == 0){
        continue;
    }
    TWI0->TWI_CR = TWI_CR_STOP;
    ISR_enableAllInterrupts();
}
