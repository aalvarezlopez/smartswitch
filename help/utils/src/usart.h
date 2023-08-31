/*
 * usart.h
 *
 *  Created on: Nov 20, 2013
 *      Author: Adrian
 */

#ifndef USART_H_
#define USART_H_

#include "sam4lc4c.h"

//RETURN ERROR DEFINITIONS
#define PORT_ERROR -1
#define SUCCESFULL 1
#define PBA_DIVMASK_CLK_USART        (1u << 2)
#define PLL0_SOURCE 0x001000;

#define MAXUSARTPORT    3
#define MAXSTRINGSIZE   50

int configure_USART(unsigned int baudrate);
void enable_tx_rx(void);
int enable_clock_usart(unsigned int port);
int start_USART(int port, unsigned int baudrate);
void usart_serial_putchar(const char c);
void usart_write(const char* string);
void usartHexWrite(const char* string, unsigned int value);

#endif /* USART_H_ */
