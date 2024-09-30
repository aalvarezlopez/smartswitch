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
#include "uart.h"
#include "delays.h"

bool smartswitch_roomActive = false;
uint8_t darkness_level = 0;
uint16_t radiatior_q[2] = {0, 0};
uint32_t temp_target;
bool isUsbAttached = false;

typedef struct date_s{
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} date_st;

char SmartSwitch_cdc_buffer[APP_MAX_BUFF_LEN];
char *SmartSwitch_cdc_buffer_head = 0;
char *SmartSwitch_cdc_buffer_tail = 0;

void SmartSwitch_Init(void)
{
    SmartSwitch_cdc_buffer_head = SmartSwitch_cdc_buffer;
    SmartSwitch_cdc_buffer_tail = SmartSwitch_cdc_buffer;
    IO_openRadiatorValve(0, false);
    IO_openRadiatorValve(1, false);
    IO_setLights(false);
    for(uint8_t j = 0; j < 2; j++){
        for(uint8_t i = 20; i < 100; i++){
            IO_setDimmer(i);
            delay_ms(10);
        }
        for(uint8_t i = 100; i > 20; i--){
            IO_setDimmer(i);
            delay_ms(10);
        }
    }
    IO_setDimmer(0);
}


void SmartSwitch_Task(void)
{
    static uint16_t lightsensor_mean[] = {0, 0, 0, 0, 0};
    uint8_t nsamples = 0;
    uint16_t lightsensor_raw = 0;
    uint32_t sum =0;
    smartswitch_roomActive = IO_isPIRactive();
    if ( IO_getLastAcquiredValue( SMARTSWITCH_LIGHTSENSOR_ADC_CH, &lightsensor_raw) ){
        lightsensor_mean[nsamples % 5] = lightsensor_raw;
    }
    for( uint8_t i = 0; i < 5; i++){
        if(lightsensor_mean[i] == 0){
            break;
        }
        sum += lightsensor_mean[i];
        nsamples++;
    }
    sum = nsamples > 0 ? sum / nsamples : 0;
    if( sum > 0){
        if( sum > SMARTSWITCH_DARK_100 ){
            darkness_level = 100;
        }else if (sum < SMARTSWITCH_DARK_0){
            darkness_level = 0;
        }else{
            darkness_level = ((sum - SMARTSWITCH_DARK_0) * 100) /
                             (SMARTSWITCH_DARK_100 - SMARTSWITCH_DARK_0);
        }
    }
    if( SmartSwitch_cdc_buffer_head - SmartSwitch_cdc_buffer_tail > 10){
        char str[] = "Hello";
        SmartSwitch_cdc_tx(str);
        SmartSwitch_cdc_buffer_tail = SmartSwitch_cdc_buffer_head;
    }

    SmartSwitch_extensionComs();
}

void SmartSwitch_SlowTask(void)
{
    uint8_t nsensors = 0;
    uint32_t t[MAX_WATER_TEMP_SENSORS + 1];
    date_st date;
    smartswitch_getdate(&date);

    nsensors =  DS18B20_getTempC(t);

    Display_printTime(date.hour, date.min);
    Display_printDate(date.day, date.month);
    Display_printHeat(temp_target > (t[0]/10));
    Display_printTemp( t[0] / 10 , temp_target);
    Display_refresh();

    IO_openRadiatorValve(0, (temp_target > t[0]/10));
    IO_openRadiatorValve(1, (temp_target > t[0]/10));
}

void SmartSwitch_Action(bool presence,bool button)
{
    if( presence ){
        IO_setDimmer(20);
    }else{
        IO_setDimmer(0);
    }
    if( !button ){
        IO_setLights(!IO_getLights());
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
    uint8_t nsensors = 0;
    uint32_t t[MAX_WATER_TEMP_SENSORS + 1];
    nsensors =  DS18B20_getTempC(t);
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
    int_to_str(msg + 16, t[0] / 10, 2);
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
    strcpy(msg+38, ",\nDARKNESS:");
    int_to_str(msg + 49, darkness_level , 3);
    strcpy(msg+52, ",\nSHUTTER:");
    int_to_str(msg + 62, shutter[0], 3);
    msg[65] = ';';
    int_to_str(msg + 66, shutter[1], 3);
    msg[69] = ';';
    int_to_str(msg + 70, shutter[2], 3);
    strcpy(msg+73, ",\nRTEMP:");
    for(uint8_t i = 0; i < nsensors; i++){
        int_to_str(msg + 81 + (3*i), t[i + 1] / 10, 2);
        strcpy(msg+83+ (3 * i), "#");
    }
    msg[81 + (3*nsensors)] = '}';
    msg[82 + (3*nsensors)] = '[';
    msg[83 + (3*nsensors)] = '0';
    msg[84 + (3*nsensors)] = '1';
    msg[85 + (3*nsensors)] = ']';
    msg[86 + (3*nsensors)] = '\n';
    msg[87 + (3*nsensors)] = 0;

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

void SmartSwitch_extensionComs(void)
{
    char message= "SHS0987{dimmer:100}";
    UART_tx( message, strlen(message));
}

void SmartSwitch_cdc_byte_ready(uint8_t port)
{
    if( SmartSwitch_cdc_buffer_head != 0 && port == 0){
        while (udi_cdc_is_rx_ready()) {
            *SmartSwitch_cdc_buffer_head = udi_cdc_getc();
            SmartSwitch_cdc_buffer_head++;
            if( (SmartSwitch_cdc_buffer_head - SmartSwitch_cdc_buffer) >= APP_MAX_BUFF_LEN ){
                SmartSwitch_cdc_buffer_head = SmartSwitch_cdc_buffer;
            }
        }
    }
}

void SmartSwitch_cdc_tx(char *str)
{
    udi_cdc_write_buf(str, strlen(str));
}

void SmartSwitch_cdc_suspend(void)
{
    isUsbAttached = false;
}

void SmartSwitch_cdc_resume(void)
{
    isUsbAttached = true;
}

bool SmartSwitch_getUsbStatus(void)
{
    return isUsbAttached;
}
