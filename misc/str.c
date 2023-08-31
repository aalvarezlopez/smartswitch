/**
 * @file str.c
 * @brief Some miscellaneus functions
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-31
 */

#include "stdint.h"
#include "str.h"
#include "string.h"
#include <stdlib.h>
#include <stdbool.h>

#define DIGIT_MAX_LENGTH 10

void __atoi(uint16_t n, char *s)
{
    uint16_t index = 0;
    char tempStr[DIGIT_MAX_LENGTH];
    for(uint16_t i = 0; i < STR_N_DIGITS; i++){
        s[i] = 0;
    }
    if(n == 0){
        s[0] = '0';
        s[1] = 0;
    }else{
        tempStr[0] = '0';
        while(n > 0){
            tempStr[index] = '0' + (n % 10);
            n /= 10;
            index++;
            if(index > STR_N_DIGITS){
                break;
            }
        }
        tempStr[index] = 0;
        for(uint8_t i = index; i > 0; i--){
            s[i - 1] = tempStr[index - i];
        }
        s[index] = 0;
    }
}

void* memset(void *dst, int value, size_t len)
{
    uint16_t i;
    for( i = 0; i < len; i++){
        *((char*)dst+i) = value;
    }
    return (dst + i);
}

void* memcpy(void *dst, const void *src, size_t len)
{
    uint16_t i;
    for( i = 0 ; i < len; i++){
        *((char*)dst + i) = *((char*)src + i);
    }
    return (dst + i);
}

char* strcat(char *dst, const char *src)
{
    char *pointerSrc = src;
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
    char *pointer = src;
    while( *(pointer) != 0){
        *(dst + (pointer - src)) = *(src + (pointer - src));
        pointer++;
    }
    *(dst + (pointer - src)) = 0;
    return pointer;
}

size_t strlen(const char *str)
{
    char *pointer = str;
    while( *(pointer) != 0){
        pointer++;
    }
    return (pointer - str);
}

char* strstr(const char *str1, const char *str2)
{
    char *init = 0;
    char *pointer = str1;
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
    return init;
}

#if 0
bool isDigit( unsigned char c)
{
    bool result = false;
    if ( c >= '0' && c <= '9' ){ result = true;}
    return result;
}
#endif

long atol(const char *num)
{
    long value = 0;
    int neg = 0;
    // decimal
    if (num[0] == '-') {
        neg = 1;
        num++;
    }
    while (*num && isdigit(*num)){
        value = value * 10 + *num++  - '0';
    }
    value = neg ? -value : value;
    return value;
}
