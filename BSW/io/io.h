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
void IO_PWM_Init(void);
void IO_triggerNewAcquisition(void);
bool IO_getLastAcquiredValue(uint8_t channel, uint16_t* const value );
void IO_newAcquisitionCompleted(uint8_t channel, uint16_t value);
bool IO_oneWire_Read(void);
void IO_oneWire_Input(void);
void IO_oneWire_Output(void);
void IO_oneWire_Set(void);
void IO_oneWire_Clear(void);
void IO_openRadiatorValve(uint8_t index, bool enabled);
void IO_getRadiatorState(bool * const state);
void IO_setShutterPosition(uint8_t index, uint8_t position);
void IO_getShutterPosition(uint8_t * const position);
void IO_setDimmer(uint8_t value);
uint8_t IO_getDimmer(void);
bool IO_isButtonPressed(void);
bool IO_isPIRactive(void);
void IO_setLights(bool active);
bool IO_getLights(void);

#endif
