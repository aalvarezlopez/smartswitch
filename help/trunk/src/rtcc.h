/*
 * rtcc.h
 *
 *  Created on: 11/12/2013
 *      Author: Adrian
 */

#ifndef RTCC_H_
#define RTCC_H_

#include "i2c.h"
#define RTCC_CNTRLADD   0x6F
#define RTCC_EEPROMADD  0x57

enum CTRLREG {
    SECONDS = 0,
    MINUTES,
    HOURS,
    DAY,
    DATE,
    MONTH,
    YEAR,
    CONTROL,
    CALIBRATION,
    UNLOCKID,
    ALARM0SECONDS,
    ALARM0MINUTES,
    ALARM0HOURS,
    ALARM0DAY,
    ALARM0DATE,
    ALARM0MONTH,
    RESERVERD0,
    ALARM1SECONDS,
    ALARM1MINUTES,
    ALARM1HOURS,
    ALARM1DAY,
    ALARM1DATE,
    ALARM1MONTH,
    RESERVERD1,
    POWERDOWNMINUTES,
    POWERDOWNHOURS,
    POWERDOWNDATE,
    POWERDOWNDAY,
    POWERUPMINUTES,
    POWERUPHOURS,
    POWERUPDATE,
    POWERUPDAY
};

#define VALIDMINUTES    59
#define VALIDSECONDS    59
#define VALIDHOUR       23
#define VALIDDAY        31
#define VALIDMONTH      12

#define BYTEMASK    0xFF
#define MAXSEQWRITE 10
#define MAXBLOCKSIZE    200

#define RTCC_SECONDS_ST_POS         7
#define RTCC_DISABLE_OSC            (0x0u << RTCC_SECONDS_ST_POS)
#define RTCC_ENABLE_OSC             (0x1u << RTCC_SECONDS_ST_POS)
#define RTCC_CONTROL_ALM0_POS       4
#define RTCC_ENABLE_ALM0            (0x1u << RTCC_CONTROL_ALM0_POS)
#define RTCC_SECONDS(value)         ((value / 10) << 4) | (value % 10)
#define RTCC_MINUTES(value)         ((value / 10) << 4) | (value % 10)
#define RTCC_HOURS(value)           ((value / 10) << 4) | (value % 10)
#define RTCC_DAY(value)             ((value / 10) << 4) | (value % 10)
#define RTCC_MONTH(value)           ((value / 10) << 4) | (value % 10)
#define RTCC_YEAR(value)            ((value / 10) << 4) | (value % 10)
#define RTCC_CONTROL_EXTOSC_POS     3
#define RTCC_CONTROL_EXTOSC         (0x1u << RTCC_CONTROL_EXTOSC_POS)
#define RTCC_CONTROL_SQWE_POS       6
#define RTCC_CONTROL_SQWE           (0x1u << RTCC_CONTROL_SQWE_POS)
#define RTCC_PAGESIZE               8

#define SECONDSMATCH        0
#define MINUTESMATCH        1
#define HOURSMATCH          2
#define ALARM_POL_HIGH      (1 << 7)
#define ALARM_POL_LOW       0
#define CTRL_OUT_HIGH      (1 << 7)
#define CTRL_OUT_LOW       0

#define VBATBITMASK         3
#define VBATEN              (1 << VBATBITMASK)

#define ALARMIFMASK     (1 << 3)

int ReadClock(unsigned int* seconds, unsigned int* minutes, unsigned int* hour);
int SetClock(unsigned int seconds, unsigned int minutes, unsigned int hour);
int ReadDate(unsigned int* day, unsigned int* month, unsigned int* year);
int SetDate(unsigned int day, unsigned int month, unsigned int year);
int CheckAlarm();
int SetAlarm(unsigned int seconds, unsigned int minutes, unsigned int hour,
             unsigned int settings);
void TimeCountReset();
unsigned long ReadTimeCountElapsed();

#include "params.h"

struct Calibration {
    //ToDo fill calibration struct
    unsigned int temp;
};

int WriteConfig(const Params* params, unsigned int address);
int ReadConfig(Params* pointer_params, unsigned int address);
//ToDo:remove
int WriteBlock(const unsigned int* buffer, unsigned int startAddress,
               unsigned int nBytes);
int ReadBlock(unsigned int* buffer, unsigned int startAddress,
              unsigned int nBytes);
int CheckEEPROM();

#endif /* RTCC_H_ */
