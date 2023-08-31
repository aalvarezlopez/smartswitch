/*
 * string_utils.h
 *
 *  Created on: 06/12/2013
 *      Author: JRB
 */

#ifndef STRING_UTILS_H_
#define STRING_UTILS_H_

#include <stdint.h>

signed long DecStrtoSlong(char* buf);
int16_t HexStrToInt16(char* str);

void ByteToHexString(char value, char* str);
void Int16ToHexString(int16_t value, char* str);
int8_t HexStrToInt8(char* str);
void UIntToDecString(uint32_t value, char* str);
void SIntToDecString(int32_t value, char* str);
unsigned DecStrtoUnsigned(char* buf);

void TimeWrite(int seconds, int minutes, int hour, char* str);
void DateWrite(int day, int month, int year, char* str);
void TimestampWrite(int year, int month, int day, int hour, int minutes,
                    int seconds, char* str);

#endif /* STRING_UTILS_H_ */
