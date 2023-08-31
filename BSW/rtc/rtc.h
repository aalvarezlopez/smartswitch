/**
 * @file rtc.h
 * @brief Public RTC header file
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-10-19
 */

#ifndef RTC_H_
#define RTC_H_

void Rtc_setTimeDate(uint8_t year, uint8_t month, uint8_t day, uint8_t hour,
	uint8_t min, uint8_t sec);
void Rtc_getTimeDate(uint8_t *year, uint8_t *month, uint8_t *day);
void Rtc_getTimeHour(uint8_t *hour, uint8_t *min, uint8_t *sec);

#endif
