/**
 * @file spi.c
 * @brief SPI driver for SAM4S4A
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-14
 */

#include <stdio.h>
#include <stdint.h>
#include "spi.h"
#include "spi_cfg.h"
#include "sam4s4a.h"
#include "component_spi.h"
#include "errorlog.h"
#include "delays.h"

static uint8_t *ptrdout = NULL;
static volatile uint16_t rcvFrames = 0;
volatile uint32_t spiflags = 0;
volatile uint32_t spitx = 0;
volatile uint32_t spirx = 0;
volatile uint32_t spiisr = 0;
volatile uint32_t spiin = 0;
volatile uint32_t spiout = 0;

void SPI_Handler(void)
{
    spiin++;
    spiflags = 0x1515AAAA;
    spiisr++;
    if( (SPI->SPI_SR & SPI_SR_RDRF) != 0 ){
        spirx++;
        *(ptrdout) = SPI->SPI_RDR;
        ptrdout++;
        rcvFrames++;
    }
    spiflags = 0x1515BBBB;
    spiout++;
}

/**
 * @brief SPI driver initialitation
 *
 * SPI baudrate configured to run at 8 MHz
 */
void SPI_Init(void)
{
    /* set baudrate, 9 bits transfer NCPHA 1 and CPOL 0
     * Capture on rising and low when inactive
     */
    SPI->SPI_WPMR = SPI_WPMR_WPKEY_PASSWD;
    SPI->SPI_CR = SPI_CR_SPIDIS;

    SPI->SPI_MR =  SPI_MR_MSTR | SPI_MR_PCS(0) | SPI_MR_DLYBCS(5);
    SPI->SPI_CSR[0] =  SPI_CSR_NCPHA |  SPI_CSR_BITS_8_BIT |
                        SPI_CSR_SCBR(SPI_BAUDRATE_8_MHZ) |
                        SPI_CSR_DLYBS(SPI_CFG_DLYBS) |
                        SPI_CSR_DLYBCT(SPI_CFG_DLYBCT);

    SPI->SPI_CR = SPI_CR_SPIEN;

    SPI->SPI_WPMR = SPI_WPMR_WPKEY_PASSWD | SPI_WPMR_WPEN;
    ISR_setInterruptEnable((IRQn_Type) ID_SPI, true);
    SPI->SPI_IER = SPI_IER_RDRF;
}



/**
 * @brief SPI task to monitor driver
 */
void SPI_Task(void)
{
}


/**
 * @brief Transmit commands through the SPI bus
 *
 * @param len Number of bytes to be transmitted
 * @param txbuff Pointer to the array where the read data will be written
 * @param rxbuff Pointer to the array with the data to be transmitted
 */
volatile uint32_t iteration = 0;

void SPI_sync_transmission(uint16_t len, const uint8_t* const txbuff,
                           uint8_t* const rxbuff)
{
    uint32_t txtemp;
    ptrdout = rxbuff;
    rcvFrames = 0;
    spiflags = 0xBABEAAAA;
    spitx = 0;
    spirx = 0;
    spiisr = 0;

    while( (SPI->SPI_SR & SPI_SR_TXEMPTY) == 0 ){ continue; }

    for(uint16_t i = 0; i < len; i++){
        iteration = 0;
        while( (SPI->SPI_SR & SPI_SR_TDRE) == 0 ){
            iteration++;
            if(iteration > SPI_MAX_ITER){
                errorlog_reportError( SPI_MODULE, NULL, 0);
                break;
            }
        }
        txtemp = (uint32_t)txbuff[i];
        SPI->SPI_TDR = txtemp;
        spitx ++;
    }
    while(rcvFrames < len){ continue; }
    spiflags = 0xBABEBBBB;
}
