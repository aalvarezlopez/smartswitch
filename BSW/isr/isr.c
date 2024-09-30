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
#include "smartswitch.h"
#include "sam4s4a.h"
#include "component_adc.h"
#include "component_uart.h"
#include "cmsis_gcc.h"
#include "core_cm4.h"
#include "io_pri.h"


#ifndef _STATIC
#define _STATIC static
#endif
volatile bool OS_Task_A = false;
volatile bool OS_Task_B = false;
volatile bool OS_Task_C = false;
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
        NVIC->ISER[(((uint32_t)isrNumber) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)isrNumber) & 0x1FUL));
    }else{
        NVIC->ICER[(((uint32_t)isrNumber) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)isrNumber) & 0x1FUL));
    }
}


/**
 * @brief Set interrupt level for this interruption
 *
 * @param isrNumber Interruption that will be modified
 * @param priority Priority level. The lower the value, the higher the priority. Valid values are 0-15,
 * only bits 7-4 are considered.
 */
void ISR_setInterruptPriority(uint8_t isrNumber, uint8_t priority)
{
    uint8_t *ptrToIPRx;
    if( priority > 15){
        priority = 15;
    }
    ptrToIPRx = NVIC->IP[(((uint32_t)isrNumber) >> 5UL)];
    ptrToIPRx += (isrNumber % 4);
    ptrToIPRx = priority << 4;
}

void NMI_Handler(void)
{
    while (1) {
    }
}
void HardFault_Handler(void)
{
    while (1) {
    }
}
void MemManage_Handler(void)
{
    while (1) {
    }
}
void BusFault_Handler(void)
{
    while (1) {
    }
}
void UsageFault_Handler(void)
{
    while (1) {
    }
}

/**
 * @brief Systemtick shall be configured to generate and interruption
 * every 1 ms
 */
void SysTick_Handler( void )
{
    if ((system_tick_counter % TASK_SCH_300_ms) == 0) {
        OS_Task_A = true;
    }
    if ((system_tick_counter % TASK_SCH_30_ms) == 0) {
        OS_Task_B = true;
    }
    system_tick_counter++;
}

void PIOB_Handler(void)
{
    PIOB_ISR_Q1_ST( PIOB->PIO_ISR );
    PIOB_ISR_Q2_ST( PIOB->PIO_ISR );
    SmartSwitch_flowMeter(IO_q1(), IO_q2());
}

/**
 * @brief PIOA interruption handler
 */
void PIOA_Handler(void)
{
    bool presence, button;
    PIOA_ISR_BUTTON_ST( PIOA->PIO_ISR );
    PIOA_ISR_PIR_ST( PIOA->PIO_ISR );

    presence = IO_isPIRactive();
    button = IO_isButtonPressed();
    SmartSwitch_Action(presence, button);
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
void UART0_Handler( void )
{
    if( UART0->UART_SR && UART_SR_RXRDY){
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

void RTC_Handler(void)
{
    RTC->RTC_SCCR = RTC_SCCR_TIMCLR;
    OS_Task_C = true;
}
