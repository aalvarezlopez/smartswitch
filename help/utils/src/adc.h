/*
 * adc.h
 *
 *  Created on: 05/12/2013
 *      Author: Adrian
 */

#ifndef ADC_H_
#define ADC_H_

#define POS_REF_AD0 0
#define NEG_REF_GROUND 0x3
#define EXT_REF 0b10


void EnableADC(unsigned int clock_divisor);
void DisableADC();
void TriggerADC();
int IsADCFinished();
unsigned int GetADC();
unsigned int GetmvADC();

#endif /* ADC_H_ */
