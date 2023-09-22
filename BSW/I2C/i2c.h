/**
 * @file spi.h
 * @brief I2C header driver for SAM4S$a
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-14
 */
#ifndef I2C_H
#define I2C_H


void I2C_Init(void);
void I2C_Task(void);
void I2C_sync_transmission(const uint8_t *data, uint8_t len);

#endif
