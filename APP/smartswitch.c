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
uint16_t radiatior_q[2] = {0, 0};
extern uint32_t temp_01;
uint32_t temp_target;

typedef struct date_s{
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} date_st;

void SmartSwitch_Init(void)
{
    IO_openRadiatorValve(0, false);
    IO_openRadiatorValve(1, false);
    IO_setLights(false);
    IO_setDimmer(0);
}


void SmartSwitch_Task(void)
{
    smartswitch_roomActive = IO_isPIRactive();
}

void SmartSwitch_SlowTask(void)
{
    date_st date;
    smartswitch_getdate(&date);

    Display_printTime(date.hour, date.min);
    Display_printDate(date.day, date.month);
    Display_printHeat(true);
    Display_printTemp( temp_01 / 10 , temp_target);
    Display_refresh();
    IO_openRadiatorValve(0, false);
    IO_openRadiatorValve(1, false);
}

void SmartSwitch_Action(bool presence,bool button)
{
    if( presence ){
        IO_setDimmer(20);
    }else{
        IO_setDimmer(0);
    }
    if( button ){
        IO_setLights(true);
    }
}

void SmartSwitch_flowMeter(bool q1, bool q2)
{
    if( q1 == false){
        radiatior_q[0]++;
    }
    if( q2 == false){
        radiatior_q[1]++;
    }
}

void SmartSwitch_newFrame(const uint8_t * msg, char * rply)
{
    char * cmd;
    cmd = strstr(msg, "LIGHT:");
    strcpy(rply, "NACK");
    if((cmd != NULL) && strlen(msg)>strlen("LIGHT:")){
        char * end;
        end = strstr(cmd, "ON");
        IO_setLights(end != NULL);
        strcpy(rply, "ACK");
    }
    cmd = strstr(msg, "TTARGET:");
    if((cmd != NULL) && strlen(msg)>strlen("TTARGET:")){
        uint32_t value = atol(cmd+strlen("TTARGET:"));
        temp_target = value;
        strcpy(rply, "ACK");
    }
    cmd = strstr(msg, "SHUTTER:");
    if((cmd != NULL) && strlen(msg)>strlen("SHUTTER:")){
        uint32_t value = atol(cmd+strlen("SHUTTER:"));
        IO_setShutterPosition(0, value);
        IO_setShutterPosition(1, value);
        IO_setShutterPosition(2, value);
        strcpy(rply, "ACK");
    }
    cmd = strstr(msg, "STATUS");
    if((cmd != NULL) && strlen(msg)>strlen("STATUS")){
        SmartSwitch_statusMessage(rply);
    }
    cmd = strstr(msg, "DATE:");
    if((cmd != NULL) && strlen(msg)>strlen("DATE:")){
        uint8_t hour, min, sec;
        uint8_t year, month, day;
        uint32_t value = atol(cmd+strlen("DATE:"));
        year = value;

        cmd = strstr(cmd, "/");
        cmd++;
        value = atol(cmd);
        month = value;

        cmd = strstr(cmd, "/");
        cmd++;
        value = atol(cmd);
        day = value;

        cmd = strstr(cmd, "/");
        cmd++;
        value = atol(cmd);
        hour = value;

        cmd = strstr(cmd, "/");
        cmd++;
        value = atol(cmd);
        min = value;

        cmd = strstr(cmd, "/");
        cmd++;
        value = atol(cmd);
        sec = value;
        Rtc_setTimeDate(year, month, day, hour, min, sec);
    }
}

void SmartSwitch_statusMessage(char * const msg)
{
    bool rad[2];
    date_st date;
    uint8_t shutter[3];
    smartswitch_getdate(&date);
    IO_getRadiatorState(rad);
    IO_getShutterPosition(shutter);
    msg[0] = '[';
    int_to_str(msg + 1, date.hour, 2);
    msg[3] = ':';
    int_to_str(msg + 4, date.min, 2);
    msg[6] = ':';
    int_to_str(msg + 7, date.sec, 2);
    strcpy(msg+9, "]{TEMP:");
    int_to_str(msg + 16, temp_01 / 10, 2);
    if( rad[0] == true || rad[1] == true ){
        strcpy(msg+18, ",\nRAD: ON");
    }else{
        strcpy(msg+18, ",\nRAD:OFF");
    }
    if( IO_getLights() ){
        strcpy(msg+27, ",\nLIGHT:ON ");
    }else{
        strcpy(msg+27, ",\nLIGHT:OFF");
    }
    strcpy(msg+38, ",\nSHUTTER:");
    int_to_str(msg + 48, shutter[0], 3);
    msg[51] = ';';
    int_to_str(msg + 52, shutter[1], 3);
    msg[55] = ';';
    int_to_str(msg + 56, shutter[2], 3);
    msg[59] = '}';
    msg[60] = '\n';
    msg[61] = 0;

}

void SmartSwitch_broadcastMessage(char * const msg)
{
    SmartSwitch_statusMessage(msg);
}

void smartswitch_getdate(date_st * date)
{
    uint8_t year_bcd, month_bcd, day_bcd;
    uint8_t hour_bcd, min_bcd, sec_bcd;
    Rtc_getTimeDate(&year_bcd, &month_bcd, &day_bcd);
    Rtc_getTimeHour(&hour_bcd, &min_bcd, &sec_bcd);
    BCD_TO_INT(month_bcd, date->month);
    BCD_TO_INT(day_bcd, date->day);
    BCD_TO_INT(hour_bcd, date->hour);
    BCD_TO_INT(min_bcd, date->min);
    BCD_TO_INT(sec_bcd, date->sec);
}
