/**
 * @file enc28j60.c
 * @brief 
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2023-10-06
 */

#include "stdint.h"
#include "enc28j60.h"

#include "spi.h"

uint8_t eth_din[] = {0x11, 0xA5};
uint8_t eth_dout[10];

void ENC_Init(void)
{
}

void ENC_Task(void)
{
    SPI_sync_transmission(2, eth_din, eth_dout);
}
