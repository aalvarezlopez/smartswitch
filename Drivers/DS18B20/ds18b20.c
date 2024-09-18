/**
 * @version 1.0.0
 * @date 2023-08-31
 */

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"

#include "ds18b20.h"

#include "ds18b20_pri.h"
#include "delays.h"

#include "smartswitch.h"

// Scratchpad locations
#define TEMP_LSB        0
#define TEMP_MSB        1
#define HIGH_ALARM_TEMP 2
#define LOW_ALARM_TEMP  3
#define CONFIGURATION   4
#define INTERNAL_BYTE   5
#define COUNT_REMAIN    6
#define COUNT_PER_C     7
#define SCRATCHPAD_CRC  8

// DSROM FIELDS
#define DSROM_FAMILY    0
#define DSROM_CRC       7

// Device resolution
#define TEMP_9_BIT  0x1F //  9 bit
#define TEMP_10_BIT 0x3F // 10 bit
#define TEMP_11_BIT 0x5F // 11 bit
#define TEMP_12_BIT 0x7F // 12 bit

#define MAX_CONVERSION_TIMEOUT      750

uint8_t scratchPad[9];
uint32_t temp_01;

uint8_t lastDiscrepancy = 0;
uint64_t address = 0;

uint64_t iface1_add[MAX_WATER_TEMP_SENSORS];
uint64_t iface1_ndevs = 0;
uint32_t iface1_temps[MAX_WATER_TEMP_SENSORS];

void DS18B20_Init(void)
{
    oneWire_reset(AIR_TEMP_IFACE);
    DS18B20_oneWireSearch(SEARCHROMCMD, AIR_TEMP_IFACE);

    while(true){
        bool lastDevice;
        oneWire_reset(RAD_TEMP_IFACE);
        lastDevice = DS18B20_oneWireSearch(SEARCHROMCMD, RAD_TEMP_IFACE);
        if( address != 0){
            iface1_add[iface1_ndevs] = address;
            iface1_ndevs++;
        }
        if( lastDevice == true ){
            break;
        }
    }

    (void)DS18B20_readScratchPad(scratchPad, AIR_TEMP_IFACE, 0);
    DS18B20_requestTemperatures(AIR_TEMP_IFACE, 0);

    for( uint8_t i = 0; i < iface1_ndevs; i++){
        (void)DS18B20_readScratchPad(scratchPad, RAD_TEMP_IFACE, iface1_add[i]);
        DS18B20_requestTemperatures(RAD_TEMP_IFACE, iface1_add[i]);
    }
}

void DS18B20_Task(void)
{
    static uint8_t iface1_lastidx = 0;
    if(DS18B20_readScratchPad(scratchPad, AIR_TEMP_IFACE, 0)){
        temp_01 = DS18B20_calculateTemperature(scratchPad);
        DS18B20_requestTemperatures(AIR_TEMP_IFACE, 0);
    }
    if(DS18B20_readScratchPad(scratchPad, RAD_TEMP_IFACE, iface1_add[iface1_lastidx])){
        iface1_temps[iface1_lastidx] = DS18B20_calculateTemperature(scratchPad);
        iface1_lastidx++;
        if(iface1_lastidx >= iface1_ndevs){
            iface1_lastidx = 0;
        }
        DS18B20_requestTemperatures(RAD_TEMP_IFACE, iface1_add[iface1_lastidx]);
    }
}


/**
 * @brief 
 *
 * currentBit currentBitComp
 * 0 0 DISCREPANCY: there are both 0s and 1s
 * 0 1 There are only 0
 * 1 0 There are only 1
 * 1 1 No device participating
 *
 * @param romCommand
 *
 * @return 
 */
