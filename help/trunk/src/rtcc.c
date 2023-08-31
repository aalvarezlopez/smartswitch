/// rtcc.c
///
/// Copyright (C) 2013 INGEN10 Ingenieria SL
/// http://www.ingen10.com
///
/// LEGAL NOTICE:
/// All information contained herein is, and remains property of INGEN10 Ingenieria SL.
/// Dissemination of this information or reproduction of this material is strictly
/// forbidden unless prior written permission is obtained from its owner.
/// ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC PERFORMANCE, OR PUBLIC DISPLAY
/// OF, OR THROUGH USE OF THIS SOURCE CODE IS STRICTLY PROHIBITED, AND IT IS A VIOLATION
/// OF INTERNATIONAL TRADE TREATIES AND LAWS.
/// THE RECEIPT OR POSSESSION OF THIS DOCUMENT DOES NOT CONVEY OR IMPLY ANY RIGHTS.
///
/// Authored by:   Adrian (11/12/2013)
/// Revised by:    AAL (08/01/2014)
/// Last Version:  11/12/2013
///
/// FILE CONTENTS:
/// Control functions for MCP79410 (I2C RTCC+EEPROM)

#include "sam4lc4c.h"
#include "configuration.h"
#include "rtcc.h"
#include "mcu_peripheral.h"
#include "debug.h"
#include "i2c.h"
#include "status_machine.h"

/** \section RTCC
 /////////////////////////////////////////////////////////////////////////////
 *  Library for RTCC control MCP79410
 /////////////////////////////////////////////////////////////////////////////
 */

int ReadSequentialRegister(unsigned int start_address, unsigned int* value,
                           unsigned int n_reg);
int WriteRegister(unsigned int reg_address, unsigned int value);

/** \brief RTCC set date
 *
 *  \param day: unsigned int represent day (0 a 31)
 *         month: unsigned int represent month (0 a 12)
 *         year:  unsigned int represent year
 *
 *  \return 1 set successfuly
 *          0 error
 */
int SetDate(unsigned int day, unsigned int month, unsigned int year)
{
    //check for valid arguments
    if (day > VALIDDAY || month > VALIDMONTH) { return 0; }

    //disable oscillator before set hour
    WriteRegister(DATE, RTCC_DAY(day));
    WriteRegister(MONTH, RTCC_MONTH(month));
    WriteRegister(YEAR, RTCC_YEAR(year));
    return 1;
}

/** \brief RTCC read current date
 *
 *  \param day: pointer to a unsigned int represent day (0 a 31)
 *         month: pointer to a unsigned int represent month (0 a 12)
 *         year:  pointer to a unsigned int represent year
 *
 *  \return 1 read successfuly
 *          0 error
 */
int ReadDate(unsigned int* day, unsigned int* month, unsigned int* year)
{
    unsigned int read[MAXSEQWRITE];

    ReadSequentialRegister(DATE, read, 3);
    *day = ((read[0] >> 4) & 0x3) * 10 + (read[0] & 0xF);
    *month = ((read[1] >> 4) & 0x1) * 10 + (read[1] & 0xF);
    *year = ((read[2] >> 4) & 0xF) * 10 + (read[2] & 0xF);
    return 1;
}

/** \brief Check if RTCC is running
 *
 * \return  1 running
 *          0 stop
 */
int IsRTCCEnabled()
{
    int status;
    ReadRegister(SECONDS, &status);

    if ( (status & (1 << RTCC_SECONDS_ST_POS)) != 0) {
        return 1;
    } else {
        return 0;
    }
}

/** \brief RTCC set clock
 *
 *  \param seconds: unsigned int represent seconds (0 a 60)
 *         minutes: unsigned int represent minutes (0 a 60)
 *         hour:  unsigned int represent hour (0 a 24)
 *
 *  \return 1 set successfuly
 *          0 error
 */
