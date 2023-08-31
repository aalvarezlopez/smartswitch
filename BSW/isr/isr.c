/**
 * @file isr.c
 * @brief SAM4S interrupts services routines and configuration
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-17
 */

#include "isr.h"
#include "io.h"
#include "spi.h"
#include "uart.h"
#include "fluid_ctrl.h"
#include "sam4s4a.h"
#include "component_adc.h"
#include "component_uart.h"
#include "cmsis_gcc.h"
#include "core_cm4.h"
#include "io_pri.h"


#ifndef _STATIC
#define _STATIC static
#endif
volatile bool OS_systemtick = false;
volatile bool g_interrupt_enabled = true;

uint32_t system_tick_counter = 0;

_STATIC uint8_t irq_disable_counter = 0;
/**
 * @brief Disable all the interrupts. Current enable status shall be stored
 * to be restored later
 *
 * This function can be nested
 */
void ISR_disableAllInterrupts(void)
{
    __disable_irq();
    irq_disable_counter++;
}


/**
 * @brief Enable all the interrupts. Interruption shall be recovered as they
 * were before disabling for time.
 *
 * This function must be call as many times as ISR_disableAllFuncionts was called
 * prior to recover the interruptions
 */
void ISR_enableAllInterrupts(void)
{
    if( irq_disable_counter != 0 ){
        irq_disable_counter--;
    }
    if(irq_disable_counter == 0){
        __enable_irq();
    }
}


/**
 * @brief Enable/disable a specific interruption
 *
 * @param isrNumber Interruption what will be modified
 * @param state True the interruption will be generated, false in other case
 */
void ISR_setInterruptEnable(uint8_t isrNumber, bool state)
{
    if(state){
        NVIC_EnableIRQ(isrNumber);
    }else{
        NVIC_DisableIRQ(isrNumber);
    }
}

/**
 * @brief Systemtick shall be configured to generate and interruption
 * every 1 ms
 */
void SysTick_Handler( void )
{
    if ((system_tick_counter % TASK_SCH_300_ms) == 0) {
        OS_systemtick = true;
    }
    system_tick_counter++;
}


/**
 * @brief PIOA interruption handler
 */
void PIOA_Handler(void)
{
}


/**
 * @brief Adc handler
 */
void ADC_Handler(void)
{
    if (ADC->ADC_ISR & ADC_ISR_DRDY) {
        IO_newAcquisitionCompleted( ADC->ADC_LCDR >> ADC_LCDR_CHNB_Pos,
                                    ADC->ADC_LCDR & ADC_LCDR_LDATA_Msk);
    }
}


/**
 * @brief Uart 1 ISR handler
 */
void UART1_Handler( void )
{
    if( UART1->UART_SR && UART_SR_RXRDY){
        UART_rx();
    }
}

/**
 * @brief USP ISR handler
 */
void UDP_Handler(void)
{
    udp_isr_handler();
}
