/**
 * @file io.c
 * @brief I/o hw abstraction layer. Any kind of intputs and outputs
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-17
 */

#include "io.h"
#include "io_pri.h"
#include "isr.h"
#include "component_adc.h"

/* Flag to check whether the ADC channel conversion is reay or not. It will
 * be set to false the first read after the conversion and chang to true
 * when a new conversion is completed
 */

#ifndef _STATIC
#define _STATIC static
#endif

#define N_ADC_CHANNELS 1u

_STATIC bool updatedChannel[N_ADC_CHANNELS];

/* ADC channel conversion in raw*/
_STATIC uint16_t lastChannelRawValue[N_ADC_CHANNELS];


/**
 * @brief Module initialization
 */
void IO_Init(void)
{
    for (uint8_t i = 0; i < N_ADC_CHANNELS; i++) {
        updatedChannel[i] = false;
    }
}

bool IO_oneWire_Read(void)
{
   return (PIOA->PIO_PDSR & PIO_PDSR_P16) != 0;
}

void IO_oneWire_Input(void)
{
    PIOA->PIO_WPMR = PIO_WPMR_WPKEY_PASSWD;
    PIOA->PIO_ODR = PIO_PER_P16;
    PIOA->PIO_PUER    = PIO_PUER_P16;
    PIOA->PIO_WPMR = PIO_WPMR_WPEN |
                     PIO_WPMR_WPKEY_PASSWD;
}

void IO_oneWire_Output(void)
{
    PIOA->PIO_WPMR = PIO_WPMR_WPKEY_PASSWD;
    PIOA->PIO_OER = PIO_PER_P16;
    PIOA->PIO_SODR = PIO_SODR_P16;
    PIOA->PIO_WPMR = PIO_WPMR_WPEN |
                     PIO_WPMR_WPKEY_PASSWD;
}

void IO_oneWire_Set(void)
{
    PIOA->PIO_SODR = PIO_CODR_P16;
}

void IO_oneWire_Clear(void)
{
    PIOA->PIO_CODR = PIO_CODR_P16;
}
/**
 * @brief Launch a new ADC acquisition using the channels which were previously
 * selected
 */
void IO_triggerNewAcquisition(void)
{
    ADC->ADC_CR = ADC_CR_START;
}


/**
 * @brief It might be called form an ISR when the acquisition was completed in order
 * to notify this module about it.
 *
 * @param channel Which channel has completed
 * @param value Conversion value in raw
 */
void IO_newAcquisitionCompleted(uint8_t channel, uint16_t value)
{
    ISR_disableAllInterrupts();
    lastChannelRawValue[channel] = value;
    updatedChannel[channel] = true;
    ISR_enableAllInterrupts();
}


/**
 * @brief Get ADC conversion from the latest conversion
 *
 * @param channel Which channel has just acquired
 * @param value Conversion value in raw
 *
 * @return
 */
bool IO_getLastAcquiredValue(uint8_t channel, uint16_t* const value )
{
    bool updated = updatedChannel[channel];
    ISR_disableAllInterrupts();
    updatedChannel[channel] = false;
    *value = lastChannelRawValue[channel];
    ISR_enableAllInterrupts();
    return updated;
}
