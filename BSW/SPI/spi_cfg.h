/**
 * @file spi_cfg.h
 * @brief Config file for SPI module
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-26
 */

#ifndef SPI_CFG_H
#define SPI_CFG_H

/* Main clock is 96 Mhz and baudrate shall be 2 MHz, therefore the prescaler
 * shall be set to 48*/
#define DEBUG_MODE
#ifdef DEBUG_MODE
#define SPI_BAUDRATE_8_MHZ  32u
#else
#define SPI_BAUDRATE_8_MHZ  6u
#endif
/* Configure wait delay between CS falling edge and SPK rising/falling edge */
#define SPI_CFG_DLYBS       5u
/* Configure delay between consecutive transferes */
#define SPI_CFG_DLYBCT      1u

#endif
