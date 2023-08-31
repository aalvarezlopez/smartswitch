/**
 * @file uart.h
 * @brief UART driver header file
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-17
 */

#ifndef UART_H
#define UART_H

#define UART_MAX_RX_BUFF_LEN 512

void UART_Init(void);
void UART_tx(const uint8_t* const dout, uint8_t len);
void UART_rx(void);
uint16_t UART_get_rx_buffer(char * const ch);
uint8_t UART_flush(char * const ch);
#endif
