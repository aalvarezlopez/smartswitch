/**
 * @file str.h
 * @brief Miscellaneous header
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-31
 */

#ifndef STR_H
#define STR_H

#define STR_N_DIGITS 3

uint16_t __atoi( char *s);
void int_to_str(char * dst, uint16_t number, uint8_t leading);
char* strncpy(char *dst, const char *src, uint16_t n);
void* memcpy(void *dst, const void *src, uint16_t len);
#endif
