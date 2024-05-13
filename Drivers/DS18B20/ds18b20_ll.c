/**
 * @file ds18b20.c
 * @brief Temperature sensor
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2023-08-31
 */


#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include "ds18b20.h"
#include "ds18b20_pri.h"
#include "io.h"
#include "delays.h"

#include "errorlog.h"

uint8_t oneWire_reset(uint32_t iface)
{
    uint8_t r;
    uint8_t retries = 125;

    IO_oneWire_Output(iface);
    do {
        if (--retries == 0){
            errorlog_reportError( DS18B20_MODULE, NULL, 0);
        }
        delay_us(2);
    } while ( IO_oneWire_Read(iface) == false );

    IO_oneWire_Clear(iface);
    delay_us(480);
    IO_oneWire_Input(iface);
    delay_us(70);
    r = IO_oneWire_Read(iface) == true ? 1 : 0;
    delay_us(490);
    return r;
}

void oneWire_begin( uint32_t iface)
{
    oneWire_reset(iface);
}

void oneWire_write_bit(uint8_t v, uint32_t iface)
{
    IO_oneWire_Output(iface);
    IO_oneWire_Clear(iface);
    if (v & 1) {
        delay_us(10);
        IO_oneWire_Set(iface);
        delay_us(55);
    } else {
        delay_us(65);
        IO_oneWire_Set(iface);
        delay_us(5);
    }
}

uint8_t oneWire_read_bit(uint32_t iface)
{
    uint8_t r;

    IO_oneWire_Output(iface);
    IO_oneWire_Clear(iface);
    delay_us(2);
    IO_oneWire_Input(iface);
    delay_us(10);
    r = IO_oneWire_Read(iface) == true ? 1 : 0;
    delay_us(53);
    return r;
}

void oneWire_write(uint8_t v, uint32_t iface)
{
    uint8_t bitMask;

    for (bitMask = 0x01; bitMask; bitMask <<= 1) {
        oneWire_write_bit( (bitMask & v)?1:0, iface);
    }
}

void oneWire_write_bytes(const uint8_t *buf, uint16_t count, uint32_t iface)
{
    for (uint16_t i = 0 ; i < count ; i++){
        oneWire_write(buf[i], iface);
    }
}

uint8_t oneWire_read(uint32_t iface)
{
    uint8_t bitMask;
    uint8_t r = 0;

    for (bitMask = 0x01; bitMask; bitMask <<= 1) {
        if ( oneWire_read_bit(iface)) r |= bitMask;
    }
    return r;
}

void oneWire_read_bytes(uint8_t *buf, uint16_t count, uint32_t iface) {
    for (uint16_t i = 0 ; i < count ; i++)
        buf[i] = oneWire_read(iface);
}

void oneWire_match(uint32_t iface, uint64_t add)
{
    uint64_t temp = add;
    oneWire_write(MATCHCMD, iface);
    oneWire_write(temp & 0xFF, iface); // Family byte
    temp >>= 8;
    for(uint8_t i = 0; i < 6; i++){
        oneWire_write(temp, iface); // Single ID byte
        temp >>= 8;
    }
    oneWire_write(temp & 0xFF, iface);  // CRC byte
}

void oneWire_skip(uint32_t iface)
{
    oneWire_write(0xCC, iface);
}

#if ONEWIRE_CRC
// The 1-Wire CRC scheme is described in Maxim Application Note 27:
// "Understanding and Using Cyclic Redundancy Checks with Maxim iButton Products"
//

#if ONEWIRE_CRC8_TABLE
// Dow-CRC using polynomial X^8 + X^5 + X^4 + X^0
// Tiny 2x16 entry CRC table created by Arjen Lentz
// See http://lentz.com.au/blog/calculating-crc-with-a-tiny-32-entry-lookup-table
static const uint8_t PROGMEM dscrc2x16_table[] = {
    0x00, 0x5E, 0xBC, 0xE2, 0x61, 0x3F, 0xDD, 0x83,
    0xC2, 0x9C, 0x7E, 0x20, 0xA3, 0xFD, 0x1F, 0x41,
    0x00, 0x9D, 0x23, 0xBE, 0x46, 0xDB, 0x65, 0xF8,
    0x8C, 0x11, 0xAF, 0x32, 0xCA, 0x57, 0xE9, 0x74
};

// Compute a Dallas Semiconductor 8 bit CRC. These show up in the ROM
// and the registers.  (Use tiny 2x16 entry CRC table)
uint8_t oneWire_crc8(const uint8_t *addr, uint8_t len)
{
    uint8_t crc = 0;

    while (len--) {
        crc = *addr++ ^ crc;  // just re-using crc as intermediate
        crc = pgm_read_byte(dscrc2x16_table + (crc & 0x0f)) ^
            pgm_read_byte(dscrc2x16_table + 16 + ((crc >> 4) & 0x0f));
    }

    return crc;
}
#else
//
// Compute a Dallas Semiconductor 8 bit CRC directly.
// this is much slower, but a little smaller, than the lookup table.
//
uint8_t oneWire_crc8(const uint8_t *addr, uint8_t len)
{
    uint8_t crc = 0;

    while (len--) {
        #if defined(__AVR__)
        crc = _crc_ibutton_update(crc, *addr++);
        #else
        uint8_t inbyte = *addr++;
        for (uint8_t i = 8; i; i--) {
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix) crc ^= 0x8C;
            inbyte >>= 1;
        }
        #endif
    }
    return crc;
}
#endif
#endif
