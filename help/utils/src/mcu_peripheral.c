/// board_init.c
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
/// Authored by:   Adrian (Nov 20, 2013)
/// Revised by:    JRB (16/12/2013)
/// Last Version:  Nov 20, 2013
///
/// FILE CONTENTS:
/// Low level MCU utilities to configure internal peripherals: PIO, TIMER, CLOCK SOURCE

#include "sam4lc4c.h"
#include "configuration.h"
#include "mcu_peripheral.h"
/** \section brief
 * Board initialization: pin configuration and run main oscillator
 *
 */

/** \brief Enable clock for the peripherals
 *
 */
void EnableTimer0PeripheralClock()
{
    PM->PM_UNLOCK = (0xAA << 24) | 0x24;
    PM->PM_HSBMASK |= 1 << 5;
    PM->PM_UNLOCK = (0xAA << 24) | 0x28;
    PM->PM_PBAMASK |= 1 << 2;
    PM->PM_UNLOCK = (0xAA << 24) | 0x28;
    PM->PM_PBAMASK |= 1 << 3;
    PM->PM_UNLOCK = (0xAA << 24)
                    | PM_UNLOCK_ADDR((uint32_t)&PM->PM_PBADIVMASK - (uint32_t)PM);
    PM->PM_PBADIVMASK = 0x55;
}

/** \brief Configure pin as input or output
 *
 *  \param pin: pin number
 *         direction: input or output
 *         pullUpEnable: set pull up enable or not (only input)
 */
void SetPinDirection(int pin, int direction, int pull_up_enable)
{
    int port, pin_index;
    port = pin / PORTSIZE;
    pin_index = pin % PORTSIZE;

    if (direction == OUTPUTPIN) {
        GPIO->GPIO_PORT[port].GPIO_GPERS = (1 << pin_index);
        GPIO->GPIO_PORT[port].GPIO_ODERS = (1 << pin_index);
        GPIO->GPIO_PORT[port].GPIO_OVRC = (1 << pin_index);
    } else {
        GPIO->GPIO_PORT[port].GPIO_GPERS = (1 << pin_index);
        GPIO->GPIO_PORT[port].GPIO_ODERC = (1 << pin_index);
        // Always enable the Schmitt trigger for input pins.
        GPIO->GPIO_PORT[port].GPIO_STERS = (1 << pin_index);

        if (pull_up_enable == PULLUPENABLE) {
            GPIO->GPIO_PORT[port].GPIO_PUERS = (1 << pin_index);
        }
    }
}

/** \brief Configure pin for special peripheral
 *
 *  \param pin: pin number
 *         function: determine wich function
 */
void SetPinFunction(int pin, int function)
{
    int port, pin_index;
    port = pin / PORTSIZE;
    pin_index = pin % PORTSIZE;

    if (function & PMR0_MASK) {
        GPIO->GPIO_PORT[port].GPIO_PMR0S = (1 << pin_index);
    } else {
        GPIO->GPIO_PORT[port].GPIO_PMR0C = (1 << pin_index);
    }

    if (function & PMR1_MASK) {
        GPIO->GPIO_PORT[port].GPIO_PMR1S = (1 << pin_index);
    } else {
        GPIO->GPIO_PORT[port].GPIO_PMR1C = (1 << pin_index);
    }

    if (function & PMR2_MASK) {
        GPIO->GPIO_PORT[port].GPIO_PMR2S = (1 << pin_index);
    } else {
        GPIO->GPIO_PORT[port].GPIO_PMR2C = (1 << pin_index);
    }

    GPIO->GPIO_PORT[port].GPIO_GPERC = (1 << pin_index);
}

/** \brief Set pin value
 *
 *  \param pin: pin number
 *         value: high or low
 */
void SetPinValue(int pin, int value)
{
    int port, pin_index;
    port = pin / PORTSIZE;
    pin_index = pin % PORTSIZE;

    if (value == HIGH) {
        GPIO->GPIO_PORT[port].GPIO_OVRS = (1 << pin_index);
    } else {
        GPIO->GPIO_PORT[port].GPIO_OVRC = (1 << pin_index);
    }
}

/** \brief Disable GPIO pin
 *
 *  \param pin: pin number
 *         value: high or low
 */
void DisablePin(int pin)
{
    int port, pin_index;
    port = pin / PORTSIZE;
    pin_index = pin % PORTSIZE;
    GPIO->GPIO_PORT[port].GPIO_GPERS = (1 << pin_index);
}

/** \brief Toggle pin value
 *
 *  \param pin: pin number
 *         value: high or low
 */
void TogglePinValue(int pin)
{
    int port, pin_index;
    port = pin / PORTSIZE;
    pin_index = pin % PORTSIZE;
    GPIO->GPIO_PORT[port].GPIO_OVRT = (1 << pin_index);
}

/** \brief This function configure main oscillator
 *          Configure oscillator for 48 MHz clock
 */
void ConfigureOsc()
{
    //configure osc0
    SCIF->SCIF_OSCCTRL0 = SCIF_OSCCTRL0_OSCEN | SCIF_OSCCTRL0_MODE |
                          SCIF_OSCCTRL0_GAIN(2);  //0x00010307
    //configure pll
    SCIF->SCIF_UNLOCK = 0xAA << SCIF_UNLOCK_KEY_Pos | 0x24;
    SCIF->SCIF_PLL[0].SCIF_PLL = SCIF_PLL_PLLEN | SCIF_PLL_PLLDIV(4)
                                 | SCIF_PLL_PLLMUL(0xF) | SCIF_PLL_PLLCOUNT(0x3F);    //0x3F0F0401
    // Set a flash wait state depending on the new cpu frequency.
    HFLASHC->FLASHCALW_FCR = 0x40;
    // Configure clock source as PLL
    PM->PM_UNLOCK = 0xAA << PM_UNLOCK_KEY_Pos | 0;
    PM->PM_MCCTRL = PM_MCCTRL_MCSEL_PLL0;
}

