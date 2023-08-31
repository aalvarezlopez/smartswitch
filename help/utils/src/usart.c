/// usart.c
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
/// Revised by:
/// Last Version:  Nov 20, 2013
///
/// FILE CONTENTS:
///

#include "usart.h"

/** \brieg Start usart communication
 *
 */
int start_USART(int port, unsigned int baudrate)
{
    if (port > MAXUSARTPORT) { return PORT_ERROR; }

    enable_clock_usart(port);
    configure_USART(baudrate);
    enable_tx_rx();
    return SUCCESFULL;
}
/** \brief Configure USART for serial write/read
 *          MODE NORMAL
 *          8 bits
 *          1 stop bit
 *          non parity
 *          CLK_USART
 *          Asynchronous
 */
int configure_USART(unsigned int baudrate)
{
    USART1->US_BRGR = baudrate;
    USART1->US_MR = US_MR_CHRL_8 | US_MR_PAR_NONE;
    return SUCCESFULL;
}

/** \brief Enable clock for serial
 *
 */
int enable_clock_usart(unsigned int port)
{
    PM->PM_UNLOCK = (0xAA << PM_UNLOCK_ADDR_Pos)
                    | (&(PM->PM_HSBMASK) - (volatile RwReg*) PM);
    PM->PM_HSBMASK |= 1 << PM_HSBMASK_HTOP0_Pos;
    PM->PM_UNLOCK = (0xAA << PM_UNLOCK_ADDR_Pos)
                    | (&(PM->PM_PBAMASK) - (volatile RwReg*) PM);
    PM->PM_PBAMASK |= 1 << (PM_PBAMASK_USART0_Pos + port);
    //sysclk_enable_pba_divmask(PBA_DIVMASK_CLK_USART);
    PM->PM_UNLOCK = (0xAA << PM_UNLOCK_ADDR_Pos)
                    | (&(PM->PM_PBADIVMASK) - (volatile RwReg*) PM);
    PM->PM_PBADIVMASK |= PBA_DIVMASK_CLK_USART;
    return SUCCESFULL;
}

/** \brief enable Tx and Rx USART transmision
 *
 */
void enable_tx_rx(void)
{
    USART1->US_CR = US_CR_RXEN;
    USART1->US_CR = US_CR_TXEN;
}

/**  \brief send char through configure USART
 *
 */
void usart_serial_putchar(const char c)
{
    while ((USART1->US_CSR & US_CSR_TXRDY) == 0)
        ;

    USART1->US_THR = c;
}

/** \brief This function take each char of string
 *  and call usart_serial_putchar function
 *
 *  \param string pointer to a string null finish
 */
void usart_write(const char* string)
{
    int i;
    i = 0;

    while (string[i] != 0) {
        usart_serial_putchar(string[i]);
        i++;

        if (i > MAXSTRINGSIZE) { return; }
    }
}

#define MAXDIGITLNG 8
/** \brief This function write the value of a hex variable
 *
 *      \param name: pointer to a string null finish
 *             value int vaue
 */
void usartHexWrite(const char* string, unsigned int value)
{
    unsigned int tempValue = value;
    unsigned int digit;
    int i;
    char hexinascci[MAXDIGITLNG];

    for (int i = 0; i < MAXDIGITLNG; i++) {
        hexinascci[i] = '0';
    }

    i = 0;

    while (string[i] != 0) {
        usart_serial_putchar(string[i]);
        i++;

        if (i > MAXSTRINGSIZE) { return; }
    }

    usart_serial_putchar('=');
    usart_serial_putchar('0');
    usart_serial_putchar('x');

    i = 0;

    while (tempValue > 0) {
        digit = tempValue % 16;
        tempValue /= 16;
        i++;

        if (i > MAXDIGITLNG) { break; }

        if (digit > 9) { hexinascci[MAXDIGITLNG - i] = 'A' + digit - 10; }
        else { hexinascci[MAXDIGITLNG - i] = '0' + digit; }
    }

    for (int i = 0; i < MAXDIGITLNG; i++) {
        usart_serial_putchar(hexinascci[i]);
    }

    usart_serial_putchar('\r');
    usart_serial_putchar('\n');
}
