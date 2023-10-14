/**
 * @file fluid_ctrl.c
 * @brief FluidControl logic which control the pulser as well as
 * the electrovalve. This module also handle what to show at the screen and
 * user interface through the buttons
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-14
 */

#include "stdio.h"
#include "stdbool.h"
#include "fluid_ctrl.h"
#include "fluid_ctrl_pri.h"
#include "fluid_ctrl_cfg.h"
#include "rtc.h"
#include "io.h"
#include "str.h"

void FluidCtrl_Init(void)
{
}


void FluidCtrl_Task(void)
{
    #if 0
    uint8_t ip[4];
    ip[0] = 192;
    ip[0] = 168;
    ip[0] = 3;
    ip[0] = 36;
    client_arp_whohas(ip);
    #else
    uint8_t data[10];
    uint8_t ip[4];
    ip[0] = 192;
    ip[1] = 168;
    ip[2] = 1;
    ip[3] = 36;
    data[0] = 0xDE;
    data[1] = 0xAD;
    data[2] = 0xBE;
    data[3] = 0xEF;
    data[4] = 0xBA;
    data[5] = 0xDC;
    data[6] = 0x0F;
    data[7] = 0xFE;
    data[8] = 0xAA;
    data[9] = 0x55;
    sendUdp(data,10, 12300, ip , 12300 );
    #endif
}

void FluidCtr_cdc_byte_ready(uint8_t port)
{
}