/** \brief This function configure timer
 *
 *  *  \param   channel: timer channel (0,1 o 2)
 *              clock: source clock of timer
 *              captureReg: value of capture
 */
void TimerCaptureConfigure(int channel, int clock, int capture_reg,
                           int priority_int)
{
    //peripheral clock enable
    EnableTimer0PeripheralClock();
    //enable TIMER0 CAPTURE interrupt
    NVIC->ISPR[((uint32_t)(TC00_IRQn) >> 5)] = (1
                                                << ((uint32_t)(TC00_IRQn) & 0x1F)); /* set interrupt pending */
    NVIC->IP[(uint32_t)(TC00_IRQn)] = ((priority_int << (8 - __NVIC_PRIO_BITS))
                                       & 0xff); /* set Priority for device specific Interrupts  */
    NVIC->ISER[(uint32_t)((int32_t) TC00_IRQn) >> 5] = (uint32_t)(
                                                           1 << ((uint32_t)((int32_t) TC00_IRQn) & (uint32_t)
                                                                 0x1F)); /* enable interrupt */
    //configure timer
    TC0->TC_CHANNEL[channel].TC_CMR = TC_CMR_CPCTRG | (clock);
    TC0->TC_CHANNEL[channel].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
    TC0->TC_CHANNEL[channel].TC_RC = capture_reg;
}

/** \brief This function enable/disabe interrupt
 *
 *  *  \param   channel: set channel of the timer
 *              enable: set enable or disable the interrupt
 *
 */
void TimerEnableInterrupt(int channel, int enable)
{
    if (enable == TMRINTENABLE) {
        TC0->TC_CHANNEL[channel].TC_IER = TC_IER_CPCS;
    } else {
        TC0->TC_CHANNEL[channel].TC_IDR = TC_IER_CPCS;
    }
}

/** \brief This function configure EIC
 *  Show table 4-2 Interrupt Request Signal Map
 *  and table 3-1 GPIO Controller Function Multiplexing
 *  for view EIC number and function
 *
 *  *  \param   pin: pin number
 *              external_interrupt: extint number
 *              function: function for EIC pin
 *              priority
 *
 */
void ExternalInterruptConfigure(int pin, int external_interrupt,
                                int function, int priority)
{
    int port, pin_index;
    port = pin / PORTSIZE;
    pin_index = pin % PORTSIZE;

    if (external_interrupt < 1 || external_interrupt > 30) {
        return;
    }

    EIC->EIC_DIS = 1 << external_interrupt;
    EIC->EIC_ICR = 1 << external_interrupt;
    EIC->EIC_ISR;

    unsigned int eic = EIC_1_IRQn + (external_interrupt - 1);
    unsigned int eic_port = eic >> 5;
    unsigned int eic_pin = eic & 0x1F;

    SetPinDirection(pin, INPUTPIN, PULLUPENABLE);
    SetPinFunction(pin, function);

    /* Set up mode level */
    EIC->EIC_MODE = (EIC->EIC_MODE & ~(1 << external_interrupt));
    EIC->EIC_EDGE = (EIC->EIC_EDGE | (1 << external_interrupt));
    EIC->EIC_LEVEL = (EIC->EIC_LEVEL | (1 << external_interrupt));
    EIC->EIC_FILTER = (EIC->EIC_FILTER | (1 << external_interrupt));
    EIC->EIC_ASYNC = (EIC->EIC_ASYNC | (1 << external_interrupt));

    //Todo: change for correct eic_port and eic_pin
    NVIC->ISPR[((uint32_t)(eic) >> 5)] = (1 << ((uint32_t)(eic) &
                                                0x1F)); /* set interrupt pending */
    NVIC->IP[(uint32_t)(eic)] = ((priority << (8 - __NVIC_PRIO_BITS)) &
                                 0xff); /* set Priority for device specific Interrupts  */
    NVIC->ISER[(uint32_t)((int32_t)eic) >> 5] = (uint32_t)(1 << ((uint32_t)((
                                                                                int32_t)eic) & (uint32_t)0x1F)); /* enable interrupt */
}

/** \brief This function enable EIC interrupt
 *          to be managed in handler
 *
 *  *  \param   enable
 *
 */
void EICHandlerEnable(int external_interrupt, int enable)
{
    if (enable) {
        EIC->EIC_ICR = 1 << external_interrupt;
        EIC->EIC_ISR;
        EIC->EIC_IER = 1 << external_interrupt;
    } else {
        EIC->EIC_IDR = 1 << external_interrupt;
    }
}

/** \brief This function enable/disable
 *  EIC interrupt
 *
 *  *  \param   enable
 *
 */
void EICEnable(int external_interrupt, int enable)
{
    if (enable) {
        EIC->EIC_ICR = 1 << external_interrupt;
        EIC->EIC_ISR;
        EIC->EIC_EN = 1 << external_interrupt;
    } else {
        EIC->EIC_DIS = 1 << external_interrupt;
    }
}


/** \brief This function configure SPI chip select device
 *          , each select device has his own baudrate, clock
 *          configuration, ...
 *
 *  *  \param   channel: spi select device (0,1,2,3)
 *              baudrate: value of the divisor for baudrate
 *
 */
void SpiCSDConfiguration(int channel, int baudrate)
{
    //REVIEW: Use defines??
    //the function is not implemented, so use a TODO flag
    SPI->SPI_CSR[1] = (4 << 8);
}

