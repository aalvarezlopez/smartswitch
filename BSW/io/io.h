/**
 * @file io.h
 * @brief I/O hardware abstraction component header
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-17
 */

#ifndef IO_H
#define IO_H

#include "stdint.h"
#include "stdbool.h"


/* Battery voltage conversion channel */
#define IO_BATTERY_LEVEL_ADC_CHANNEL 4u

/* Convert raw value in to voltage*/
#define CH4_ADC_FACTOR 1u
#define CH4_ADC_OFFSET 0u
#define IO_RAW_TO_VOLTS(x) ((x * CH4_ADC_FACTOR) + CH4_ADC_OFFSET)


void IO_Init(void);
void IO_triggerNewAcquisition(void);
bool IO_getLastAcquiredValue(uint8_t channel, uint16_t* const value );
void IO_newAcquisitionCompleted(uint8_t channel, uint16_t value);
bool IO_oneWire_Read(void);
void IO_oneWire_Input(void);
void IO_oneWire_Output(void);
void IO_oneWire_Set(void);
void IO_oneWire_Clear(void);

#endif
