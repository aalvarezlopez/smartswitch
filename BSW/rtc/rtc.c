/**
 * @file rtc.c
 * @brief Real time clock driver for SAM4S
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-10-18
 */
#include "stdint.h"
#include "sam4s4a.h"
#include "component_rtc.h"


/**
 * @brief Get time from the RTC peripheral
 *
 * @param hour Current hour in BCD
 * @param min Current minutes in BCD
 * @param sec Current minutes in BCD
 */
void Rtc_getTimeHour(uint8_t *hour, uint8_t *min, uint8_t *sec)
{
    uint32_t rtc = RTC->RTC_TIMR;
    *hour = (rtc & RTC_TIMR_HOUR_Msk) >> RTC_TIMR_HOUR_Pos;
    *min = (rtc & RTC_TIMR_MIN_Msk) >> RTC_TIMR_MIN_Pos;
    *sec = (rtc & RTC_TIMR_SEC_Msk) >> RTC_TIMR_SEC_Pos;
}


/**
 * @brief Get current date from the RTC peripheral
 *
 * @param year Current year in BCD. RANGE 00-99
 * @param month Current year in BCD. RANGE 01-12
 * @param day Current year in BCD. RANGE 01-31
 */
void Rtc_getTimeDate(uint8_t *year, uint8_t *month, uint8_t *day)
{
    uint32_t rtc = RTC->RTC_CALR;
    *year = (rtc & RTC_CALR_YEAR_Msk) >> RTC_CALR_YEAR_Pos;
    *month = (rtc & RTC_CALR_MONTH_Msk) >> RTC_CALR_MONTH_Pos;
    *day = (rtc & RTC_CALR_DATE_Msk) >> RTC_CALR_DATE_Pos;
}


/**
 * @brief Update RTC peripheral with given date and time.
 *
 * When stopping the timer, both the date and the time are stopped.
 *
 * @param year Year in decimal format. Range 0-99
 * @param month. Month in decimal format. Range 1-12
 * @param day. Day in decimal format. Range 1-31
 * @param hour. Hour in decimal format. Range 0-24
 * @param min. Minutes in decimal format. Range 0-59
 * @param sec. Seconds in decimal format. Range 0-59
 */
void Rtc_setTimeDate(uint8_t year, uint8_t month, uint8_t day,
    uint8_t hour, uint8_t min, uint8_t sec)
{
    RTC->RTC_CR = RTC->RTC_CR | RTC_CR_UPDTIM | RTC_CR_UPDCAL;

    while( (RTC->RTC_SR & RTC_SR_ACKUPD) != (RTC_SR_ACKUPD_UPDATE)){ continue; }

    RTC->RTC_SCCR = RTC_SCCR_ACKCLR;

    RTC->RTC_TIMR = RTC_TIMR_MIN(0x12) |
        RTC_TIMR_HOUR( 0x08 ) |
        RTC_TIMR_SEC( 0x53);
    RTC->RTC_CALR =  RTC_CALR_YEAR(0x22) |
        RTC_CALR_MONTH(0x10) |
        RTC_CALR_DATE(0x19) |
        RTC_CALR_DAY(0x1) |
        RTC_CALR_CENT(0x20);

    RTC->RTC_CR = RTC->RTC_CR & ~(RTC_CR_UPDTIM | RTC_CR_UPDCAL);
}
