/**
 * @file ds18b20_pri.h
 * @brief Internal private functions
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2023-08-31
 */

#ifndef DS18B20_PRI_H
#define DS18B20_PRI_H

// OneWire commands
#define SEARCHROMCMD    0xF0  // Search ROM command
#define MATCHCMD        0x55  // MATCH ROM cmd


#define STARTCONVO      0x44  // Tells device to take a temperature reading and put it on the scratchpad
#define COPYSCRATCH     0x48  // Copy scratchpad to EEPROM
#define READSCRATCH     0xBE  // Read from scratchpad
#define WRITESCRATCH    0x4E  // Write to scratchpad
#define RECALLSCRATCH   0xB8  // Recall from EEPROM to scratchpad
#define READPOWERSUPPLY 0xB4  // Determine if device needs parasite power
#define ALARMSEARCH     0xEC  // Query bus for devices with an alarm condition


#define AIR_TEMP_IFACE (0x1u << 16) /*P16*/
#define RAD_TEMP_IFACE (0x1u << 20) /*P20*/

bool DS18B20_readScratchPad( uint8_t* scratchPad, uint32_t iface, uint64_t add);
void DS18B20_writeScratchPad( const uint8_t* scratchPa, uint32_t iface, uint64_t add);
int32_t DS18B20_calculateTemperature( uint8_t* scratchPad);
bool DS18B20_oneWireSearch(uint8_t romCommand, uint32_t iface);
void DS18B20_requestTemperatures(uint32_t iface, uint64_t add);

void oneWire_begin(uint32_t iface);
uint8_t oneWire_reset(uint32_t iface);
void oneWire_write_bit(uint8_t v, uint32_t iface);
uint8_t oneWire_read_bit(uint32_t iface);
void oneWire_write(uint8_t v, uint32_t iface);
void oneWire_write_bytes(const uint8_t *buf, uint16_t count, uint32_t iface);
uint8_t oneWire_read(uint32_t iface);
void oneWire_read_bytes(uint8_t *buf, uint16_t count, uint32_t iface) ;
void oneWire_skip(uint32_t iface);
void oneWire_match(uint32_t iface, uint64_t add);

#endif
