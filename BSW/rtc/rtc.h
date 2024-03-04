/**
 * @file rtc.h
 * @brief Public RTC header file
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-10-19
 */

#ifndef RTC_H_
#define RTC_H_

#define INT_TO_BCD(x, y) do{y = x / 10; y<<= 4; y = y | (x%10);}while(0)
#define BCD_TO_INT(x, y) do{y = x & 0xF; y = y + (( (x >> 4) & 0xF) * 10);}while(0)


void Rtc_setTimeDate(uint8_t year, uint8_t month, uint8_t day, uint8_t hour,
	uint8_t min, uint8_t sec);
void Rtc_getTimeDate(uint8_t *year, uint8_t *month, uint8_t *day);
void Rtc_getTimeHour(uint8_t *hour, uint8_t *min, uint8_t *sec);

#endif
