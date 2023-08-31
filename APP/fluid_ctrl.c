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
}

void FluidCtr_cdc_byte_ready(uint8_t port)
{
    if(port == 0){
        while (udi_cdc_is_rx_ready()) {
        }
    }
}
