/**
 * @file uart.c
 * @brief UART driver to send and receive messages through Tx/Rx
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-17
 *
 * Baudrate is divided by 16 after the prescales
 * Desired baudrate is 115200
 */

#include "stdint.h"
#include "uart.h"
#include "sam4s4a.h"
#include "component_uart.h"
#include "stddef.h"


#define UART_MHZ_TO_HZ 1000000U
#define UART_FIX_PRESCALER 32u
#define UART_BAUDRATE 115200


#ifndef _STATIC
#define _STATIC static
#endif

#define DEBUG_MODE

#ifdef DEBUG_MODE
char debug_log[1024];
uint16_t debugLogCnt = 0;
#endif

_STATIC uint16_t rx_counter =0;
_STATIC uint8_t rx_buffer[UART_MAX_RX_BUFF_LEN];


/**
 * @brief Initialize UART0 peripheral
 */
void UART_Init(void)
{
    UART0->UART_BRGR = ((uint32_t)(EcuM_getMainClockSpeed()) * UART_MHZ_TO_HZ )
        / UART_FIX_PRESCALER / UART_BAUDRATE;
    UART0->UART_MR = UART_MR_PAR_NO | UART_MR_CHMODE_NORMAL;
    UART0->UART_CR =  UART_CR_RSTRX | UART_CR_RSTTX | UART_CR_RSTSTA;
    UART0->UART_CR = UART_CR_RXEN | UART_CR_TXEN;
    UART0->UART_IER = UART_IER_RXRDY;
}


/**
 * @brief Transmit an array of chars through the UART0. Blocking function, it will
 * stay here till the transmission is completed
 *
 * @param dout Pointer to the array of characters to be trasnmitted
 * @param len Number of characters to be transmitted
 */
void UART_tx(const uint8_t* const dout, uint8_t len)
{
    ISR_disableAllInterrupts();
    for(uint8_t i = 0; i < len; i++){
        while( (UART0->UART_SR & UART_SR_TXRDY) == 0 ){ continue;}
        UART0->UART_THR = dout[i];
#ifdef DEBUG_MODE
        debug_log[debugLogCnt] = dout[i];
        debugLogCnt++;
        debugLogCnt = (debugLogCnt >= 1024) ? 0 : debugLogCnt;
#endif
    }
    ISR_enableAllInterrupts();
}


/**
 * @brief Callback function called from the ISR handler
 */
void UART_rx(void)
{
    rx_buffer[rx_counter] = UART0->UART_RHR;
#ifdef DEBUG_MODE
    debug_log[debugLogCnt] = rx_buffer[rx_counter];
    debugLogCnt++;
    debugLogCnt = (debugLogCnt >= 1024) ? 0 : debugLogCnt;
#endif
    rx_counter++;
    rx_counter = (rx_counter >= UART_MAX_RX_BUFF_LEN) ? 0 : rx_counter;
}


/**
 * @brief Get received chars since the last time the buffer was cleared
 *
 * @param ch Pointer where the received message will be saved
 *
 * @return  Number of storage chars
 */
uint16_t UART_get_rx_buffer(char * const ch)
{
    for(uint16_t i = 0; i < rx_counter; i++){
        if(rx_buffer[i] == 0){rx_buffer[i] = '^';}
        ch[i] = rx_buffer[i];
    }
    return rx_counter;
}


/**
 * @brief Clean internal buffer and get the contain
 *
 * @param ch Pointer where the received buffer will be copied
 *
 * @return Number of storage chars
 */
uint8_t UART_flush(char * const ch)
{
    uint8_t result = 0;
    for(uint8_t i = 0; i < rx_counter; i++){
        if( ch != NULL ){
            ch[i] = rx_buffer[i];
        }
        rx_buffer[i] = 0;
    }
    result = rx_counter;
    rx_counter = 0;
    return result;
}
