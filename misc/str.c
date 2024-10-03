/**
 * @file str.c
 * @brief Some miscellaneus functions
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-31
 */

#include "stdint.h"
#include "stdio.h"
#include "str.h"
#include <stdlib.h>
#include <stdbool.h>

#define DIGIT_MAX_LENGTH 10
int isdigit(int);


/**
 * @brief Convert unsigned integer to string
 *
 * @param n Unsigned integer to convert
 * @param s Output string
 */
uint16_t __atoi( char *s)
{
    uint16_t result = 0;
    while( *s != 0){
        if ( *s >= '0' && *s <= '9' ){
            result *= 10;
            result += *s - '0';
        }else{
            break;
        }
        s++;
    }
    return result;
}


/**
 * @brief Set byte to byte the memory starting from the pointer to the defined value
 *
 * @param dst Pointer to the memory to be set
 * @param value Value to be set
 * @param len Number of bytes to set
 *
 * @return  Pointer to the last element set
 */
void* memset(void *dst, int value, size_t len)
{
    uint16_t i;
    for( i = 0; i < len; i++){
        *((char*)dst+i) = value;
    }
    return (dst + i);
}


void* memcpy(void *dst, const void *src, uint16_t len)
{
    uint16_t i;
    for( i = 0 ; i < len; i++){
        *((char*)dst + i) = *((char*)src + i);
    }
    return (dst + i);
}

char* strcat(char *dst, const char *src)
{
    const char *pointerSrc = src;
    char *pointerDst = dst;
    while( *(pointerDst) != 0){
        pointerDst++;
    }
    while( *(pointerSrc) != 0){
        *(pointerDst) = *pointerSrc;
        pointerSrc++;
        pointerDst++;
    }
    *(pointerDst) = 0;
    return dst;
}

char* strcpy(char *dst, const char *src)
{
    const char *pointer = src;
    while( *(pointer) != 0){
        *(dst + (pointer - src)) = *(src + (pointer - src));
        pointer++;
    }
    *(dst + (pointer - src)) = 0;
    return (char*)pointer;
}
char* strncpy(char *dst, const char *src, uint16_t n)
{
    char *pointer = src;
    uint8_t len = 0;
    while( *(pointer) != 0){
        *(dst + (pointer - src)) = *(src + (pointer - src));
        pointer++;
        len++;
        if( len >= n ){
            break;
        }
    }
    *(dst + (pointer - src)) = 0;
    return pointer;
}

size_t strlen(const char *str)
{
    const char *pointer = str;
    while( *(pointer) != 0){
        pointer++;
    }
    return (pointer - str);
}

char* strstr(const char *str1, const char *str2)
{
    const char *init = 0;
    const char *pointer = str1;
    while( *(pointer) != 0){
        if( init == 0 ){
            if((*(pointer) == *(str2) )){
                init = pointer;
            }
        }else if( (*(str2 + (pointer - init)) == 0)){
            break;
        }else if( (*(pointer) != *(str2 + (pointer - init)))){
            init = 0;
            continue;
        }
        pointer++;
    }
    if( (pointer - init) < strlen(str2)){
        init = 0;
    }
    return (char*)init;
}
bool isDigit( unsigned char c)
{
    bool result = false;
    if ( c >= '0' && c <= '9' ){ result = true;}
    return result;
}

long atol(const char *num)
{
    long value = 0;
    int neg = 0;
    // decimal
    if (num[0] == '-') {
        neg = 1;
        num++;
    }
    while (*num && isDigit(*num)){
        value = value * 10 + *num++  - '0';
    }
    value = neg ? -value : value;
    return value;
}

void int_to_str(char * dst, uint16_t number, uint8_t leading)
{
    uint8_t count = 0;
    uint8_t digit[6];
    uint8_t offset;
    while( number > 0 ){
        digit[count] = '0' + number % 10;
        number /= 10;
        count++;
    }
    if( leading > count ){
        for( uint8_t i = 0; i < (leading - count); i++){
            *(dst + i) = '0';
        }
        offset = leading - count;
    }else{
        offset = 0;
    }
    for(uint8_t i = 0; i < count; i++){
        *(dst + i + offset) = digit[count - i - 1];
    }
}