int SetClock(unsigned int seconds, unsigned int minutes, unsigned int hour)
{
    //check for valid arguments
    if (seconds > VALIDSECONDS || minutes > VALIDMINUTES || hour > VALIDHOUR) { return 0; }

    //disable oscillator before set hour
    WriteRegister(SECONDS, RTCC_DISABLE_OSC);
    WriteRegister(CONTROL, RTCC_CONTROL_SQWE);
    WriteRegister(HOURS, RTCC_HOURS(hour));
    WriteRegister(MINUTES, RTCC_MINUTES(minutes));
    WriteRegister(SECONDS, RTCC_SECONDS(seconds) | RTCC_ENABLE_OSC);
    WriteRegister(DAY, VBATEN);
    SetAlarm((seconds + STANDBYTICKS) % 60, 0, 0, SECONDSMATCH | ALARM_POL_HIGH);
    return 1;
}

/** \brief RTCC read current time
 *
 *  \param seconds: pointer to unsigned int represent seconds (0 a 60)
 *         minutes: pointer to unsigned int represent minutes (0 a 60)
 *         hour:  pointer to unsigned int represent hour (0 a 24)
 *
 *  \return 1 set successfuly
 *          0 error
 */
int ReadClock(unsigned int* seconds, unsigned int* minutes, unsigned int* hour)
{
    unsigned int read[MAXSEQWRITE];

    ReadSequentialRegister(SECONDS, read, 3);
    *seconds = ((read[0] >> 4) & 0x7) * 10 + (read[0] & 0xF);
    *minutes = ((read[1] >> 4) & 0x7) * 10 + (read[1] & 0xF);
    *hour = ((read[2] >> 4) & 0x3) * 10 + (read[2] & 0xF);
    /*if(*seconds > VALIDSECONDS || *minutes > VALIDMINUTES ||
     *hour > VALIDHOUR)
     return 0;*/
    return 1;
}

/** \brief RTCC alarm configuration
 *
 *  \param seconds: pointer to unsigned int represent seconds (0 a 60)
 *         minutes: pointer to unsigned int represent minutes (0 a 60)
 *         hour:  pointer to unsigned int represent hour (0 a 24)
 *         settings: configure seconds/minutes/hour match
 *
 *  \return 1 set successfuly
 *          0 error
 */
int SetAlarm(unsigned int seconds, unsigned int minutes, unsigned int hour,
             unsigned int settings)
{
    //check for valid arguments
    if (seconds > VALIDSECONDS || minutes > VALIDMINUTES || hour > VALIDHOUR) { return 0; }

    //disable oscillator before set hour
    WriteRegister(ALARM0HOURS, RTCC_HOURS(hour));
    WriteRegister(ALARM0MINUTES, RTCC_MINUTES(minutes));
    WriteRegister(ALARM0SECONDS, RTCC_SECONDS(seconds) | RTCC_ENABLE_OSC);
    WriteRegister(ALARM0DAY, settings);
    int control_reg;
    ReadRegister(CONTROL, &control_reg);
    WriteRegister(CONTROL, RTCC_ENABLE_ALM0 | CTRL_OUT_HIGH);
    return 1;
}

/** \brief RTCC check alarm and clear if necessary
 *
 */
int CheckAlarm()
{
    int alm0day;
    ReadRegister(ALARM0DAY, &alm0day);
    int alm0if = alm0day & ALARMIFMASK;

    if ( alm0if != 0) {
        WriteRegister(ALARM0DAY, alm0day & ~(ALARM0DAY));
        return 1;
    } else {
        return 0;
    }
}

/** \brief RTCC write register
 *
 *  \param regAddress: address of the register to write
 *          value: value to write in the register
 *
 *  \return 1 set successfuly
 *          0 error
 */
int WriteRegister(unsigned int reg_address, unsigned int value)
{
    unsigned int bytes_to_send[MAXSEQWRITE];
    bytes_to_send[0] = reg_address;
    bytes_to_send[1] = value;
    return I2CWriteData(RTCC_CNTRLADD, bytes_to_send, 2);
}

