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

void IO_PWM_Init(void)
{
    PWM->PWM_WPCR = PWM_WPCR_WPCMD_DISABLE_SW_PROT | PWM_WPCR_WPKEY_PASSWD | PWM_WPCR_WPRG0 |
                    PWM_WPCR_WPRG1 | PWM_WPCR_WPRG2 | PWM_WPCR_WPRG3;
    PWM->PWM_CLK = PWM_CLK_PREA(10) | PWM_CLK_PREB(10) | PWM_CLK_PREA(1) | PWM_CLK_PREB(1);

    /* 48 MHz / 512 = 9.375 kHz */
    PWM->PWM_CH_NUM[0].PWM_CMR = PWM_CMR_CPRE_MCK_DIV_512 | PWM_CMR_CPOL;
    PWM->PWM_CH_NUM[1].PWM_CMR = PWM_CMR_CPRE_MCK_DIV_512 | PWM_CMR_CPOL;
    PWM->PWM_CH_NUM[2].PWM_CMR = PWM_CMR_CPRE_MCK_DIV_512 | PWM_CMR_CPOL;
    PWM->PWM_CH_NUM[3].PWM_CMR = PWM_CMR_CPRE_MCK_DIV_512 | PWM_CMR_CPOL;

    /* 9375 / 200 = 468 ----> 200 HZ */
    PWM->PWM_CH_NUM[0].PWM_CPRD = 468;
    PWM->PWM_CH_NUM[1].PWM_CPRD = 468;
    PWM->PWM_CH_NUM[2].PWM_CPRD = 468;
    PWM->PWM_CH_NUM[3].PWM_CPRD = 468;

    PWM->PWM_CH_NUM[0].PWM_CDTY = 0;
    PWM->PWM_CH_NUM[1].PWM_CDTY = 0;
    PWM->PWM_CH_NUM[2].PWM_CDTY = 0;
    PWM->PWM_CH_NUM[3].PWM_CDTY = 0;

    PWM->PWM_ENA = PWM_ENA_CHID0 |
                   PWM_ENA_CHID1 |
                   PWM_ENA_CHID2 |
                   PWM_ENA_CHID3;

    PWM->PWM_WPCR = PWM_WPCR_WPCMD_ENABLE_SW_PROT | PWM_WPCR_WPKEY_PASSWD | PWM_WPCR_WPRG0 |
                    PWM_WPCR_WPRG1 | PWM_WPCR_WPRG2 | PWM_WPCR_WPRG3;
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
    PWM->PWM_CH_NUM[index].PWM_CDTYUPD = (position * 468) / 100;
}

void IO_getShutterPosition(uint8_t * const position)
{
    position[0] = (PWM->PWM_CH_NUM[0].PWM_CDTY * 100) / 468 ;
    position[1] = (PWM->PWM_CH_NUM[1].PWM_CDTY * 100) / 468 ;
    position[2] = (PWM->PWM_CH_NUM[2].PWM_CDTY * 100) / 468 ;
}

void IO_setDimmer(uint8_t value)
{
    PWM->PWM_CH_NUM[3].PWM_CDTYUPD = (value * 468) / 100;
}

uint8_t IO_getDimmer(void)
{
    return (PWM->PWM_CH_NUM[3].PWM_CDTY * 100) / 468 ;
}

bool IO_isButtonPressed(void)
{
    return PIOA->PIO_PDSR & PIO_PDSR_P8;
}

bool IO_q1(void)
{
    return PIOB->PIO_PDSR & PIO_PDSR_P0;
}

bool IO_q2(void)
{
    return PIOB->PIO_PDSR & PIO_PDSR_P1;
}

bool IO_isPIRactive(void)
{
    return PIOA->PIO_PDSR & PIO_PDSR_P9;
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
