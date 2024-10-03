/**
 * @file errorlog.c
 * @brief Errors can be reported through this module. Reactions can be defined for each error type
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-07-02
 */

#include "stdint.h"
#include "errorlog.h"
#include "rtc.h"

uint8_t errorCounter = 0;
uint8_t debugCounter = 0;

log_st errors[MAX_HISTORY_LEN];
log_st debuglog[MAX_HISTORY_LEN];

void errorlog_Init(void)
{
    memset(errors, 0, sizeof(errors));
    errorCounter = 0;
    debugCounter = 0;
}

void errorlog_reportError(MODULES_t module, uint32_t code, uint32_t info)
{
    uint8_t hour, min, sec;
    uint8_t year, month, day;
    Rtc_getTimeDate(&year, &month, &day);
    Rtc_getTimeHour(&hour, &min, &sec);
    errors[ errorCounter % MAX_HISTORY_LEN].module = module;
    errors[ errorCounter % MAX_HISTORY_LEN].code = code;
    errors[ errorCounter % MAX_HISTORY_LEN].info = info;
    errors[ errorCounter % MAX_HISTORY_LEN].hour = hour;
    errors[ errorCounter % MAX_HISTORY_LEN].minute = min;
    errors[ errorCounter % MAX_HISTORY_LEN].sec = sec;
    errors[ errorCounter % MAX_HISTORY_LEN].day = day;
    errorCounter++;
}

void errorlog_reportDebug(MODULES_t module, uint32_t code, uint32_t info)
{
    uint8_t hour, min, sec;
    uint8_t year, month, day;
    Rtc_getTimeDate(&year, &month, &day);
    Rtc_getTimeHour(&hour, &min, &sec);
    debuglog[ debugCounter % MAX_HISTORY_LEN].module = module;
    debuglog[ debugCounter % MAX_HISTORY_LEN].code = code;
    debuglog[ debugCounter % MAX_HISTORY_LEN].info = info;
    debuglog[ debugCounter % MAX_HISTORY_LEN].hour = hour;
    debuglog[ debugCounter % MAX_HISTORY_LEN].minute = min;
    debuglog[ debugCounter % MAX_HISTORY_LEN].sec = sec;
    debuglog[ debugCounter % MAX_HISTORY_LEN].day = day;
    debugCounter++;
}

uint16_t errorlog_readErrors(uint32_t * add)
{
    *add = (uint32_t)(&errors[0]);
    return (errorCounter * sizeof(log_st));
}

uint16_t errorlog_readDebug(uint32_t * add)
{
    *add = (uint32_t)(&debuglog[0]);
    return (debugCounter * sizeof(log_st));
}
