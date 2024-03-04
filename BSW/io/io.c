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
    PIOA->PIO_WPMR  = PIO_WPMR_WPKEY_PASSWD;
    PIOA->PIO_ODR   = PIO_PER_P16;
    PIOA->PIO_PUER  = PIO_PUER_P16;
    PIOA->PIO_WPMR  = PIO_WPMR_WPEN |
                     PIO_WPMR_WPKEY_PASSWD;
}

void IO_oneWire_Output(void)
{
    PIOA->PIO_WPMR = PIO_WPMR_WPKEY_PASSWD;
    PIOA->PIO_OER  = PIO_PER_P16;
    PIOA->PIO_SODR = PIO_SODR_P16;
    PIOA->PIO_WPMR = PIO_WPMR_WPEN |
                     PIO_WPMR_WPKEY_PASSWD;
}

void IO_oneWire_Set(void)
{
    PIOA->PIO_SODR = PIO_SODR_P16;
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

void IO_openRadiatorValve(uint8_t index, bool enabled)
{
    if( enabled == true ){
        if( index == 0){
            PIOB->PIO_SODR = PIO_SODR_P2;
        }else{
            PIOB->PIO_SODR = PIO_SODR_P3;
        }
    }else{
        if( index == 0){
            PIOB->PIO_CODR = PIO_CODR_P2;
        }else{
            PIOB->PIO_CODR = PIO_CODR_P3;
        }
    }
}

void IO_getRadiatorState(bool * const state)
{
    state[0] = PIOB->PIO_ODSR & ( PIO_ODSR_P2);
    state[1] = PIOB->PIO_ODSR & ( PIO_ODSR_P3);
}

void IO_setShutterPosition(uint8_t index, uint8_t position)
{
}

void IO_getShutterPosition(uint8_t * const position)
{
    position[0] = 0;
    position[1] = 0;
    position[2] = 0;
}

void IO_setDimmer(uint8_t value)
{
}

uint8_t IO_getDimmer(void)
{
}

bool IO_isButtonPressed(void)
{
    return PIOA->PIO_ODSR & PIO_ODSR_P8;
}

bool IO_isPIRactive(void)
{
    return PIOA->PIO_ODSR & PIO_ODSR_P9;
}

void IO_setLights(bool active)
{
    if( active ){
        PIOA->PIO_SODR = PIO_SODR_P10;
    }else{
        PIOA->PIO_CODR = PIO_CODR_P10;
    }
}

bool IO_getLights(void)
{
    return PIOA->PIO_ODSR & PIO_ODSR_P10;
}