/** \brief RTCC write register
 *
 *  \param startAddress: address of the first register to write
 *          value: array of value to write in the register
 *
 *  \return 1 set successfuly
 *          0 error
 */
int WriteSequentialRegister(unsigned int start_address, unsigned int* value,
                            unsigned int n_reg)
{
    unsigned int bytes_to_send[MAXSEQWRITE];
    int i;

    if (n_reg > (MAXSEQWRITE - 1)) { return 0; }

    bytes_to_send[0] = start_address;

    for (i = 0; i < n_reg; i++) {
        bytes_to_send[i + 1] = value[i];
    }

    return I2CWriteData(RTCC_CNTRLADD, bytes_to_send, n_reg + 1);
}

/** \brief RTCC read register
 *
 *  \param regAddress: address of the register to write
 *          value: pointer return value
 *
 *  \return 1 set successfuly
 *          0 error
 */
int ReadRegister(unsigned int reg_address, unsigned int* value)
{
    unsigned int bytes_to_send[MAXSEQWRITE];
    bytes_to_send[0] = reg_address;
    return I2CWriteReadData(RTCC_CNTRLADD, bytes_to_send, 1, value, 1);
}

/** \brief RTCC write register
 *
 *  \param startAddress: address of the first register to write
 *          value: array of value to write in the register
 *
 *  \return 1 set successfuly
 *          0 error
 */
int ReadSequentialRegister(unsigned int start_address, unsigned int* value,
                           unsigned int n_reg)
{
    unsigned int bytes_to_send[MAXSEQWRITE];

    if (n_reg > (MAXSEQWRITE - 1)) { return 0; }

    bytes_to_send[0] = start_address;
    return I2CWriteReadData(RTCC_CNTRLADD, bytes_to_send, 1, value, n_reg);
}

/** \section TimeCount
 /////////////////////////////////////////////////////////////////////////////
 * Routines to count elapsed seconds (using in status machine)
 /////////////////////////////////////////////////////////////////////////////
 */

//TODO: provisional way of measuring elapsed time. Must find a better way.

unsigned int st_hour = 0, st_min = 0, st_sec = 0;
unsigned int st_year = 10, st_month = 1, st_day = 1;

void TimeCountReset()
{
    ReadClock(&st_sec, &st_min, &st_hour);
    ReadDate(&st_day, &st_month, &st_year);
    DebugHourWrite(st_sec, st_min, st_hour);
}

unsigned long ReadTimeCountElapsed()
{
    /** \REVIEW: AAL
     * unused variable el_month, should ReadData
     * use el_month instead of st_month?
     */
    unsigned int el_hour = 0, el_min = 0, el_sec = 0;
    unsigned int el_year = 10, el_month = 1, el_day = 1;

    unsigned long st_timestamp = 0;
    unsigned long el_timestamp = 0;

    ReadClock(&el_sec, &el_min, &el_hour);
    ReadDate(&el_day, &el_month, &el_year);

    st_timestamp = st_hour;
    st_timestamp *= 60;
    st_timestamp += st_min;
    st_timestamp *= 60;
    st_timestamp += st_sec;

    if (st_day != el_day) { el_timestamp += 24; }          //up to 24 hours of difference, switching date

    el_timestamp += el_hour;
    el_timestamp *= 60;
    el_timestamp += el_min;
    el_timestamp *= 60;
    el_timestamp += el_sec;

    return (el_timestamp - st_timestamp);
}

/** \section EEPROM
 /////////////////////////////////////////////////////////////////////////////
 * Routines to use internal general EEPROM memory, to store device parameters
 /////////////////////////////////////////////////////////////////////////////
 */


//TODO:Configuration are dummy struct. Use the real one.



/** \brief Write Configuration in EEPROM memory
 *
 *  \param  config: struct to be save
 *          address: memory address where the struct will be saved
 *
 *  \return 1 set successfuly
 *          0 error
 */
