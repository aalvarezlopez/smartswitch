/// string_utils.c
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
/// Authored by:   JRB (05/12/2013)
/// Revised by:     AAL (17/12/2013)
/// Last Version:  09/12/2013
///
/// FILE CONTENTS:
/// Functions to convert between strings and integer numbers (HEX and decimal cases)

#include "string_utils.h"

/** \brief
 *  Extracts a signed integer from a decimal string (with sign)
 *  \param
 *  pointer to a string finishing in \0
 *  \return
 *  A signed long with the value of the number represented in the string
 */
signed long DecStrtoSlong(char* buf)
{
    signed long value = 0;
    int aux, sign;
    char b;
    int i;
    char* buffer;

    if (buf[0] == '-') {
        sign = -1;
        buffer = buf + 1;
    } else {
        sign = 1;
        buffer = buf;
    }

    for (i = 0; i < 6; i++) {
        b = buffer[i];

        if (b <= '9' && b >= '0') {
            aux = b - '0';
            value *= 10;
            value += aux;
        } else {
            break;
        }
    }

    value *= sign;

    return value;
}

/** \brief
 *  Extracts an unsigned integer from a decimal string
 *  \param
 *  pointer to a string finishing in \0
 *  \return
 *  An unsigned int with the value of the number represented in the string
 */
unsigned DecStrtoUnsigned(char* buf)
{
    unsigned value = 0;
    int aux;
    char b;
    int i;
    char* buffer;

    buffer = buf;

    for (i = 0; i < 6; i++) {
        b = buffer[i];

        if (b <= '9' && b >= '0') {
            aux = b - '0';
            value *= 10;
            value += aux;
        } else {
            break;
        }
    }

    return value;
}

/** \brief
 *  Extracts a 16bit integer from an hexadecimal string
 *  \param
 *  pointer to a string finishing in \0
 *  \return
 *  A 16bit integer containing the value (hexadecimal) of the string
 */
int16_t HexStrToInt16(char* str)
{
    int16_t value = 0, aux;
    char b;
    int i;

    for (i = 0; i < 4; i++) {
        b = str[i];

        if (b <= '9' && b >= '0') { aux = b - '0'; }
        else if (b <= 'F' && b >= 'A') { aux = 0xA + b - 'A'; }
        else { aux = 0; }

        aux = aux << 4 * (3 - i);
        value += aux;
    }

    return value;
}

/** \brief
 *  Extracts a 8bit integer from an hexadecimal string
 *  \param
 *  pointer to a string finishing in \0
 *  \return
 *  A 8bit integer containing the value (hexadecimal) of the string
 */
int8_t HexStrToInt8(char* str)
{
    int8_t value = 0, aux;
    char b;
    int i;

    for (i = 0; i < 2; i++) {
        b = str[i];

        if (b <= '9' && b >= '0') { aux = b - '0'; }
        else if (b <= 'F' && b >= 'A') { aux = 0xA + b - 'A'; }
        else { aux = 0; }

        aux = aux << 4 * (1 - i);
        value += aux;
    }

    return value;
}

/** \brief
 *  Formats a 16bit integer in hexadecimal into a string (finishing with a '0')
 *  \param
 *  value: the number to write (must be 16 bit)
 *  str: a pointer to a string (it will add a \0 as end character)
 */
void Int16ToHexString(int16_t value, char* str)
{
    int hex;
    int i;
    uint16_t temp = (uint16_t) value;

    for (i = 0; i < 4; i++) {
        hex = temp % 16;

        if (hex <= 9) { *(str + 3 - i) = '0' + hex; }
        else { *(str + 3 - i) = 'A' + hex - 10; }

        temp /= 16;
    }

    str[4] = 0;

}

/** \brief
 *  Formats a 8bit integer in hexadecimal into a string (finishing with a '0')
 *  \param
 *  value: the number to write (must be 8 bit)
 *  str: a pointer to a string (it will add a \0 as end character)
 */
void ByteToHexString(char value, char* str)
{
    int hex;
    int i;
    char temp = value;

    for (i = 0; i < 2; i++) {
        hex = temp & 0x0F;

        if (hex <= 9) { *(str + 1 - i) = '0' + hex; }
        else { *(str + 1 - i) = 'A' + hex - 0xA; }

        temp >>= 4;
    }

    str[2] = 0;
}

/** \brief
 *  Writes a UNSIGNED integer (any size) into a string (finishing with a '0')
 *  Decimal format
 *  \param
 *  value: the number to write
 *  str: a pointer to a string (it will add a \0 as end character)
 */
void UIntToDecString(uint32_t value, char* str)
{
    int n = 0, i;
    int remainder;
    uint32_t temp = value;

    str[0] = 0;

    do {
        n++;

        for (i = n; i > 0; i--) {
            str[i] = str[i - 1];
        }

        remainder = temp % 10;
        str[0] = '0' + remainder;
        temp /= 10;

        if (n > 10) { break; }
    } while (temp > 0);

    str[n] = 0;
}

/** \brief
 *  Writes a SIGNED integer (any size) into a string (finishing with a '0')
 *  Decimal format
 *  \param
 *  value: the number to write
 *  str: a pointer to a string (it will add a \0 as end character)
 */
void SIntToDecString(int32_t value, char* str)
{
    int n = 0, i;
    int remainder;
    int32_t temp = value;

    if (temp < 0) { temp = -temp; }

    str[0] = 0;

    do {
        n++;

        for (i = n; i > 0; i--) {
            str[i] = str[i - 1];
        }

        remainder = temp % 10;
        str[0] = '0' + remainder;
        temp /= 10;

        if (n > 10) { break; }
    } while (temp > 0);

    str[n] = 0;

    if (value < 0) {
        n++;

        for (i = n; i > 0; i--) {
            str[i] = str[i - 1];
        }

        str[0] = '-';
    }

}

/** \brief This function writes time (hour) in pretty format
 *
 *  \param seconds
 *         minutes
 *         hour
 */
void TimeWrite(int seconds, int minutes, int hour, char* str)
{
    str[0] = hour / 10 + '0';
    str[1] = hour % 10 + '0';
    str[2] = ':';
    str[3] = minutes / 10 + '0';
    str[4] = minutes % 10 + '0';
    str[5] = ':';
    str[6] = seconds / 10 + '0';
    str[7] = seconds % 10 + '0';
    str[8] = 0;
}

/** \brief This function writes a date in pretty format
 *
 *  \param day
 *         month
 *         year
 */
void DateWrite(int day, int month, int year, char* str)
{
    str[0] = day / 10 + '0';
    str[1] = day % 10 + '0';
    str[2] = '-';
    str[3] = month / 10 + '0';
    str[4] = month % 10 + '0';
    str[5] = '-';
    str[6] = year / 10 + '0';
    str[7] = year % 10 + '0';
    str[8] = 0;
}

/** \brief This function writes a timestamp in pretty format
 *  for file name generation
 */
void TimestampWrite(int year, int month, int day, int hour, int minutes,
                    int seconds, char* str)
{
    str[0] = year / 10 + '0';
    str[1] = year % 10 + '0';
    str[2] = month / 10 + '0';
    str[3] = month % 10 + '0';
    str[4] = day / 10 + '0';
    str[5] = day % 10 + '0';
    str[6] = hour / 10 + '0';
    str[7] = hour % 10 + '0';
    str[8] = minutes / 10 + '0';
    str[9] = minutes % 10 + '0';
    str[10] = seconds / 10 + '0';
    str[11] = seconds % 10 + '0';
    str[12] = 0;
}
