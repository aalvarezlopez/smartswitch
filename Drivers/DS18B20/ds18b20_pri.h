/**
 * @file ds18b20_pri.h
 * @brief Internal private functions
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2023-08-31
 */

#ifndef DS18B20_PRI_H
#define DS18B20_PRI_H

bool DS18B20_readScratchPad( uint8_t* scratchPad );
void DS18B20_writeScratchPad( const uint8_t* scratchPad);
int32_t DS18B20_calculateTemperature( uint8_t* scratchPad);

void oneWire_begin(uint8_t pin);
uint8_t oneWire_reset(void);
void oneWire_write_bit(uint8_t v);
uint8_t oneWire_read_bit(void);
void oneWire_write(uint8_t v);
void oneWire_write_bytes(const uint8_t *buf, uint16_t count);
uint8_t oneWire_read();
void oneWire_read_bytes(uint8_t *buf, uint16_t count) ;
void oneWire_skip();

#endif