bool DS18B20_oneWireSearch(uint8_t romCommand, uint32_t iface)
{
    uint8_t lastZero = 0;
    uint8_t direction, currentBit, currentBitComp;
    bool    lastDevice = true;
    address = 0;

    oneWire_write(romCommand, iface);

    for (uint8_t bitPosition = 0; bitPosition < 64; bitPosition++) {
        currentBit = oneWire_read_bit(iface);
        currentBitComp = oneWire_read_bit(iface);

        if (currentBit == 1 && currentBitComp == 1) {
            /* No devices participating in search */
            lastDiscrepancy = 0;
            return true;
        }else if (currentBit == 0 && currentBitComp == 0) {
            if (bitPosition == lastDiscrepancy) {
                direction = 1;
            } else if (bitPosition > lastDiscrepancy) {
                direction = 0;
                lastZero = bitPosition;
            } else {
                direction = (address >> bitPosition) & 1u;

                if ( direction == 0) {
                    lastZero = bitPosition;
                }
            }
        } else {
            direction = currentBit;
        }
        address = address | ((uint64_t)direction << bitPosition);
        oneWire_write_bit(direction, iface);
    }

    lastDiscrepancy = lastZero;

    if (lastZero != 0) {
        lastDevice = false;
    }

    return lastDevice;
}


bool DS18B20_readScratchPad( uint8_t* scratchPad, uint32_t iface, uint64_t add )
{
    // send the reset command and fail fast
    bool result = false;
    int b = oneWire_reset(iface);
    if (b == 0){ 
        if( add == 0 ){
            oneWire_skip(iface);
        }else{
            oneWire_match(iface, add);
        }
        oneWire_write(READSCRATCH, iface);

        // Read all registers in a simple loop
        // byte 0: temperature LSB
        // byte 1: temperature MSB
        // byte 2: high alarm temp
        // byte 3: low alarm temp
        // byte 4: DS18S20: store for crc
        //         DS18B20 & DS1822: configuration register
        // byte 5: internal use & crc
        // byte 6: DS18S20: COUNT_REMAIN
        //         DS18B20 & DS1822: store for crc
        // byte 7: DS18S20: COUNT_PER_C
        //         DS18B20 & DS1822: store for crc
        // byte 8: SCRATCHPAD_CRC
        for (uint8_t i = 0; i < 9; i++) {
            scratchPad[i] = oneWire_read(iface);
        }

        b = oneWire_reset(iface);
        result = (b == 0) ? true : false;
    }else{
        /*Do nothing as device is not present. Line in high level */
    }
    return result;
}

void DS18B20_writeScratchPad( const uint8_t* scratchPad, uint32_t iface, uint64_t add)
{
    oneWire_reset(iface);
    if( add == 0 ){
        oneWire_skip(iface);
    }else{
        oneWire_match(iface, add);
    }
    oneWire_write(WRITESCRATCH, iface);
    oneWire_write(scratchPad[HIGH_ALARM_TEMP], iface); // high alarm temp
    oneWire_write(scratchPad[LOW_ALARM_TEMP], iface); // low alarm temp

    // DS1820 and DS18S20 have no configuration register
    oneWire_write(scratchPad[CONFIGURATION], iface);

    oneWire_reset(iface);
}
// set resolution of a device to 9, 10, 11, or 12 bits
// if new resolution is out of range, 9 bits is used.
bool DS18B20_setResolution( uint8_t newResolution, bool skipGlobalBitResolutionCalculation)
{
#ifdef ENABLED_AAL
    bool success = false;

    // DS1820 and DS18S20 have no resolution configuration register
    success = true;
    // handle the sensors with configuration register
    newResolution = constrain(newResolution, 9, 12);
    uint8_t newValue = 0;
    // MAX31850 has no resolution configuration register
    // this is also a hack as the MAX31850 Coversion time is 100ms max.
    // use a low res (~10 by spec, but 9 might work) for faster blocking read times.
    if (scratchPad[CONFIGURATION] & 0x80 ) {
        success = true;
    } else {
        switch (newResolution) {
            case 12:
                newValue = TEMP_12_BIT;
                break;
            case 11:
                newValue = TEMP_11_BIT;
                break;
            case 10:
                newValue = TEMP_10_BIT;
                break;
            case 9:
            default:
                newValue = TEMP_9_BIT;
                break;
        }

        // if it needs to be updated we write the new value
        if (scratchPad[CONFIGURATION] != newValue) {
            scratchPad[CONFIGURATION] = newValue;
            writeScratchPad(scratchPad);
        }
        // done
        success = true;
    }

    // do we need to update the max resolution used?
    if (skipGlobalBitResolutionCalculation == false) {
        bitResolution = newResolution;
        if (devices > 1) {
            DeviceAddress deviceAddr;
            oneWire_reset_search();
            for (uint8_t i = 0; i < devices; i++) {
                if (bitResolution == 12) break;
                if (oneWire_search(deviceAddr) && validAddress(deviceAddr)) {
                    uint8_t b = getResolution(deviceAddr);
                    if (b > bitResolution) bitResolution = b;
                }
            }
        }
    }

    return success;
    #endif
}