int WriteConfig(const Params* params, unsigned int address)
{
    //split struct in page size and write
    char* pointer;
    unsigned int page_block[RTCC_PAGESIZE];
    unsigned int struct_size;
    int i, n_blocks;

    if (address % RTCC_PAGESIZE != 0) { return 0; }

    n_blocks = 0;
    pointer = (char*) params;
    //get struct size
    struct_size = sizeof(*params);

    for (i = 0; i < struct_size; i++) {
        page_block[i % RTCC_PAGESIZE] = *pointer;
        pointer++;

        if ((i % RTCC_PAGESIZE) >= (RTCC_PAGESIZE - 1)) {
            //write block
            WriteBlock(page_block, address + n_blocks * RTCC_PAGESIZE,
                       RTCC_PAGESIZE);

            while (!CheckEEPROM())
                ;

            n_blocks++;
        }
    }

    //write last bytes
    WriteBlock(page_block, address + n_blocks * RTCC_PAGESIZE,
               i % RTCC_PAGESIZE);

    while (!CheckEEPROM())
        ;

    return 1;
}

/** \brief Read Configuration from EEPROM memory
 *
 *  \param  config: pointer to the struct where the info will be saved
 *          address: memory address where the struct will be read
 *                  address should be start page address
 *
 *  \return 1 set successfuly
 *          0 error
 */
int ReadConfig(Params* pointer_params, unsigned int address)
{
    char* pointer;
    unsigned int read_page[RTCC_PAGESIZE];
    unsigned int struct_size;
    int i, j, n_blocks;

    if (address % RTCC_PAGESIZE != 0) { return 0; }

    pointer = (char*) pointer_params;
    //get struct size
    struct_size = sizeof(*pointer_params);
    n_blocks = struct_size / RTCC_PAGESIZE;

    for (i = 0; i < n_blocks; i++) {
        ReadBlock(read_page, address + i * RTCC_PAGESIZE, RTCC_PAGESIZE);

        //copy to the structure
        for (j = 0; j < RTCC_PAGESIZE; j++) {
            *pointer = read_page[j];
            pointer++;
        }
    }

    //read las block
    ReadBlock(read_page, address + n_blocks * RTCC_PAGESIZE, RTCC_PAGESIZE);

    for (i = 0; i < struct_size % RTCC_PAGESIZE; i++) {
        *pointer = read_page[i];
        pointer++;
    }

    return 1;
}

/** \brief Read Calibration from EEPROM memory
 *
 *  \param  config: pointer to the struct where the info will be saved
 *          address: memory address where the struct will be read
 *              address should be start page address
 *
 *  \return 1 set successfuly
 *          0 error
 */
int ReadCalibration(const struct Calibration* calib, unsigned int address)
{
    char* pointer;
    unsigned int read_page[RTCC_PAGESIZE];
    unsigned int struct_size;
    int i, j, n_blocks;

    if (address % RTCC_PAGESIZE != 0) { return 0; }

    pointer = (char*)calib;
    //get struct size
    struct_size = sizeof(*calib);
    n_blocks = struct_size / RTCC_PAGESIZE;

    for (i = 0; i < n_blocks; i++) {
        ReadBlock(read_page, address + i * RTCC_PAGESIZE, RTCC_PAGESIZE);

        //copy to the structure
        for (j = 0; j < RTCC_PAGESIZE; j++) {
            *pointer = read_page[j];
            pointer++;
        }
    }

    //read las block
    ReadBlock(read_page, address + n_blocks * RTCC_PAGESIZE, RTCC_PAGESIZE);

    for (i = 0; i < struct_size % RTCC_PAGESIZE; i++) {
        *pointer = read_page[i];
        pointer++;
    }

    return 1;
}

/** \brief Write calibration in EEPROM memory
 *
 *  \param  config: struct to be save
 *          address: memory address where the struct will be saved
 *              address should be start page address
 *
 *  \return 1 set successfuly
 *          0 error
 */
