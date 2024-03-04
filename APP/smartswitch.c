/**
 * @file smartswitch.c
 * @brief 
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2024-03-04
 */

#include "stdio.h"
#include "stdbool.h"
#include "smartswitch.h"
#include "smartswitch_pri.h"
#include "smartswitch_cfg.h"
#include "rtc.h"
#include "io.h"
#include "str.h"

bool smartswitch_roomActive = false;

void SmartSwitch_Init(void)
{
}


void SmartSwitch_Task(void)
{
    smartswitch_roomActive = IO_isPIRactive();
}

void FluidCtr_cdc_byte_ready(uint8_t port)
{
}

void SmartSwitch_newFrame(const uint8_t * msg)
{
}
