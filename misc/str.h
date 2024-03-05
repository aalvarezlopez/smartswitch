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

void __atoi(uint16_t n, char *s);
void int_to_str(char * dst, uint16_t number, uint8_t leading);

#endif
