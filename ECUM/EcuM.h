/**
 * @file EcuM.h
 * @brief ECU abstraction layer
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-14
 */

#ifndef ECUM_H
#define ECUM_H
#include "stdio.h"

# define ECUM_MAIN_CLOCK_FREQ 96


void EcuM_Startup_one(void);
void EcuM_Startup_two(void);
uint8_t EcuM_getMainClockSpeed(void);
uint32_t EcuM_GetCurrentCounter(void);
uint32_t EcuM_GetElapsed(void);


#endif
