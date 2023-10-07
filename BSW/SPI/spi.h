/**
 * @file spi.h
 * @brief SPI header driver for SAM4S$a
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-14
 */
#ifndef SPI_H
#define SPI_H

#define SPI_MAX_ITER 100u

void SPI_Init(void);
void SPI_Task(void);
void SPI_sync_transmission(uint16_t len, const uint8_t* const din,
                           uint16_t* const dout);

#endif
