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

extern uint32_t temp_01;

void SmartSwitch_Init(void)
{
    IO_openRadiatorValve(0, false);
    IO_openRadiatorValve(1, false);
    IO_setLights(true);
}


void SmartSwitch_Task(void)
{
    smartswitch_roomActive = IO_isPIRactive();
}

void SmartSwitch_SlowTask(void)
{
    uint8_t year_bcd, month_bcd, day_bcd;
    uint8_t hour_bcd, min_bcd, sec_bcd;
    uint8_t month, day;
    uint8_t hour, min;
    uint32_t status;
    Rtc_getTimeDate(&year_bcd, &month_bcd, &day_bcd);
    Rtc_getTimeHour(&hour_bcd, &min_bcd, &sec_bcd);
    BCD_TO_INT(month_bcd, month);
    BCD_TO_INT(day_bcd, day);
    BCD_TO_INT(hour_bcd, hour);
    BCD_TO_INT(min_bcd, min);

    Display_printTime(hour, min);
    Display_printDate(day, month);
    Display_printHeat(true);
    Display_printTemp( temp_01 / 10 , 29);
    Display_refresh();
    IO_openRadiatorValve(0, false);
    IO_openRadiatorValve(1, false);
}

void FluidCtr_cdc_byte_ready(uint8_t port)
{
}

void SmartSwitch_newFrame(const uint8_t * msg)
{
}