int WriteCalibration(const struct Calibration* calib, unsigned int address)
{
    //split struct in page size and write
    char* pointer;
    unsigned int page_block[RTCC_PAGESIZE];
    unsigned int struct_size;
    int i, n_blocks;

    if (address % RTCC_PAGESIZE != 0) { return 0; }

    n_blocks = 0;
    pointer = (char*)calib;
    //get struct size
    struct_size = sizeof(*calib);

    for (i = 0; i < struct_size; i++) {
        page_block[i % RTCC_PAGESIZE] = *pointer;
        pointer++;

        if ((i % RTCC_PAGESIZE) >= (RTCC_PAGESIZE - 1)) {
            //write block
            WriteBlock(page_block, address + n_blocks * RTCC_PAGESIZE,
                       RTCC_PAGESIZE);

            while (!CheckEEPROM()) { continue; }

            n_blocks++;
        }
    }

    //write last bytes
    WriteBlock(page_block, address + n_blocks * RTCC_PAGESIZE,
               i % RTCC_PAGESIZE);

    while (!CheckEEPROM())
        ;

    return 1;
}

/** \brief Write nBytes number of bytes in memory. This function
 * is limited to write only one page of eeprom. If params function
 * attempts to write across a physical page boundary, function return
 * 0
 *
 *  \param  buffer: pointer to array of values to be written
 *          startAddress:
 *          nBytes: number of bytes to be written
 *  \return 1 set successfuly
 *          0 error
 */
int WriteBlock(const unsigned int* buffer, unsigned int start_address,
               unsigned int n_bytes)
{
    unsigned int bytes_to_send[MAXBLOCKSIZE];
    int i;

    //verification write into same page
    if ((start_address % RTCC_PAGESIZE) + n_bytes > RTCC_PAGESIZE) { return 0; }

    if (n_bytes > (MAXBLOCKSIZE - 1)) { return 0; }

    bytes_to_send[0] = start_address;

    for (i = 0; i < n_bytes; i++) {
        bytes_to_send[i + 1] = buffer[i];
    }

    return I2CWriteData(RTCC_EEPROMADD, bytes_to_send, n_bytes + 1);
}

/** \brief Read nBytes number of bytes in memory
 *
 *  \param  buffer: pointer to array where values will be saved
 *          startAddress:
 *          nBytes: number of bytes to be read
 *  \return 1 set successfuly
 *          0 error
 */
int ReadBlock(unsigned int* buffer, unsigned int start_address,
              unsigned int n_bytes)
{
    unsigned int bytes_to_send[MAXBLOCKSIZE];

    if (n_bytes > (MAXBLOCKSIZE - 1)) { return 0; }

    bytes_to_send[0] = start_address;
    return I2CWriteReadData(RTCC_EEPROMADD, bytes_to_send, 1, buffer, n_bytes);
}

/** \brief Write byte in eeprom memory
 *
 *  \param  value: value to be written
 *          address:
 *  \return 1 set successfuly
 *          0 error
 */
int WriteByte(unsigned int value, unsigned int address)
{
    unsigned int bytes_to_send[2];
    bytes_to_send[0] = address;
    bytes_to_send[1] = value;
    return I2CWriteData(RTCC_EEPROMADD, bytes_to_send, 2);
}

/** \brief Read byte from eeprom memory
 *
 *  \param  value: pointer to array where values will be saved
 *          address:
 *  \return 1 set successfuly
 *          0 error
 */
int ReadByte(unsigned int* value, unsigned int address)
{
    unsigned int bytes_to_send[1];
    bytes_to_send[0] = address;
    return I2CWriteReadData(RTCC_EEPROMADD, bytes_to_send, 1, value, 1);
}

/** \brief Check internal write cycle has finished
 *
 *  \return 1: new writte operation available
 *          0: device busy
 */
int CheckEEPROM()
{
    // REVIEW: AAL implicit declaration of CheckDevice
    return CheckDevice(RTCC_EEPROMADD);
}