// returns the current resolution of the device, 9-12
// returns 0 if device not found
uint8_t DS18B20_getResolution(void)
{
#ifdef ENABLED_AAL
    ScratchPad scratchPad;
    if (isConnected(scratchPad)) {

        // MAX31850 has no resolution configuration register
        if (scratchPad[CONFIGURATION] & 0x80)
            return 12;

        switch (scratchPad[CONFIGURATION]) {
            case TEMP_12_BIT:
                return 12;

            case TEMP_11_BIT:
                return 11;

            case TEMP_10_BIT:
                return 10;

            case TEMP_9_BIT:
                return 9;
        }
    }
    return 0;
#endif

}


bool DS18B20_isConversionComplete(uint32_t iface) {
    uint8_t b = oneWire_read_bit(iface);
    return (b == 1);
}

// sends command for all devices on the bus to perform a temperature conversion
void DS18B20_requestTemperatures(uint32_t iface, uint64_t add)
{
    oneWire_reset(iface);
    if( add == 0 ){
        oneWire_skip(iface);
    }else{
        oneWire_match(iface, add);
    }
    oneWire_write(STARTCONVO, iface);

    while(DS18B20_isConversionComplete(iface) == false){
        delay_ms(10);
    }
}


// Sends command to one or more devices to save values from scratchpad to EEPROM
// If optional argument deviceAddress is omitted the command is send to all devices
// Returns true if no errors were encountered, false indicates failure
bool DS18B20_saveScratchPad(uint32_t iface, uint64_t add)
{

    if (oneWire_reset(iface) == 0)
        return false;

    if( add == 0 ){
        oneWire_skip(iface);
    }else{
        oneWire_match(iface, add);
    }

    oneWire_write(COPYSCRATCH, iface);

    // Specification: NV Write Cycle Time is typically 2ms, max 10ms
    // Waiting 20ms to allow for sensors that take longer in practice
    delay_ms(20);

    return oneWire_reset(iface) == 1;

}

// Sends command to one or more devices to recall values from EEPROM to scratchpad
// If optional argument deviceAddress is omitted the command is send to all devices
// Returns true if no errors were encountered, false indicates failure
bool DS18B20_recallScratchPad(uint32_t iface, uint64_t add)
{
    if( add == 0 ){
        oneWire_skip(iface);
    }else{
    }

    oneWire_write(RECALLSCRATCH, iface);

    while (oneWire_read_bit(iface) == 0) {
    }

    return oneWire_reset(iface) == 1;

}

// reads scratchpad and returns fixed-point temperature, scaling factor 2^-7
int32_t DS18B20_calculateTemperature( uint8_t* scratchPad)
{
    uint32_t temperature = scratchPad[TEMP_MSB];
    temperature <<=8;
    temperature |= scratchPad[TEMP_LSB];

    temperature *=  5;
    temperature >>= 3;

    return temperature;
}

uint8_t  DS18B20_getTempC(uint32_t *temp)
{
    temp[0] = temp_01;
    for(uint8_t i = 0; i < iface1_ndevs; i++){
        temp[1 + i] = iface1_temps[i];
    }
    return iface1_ndevs;
}

void DS18B20_setUserData( int16_t data)
{
#ifdef ENABLE_AAL
    // return when stored value == new value
    if (getUserData() == data)
        return;

    ScratchPad scratchPad;
    if (isConnected(scratchPad)) {
        scratchPad[HIGH_ALARM_TEMP] = data >> 8;
        scratchPad[LOW_ALARM_TEMP] = data & 255;
        writeScratchPad(scratchPad);
    }
#endif
}
