/// adc.c
///
/// Copyright (C) 2013 INGEN10 Ingenieria SL
/// http://www.ingen10.com
///
/// LEGAL NOTICE:
/// All information contained herein is, and remains property of INGEN10 Ingenieria SL.
/// Dissemination of this information or reproduction of this material is strictly
/// forbidden unless prior written permission is obtained from its owner.
/// ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC PERFORMANCE, OR PUBLIC DISPLAY
/// OF, OR THROUGH USE OF THIS SOURCE CODE IS STRICTLY PROHIBITED, AND IT IS A VIOLATION
/// OF INTERNATIONAL TRADE TREATIES AND LAWS.
/// THE RECEIPT OR POSSESSION OF THIS DOCUMENT DOES NOT CONVEY OR IMPLY ANY RIGHTS.
///
/// Authored by:   Adrian (05/12/2013)
/// Revised by:    JRB (16/12/2013)
/// Last Version:  05/12/2013
///
/// FILE CONTENTS:
/// MCU internal ADC control and configuration funcions

#include "sam4lc4c.h"
#include "adc.h"

/** \section brief
 * ADC peripheral control and configuration
 *
 */

/** \brief Check if ADC conversion is finished
 *
 *  \return 1 if it's finished
 *          0 in other case
 */
int IsADCFinished()
{
    int status_register;
    status_register = ADCIFE->ADCIFE_SR;

    if ( (status_register & ADCIFE_SR_SBUSY) != 0) { return 0; }

    return 1;
}

/** \brief Get adc conversion value
 *
 *  \return unsigned int with the ADC value (raw)
 */
unsigned int GetADC()
{
    return (ADCIFE->ADCIFE_LCV & ADCIFE_LCV_LCV_Msk);
}

#define ADCRESOLUTION   12
#define EXTERNALMULT    2
#define EXTERNALREF     3200
/** \brief Get adc value and apply conversion to mv
 *
 *  \return unsigned int voltage in mv
 */
unsigned int GetmvADC()
{
    unsigned int raw_value;
    unsigned int mv;
    raw_value = GetADC();
    mv = raw_value;
    mv *= EXTERNALMULT;
    mv *= EXTERNALREF;
    mv >>= (ADCRESOLUTION);
    return mv;
}

/** \brief ADC clock enable and configuration
 *
 */
void EnableADCPeripheralClock(unsigned int clock_divisor)
{
    int i;
    PM->PM_UNLOCK = (0xAA << 24) | 0x24;
    PM->PM_HSBMASK |= 1 << 5;
    PM->PM_UNLOCK = (0xAA << 24) | 0x28;
    PM->PM_PBAMASK |= (1 << 12);

    SCIF->SCIF_GCCTRL[10].SCIF_GCCTRL = SCIF_GCCTRL_DIV(
                                            clock_divisor) | SCIF_GCCTRL_OSCSEL(9)
                                        | SCIF_GCCTRL_DIVEN | SCIF_GCCTRL_CEN;          //source PBA/100 and enable

    for (i = 0; i < 1000; i++) { continue; }
}

/** \brief trigger ADC conversion
 *
 */
void TriggerADC()
{
    ADCIFE->ADCIFE_CR = ADCIFE_CR_STRIG;
}

/** \brief ADC enable and configure ADC
 *          select generic clock for ADC
 *          without speed reduction
 *          external positive and negative reference
 *          select channel 0
 *          no zoom
 *          gain x1
 *          12 bits resolution
 *  \param: clockDivisor: select divisor of clock for ADC
 */
void EnableADC(unsigned int clock_divisor)
{
    EnableADCPeripheralClock(clock_divisor);
    ADCIFE->ADCIFE_CR = ADCIFE_CR_SWRST;
    ADCIFE->ADCIFE_CR = ADCIFE_CR_EN;

    while (!(ADCIFE->ADCIFE_SR & ADCIFE_SR_EN)) {
        continue;
    }

    ADCIFE->ADCIFE_CR = ADCIFE_CR_BGREQEN | ADCIFE_CR_REFBUFEN;

    ADCIFE->ADCIFE_CFG = ADCIFE_CFG_REFSEL(EXT_REF);          //external reference
    ADCIFE->ADCIFE_SEQCFG = ADCIFE_SEQCFG_MUXNEG(NEG_REF_GROUND)
                            | ADCIFE_SEQCFG_MUXPOS(POS_REF_AD0)
                            | ADCIFE_SEQCFG_INTERNAL(0b10);
}

/** \brief ADC disable
 */
void DisableADC()
{
    ADCIFE->ADCIFE_CR = ADCIFE_CR_SWRST;
    ADCIFE->ADCIFE_CR = ADCIFE_CR_DIS;

    while ((ADCIFE->ADCIFE_SR & ADCIFE_SR_EN)) {
        continue;
    }

    ADCIFE->ADCIFE_CR = ADCIFE_CR_BGREQDIS | ADCIFE_CR_REFBUFDIS;
    ADCIFE->ADCIFE_CFG = ADCIFE_CFG_REFSEL(EXT_REF);
    //Disable generic clock
    SCIF->SCIF_GCCTRL[10].SCIF_GCCTRL = 0;
}
