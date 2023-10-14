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


/**
 * @brief SPI driver initialitation
 *
 * SPI baudrate configured to run at 5 MHz
 */
void SPI_Init(void)
{
    /* set baudrate, 9 bits transfer NCPHA 1 and CPOL 0
     * Capture on rising and low when inactive
     */
    SPI->SPI_WPMR = SPI_WPMR_WPKEY_PASSWD;
    SPI->SPI_CR = SPI_CR_SPIDIS;

    SPI->SPI_MR =  SPI_MR_MSTR | SPI_MR_PCS(0) | SPI_MR_DLYBCS(5);
    SPI->SPI_CSR[0] =  SPI_CSR_NCPHA |  SPI_CSR_BITS_16_BIT |
                        SPI_CSR_SCBR(SPI_BAUDRATE_8_MHZ) |
                        SPI_CSR_DLYBS(SPI_CFG_DLYBS) |
                        SPI_CSR_DLYBCT(SPI_CFG_DLYBCT);

    SPI->SPI_CR = SPI_CR_SPIEN;

    SPI->SPI_WPMR = SPI_WPMR_WPKEY_PASSWD | SPI_WPMR_WPEN;
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
 * @param din Pointer to the array where the read data will be written
 * @param dout Pointer to the array with the data to be transmitted
 */
volatile uint32_t iteration = 0;
uint32_t spi_sr;
uint32_t spirx;

void SPI_sync_transmission(uint16_t len, const uint16_t* const din,
                           uint16_t* const dout)
{
    spi_sr = SPI->SPI_SR;
    while( (spirx & SPI_SR_TXEMPTY) == 0 ){ spirx = SPI->SPI_SR; }
    spirx = SPI->SPI_RDR & 0xFFFF; /* Read data to avoid overload*/

    if(len == 1){
        iteration = 0;
        spi_sr = SPI->SPI_SR;
        while( (spi_sr & SPI_SR_TDRE) == 0 ){
            iteration++;
            if(iteration > SPI_MAX_ITER){
                errorlog_reportError( SPI_MODULE, NULL, 0);
                break;
            }
            spi_sr = SPI->SPI_SR;
        }
        SPI->SPI_TDR = din[0];
        iteration = 0;
        spi_sr = SPI->SPI_SR;
        while( (spi_sr & SPI_SR_RDRF) == 0 ){
            iteration++;
            if(iteration > SPI_MAX_ITER){
                errorlog_reportError( SPI_MODULE, NULL, 0);
                break;
            }
            spi_sr = SPI->SPI_SR;
        }
        spirx = SPI->SPI_RDR;
        dout[0] = (uint16_t)(spirx & 0xFFFF); /* Read data */
    }else{
        iteration = 0;
        spi_sr = SPI->SPI_SR;
        while( (spi_sr & SPI_SR_TDRE) == 0 ){
            iteration++;
            if(iteration > SPI_MAX_ITER){
                errorlog_reportError( SPI_MODULE, NULL, 0);
                break;
            }
            spi_sr = SPI->SPI_SR;
        }
        SPI->SPI_TDR = din[0];
        iteration = 0;
        spi_sr = SPI->SPI_SR;
        while( (spi_sr & SPI_SR_TDRE) == 0 ){
            iteration++;
            if(iteration > SPI_MAX_ITER){
                errorlog_reportError( SPI_MODULE, NULL, 0);
                break;
            }
            spi_sr = SPI->SPI_SR;
        }
        SPI->SPI_TDR = din[1];
        iteration = 0;
        spi_sr = SPI->SPI_SR;
        while( (spi_sr & SPI_SR_RDRF) == 0 ){
            iteration++;
            if(iteration > SPI_MAX_ITER){
                errorlog_reportError( SPI_MODULE, NULL, 0);
                break;
            }
            spi_sr = SPI->SPI_SR;
        }
        spirx = SPI->SPI_RDR;
        dout[0] = (uint16_t)(spirx & 0xFFFF); /* Read data */
        iteration = 0;
        spi_sr = SPI->SPI_SR;
        while( (spi_sr & SPI_SR_RDRF) == 0 ){
            iteration++;
            if(iteration > SPI_MAX_ITER){
                errorlog_reportError( SPI_MODULE, NULL, 0);
                break;
            }
            spi_sr = SPI->SPI_SR;
        }
        spirx = SPI->SPI_RDR;
        dout[1] = (uint16_t)(spirx & 0xFFFF); /* Read data */
    }
}
