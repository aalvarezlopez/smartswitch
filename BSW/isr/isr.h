/**
 * @file isr.h
 * @brief ISR header file
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-17
 */

#ifndef ISR_H
#define ISR_H

#include "stdint.h"
#include "stdbool.h"

/* The LED1 is on for 500 ms and off for another 500 ms*/
#define LED_DISPLAY_500_ms 500U
#define TASK_SCH_300_ms 300U

void ISR_disableAllInterrupts(void);
void ISR_enableAllInterrupts(void);
void ISR_setInterruptEnable(uint8_t isrNumber, bool state);

#endif
