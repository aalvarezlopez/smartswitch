/**
 * @file smartswitch.c
 * @brief 
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2024-03-04
 */

#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "smartswitch.h"
#include "smartswitch_pri.h"
#include "smartswitch_cfg.h"
#include "rtc.h"
#include "io.h"
#include "delays.h"
#include "nvm.h"
#include "str.h"
#include "dimmer.h"

#ifndef _STATIC
#define _STATIC static
#endif

bool smartswitch_roomActive = false;
uint8_t darkness_level = 0;
uint16_t radiatior_q[2] = {0, 0};
uint8_t flow_l_min[2] = {0, 0};
uint32_t temp_target;
bool isUsbAttached = false;
_STATIC nvmSettings_st settings;

uint8_t smartswitch_log[32];
uint8_t smartswitch_logcounter = 0;

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
    Dimmer_Init();
    Dimmer_start(0, 100, 3, 1);

    if(NvM_ReadBlock( 0, sizeof(settings), 1, (uint8_t*)(&settings)) == 0){
        settings.ip[0] = 192;
        settings.ip[1] = 168;
        settings.ip[2] = 1;
        settings.ip[3] = 137;
        settings.id = 999;
        NvM_Write((uint32_t*)(&settings), 0);
    }
}


void SmartSwitch_Task(void)
{
    static uint16_t lightsensor_mean[] = {0, 0, 0, 0, 0};
    uint8_t nsamples = 0;
    uint16_t lightsensor_raw = 0;
    uint32_t sum =0;
    uint16_t result = false;
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
    if( SmartSwitch_cdc_buffer_head != SmartSwitch_cdc_buffer_tail){
        if(SmartSwitch_cdc_buffer_head > SmartSwitch_cdc_buffer_tail ){
            result = smartswitch_cfg_msg(SmartSwitch_cdc_buffer_tail,
                SmartSwitch_cdc_buffer_head - SmartSwitch_cdc_buffer_tail);
        }else{
            char temp[APP_MAX_BUFF_LEN];
            uint8_t *ptr;
            uint16_t size = APP_MAX_BUFF_LEN - ( SmartSwitch_cdc_buffer_tail - SmartSwitch_cdc_buffer);
            memcpy( temp, SmartSwitch_cdc_buffer_tail,
                size);
            memcpy( temp + size - 1,  SmartSwitch_cdc_buffer,
                (uint16_t)(SmartSwitch_cdc_buffer_head - SmartSwitch_cdc_buffer)); 
            size += (SmartSwitch_cdc_buffer_head - SmartSwitch_cdc_buffer);
            result = smartswitch_cfg_msg(temp, size);
        }
        SmartSwitch_cdc_buffer_tail += result;
        if( SmartSwitch_cdc_buffer_tail >= ( SmartSwitch_cdc_buffer + APP_MAX_BUFF_LEN)){
            SmartSwitch_cdc_buffer_tail = SmartSwitch_cdc_buffer +
                (SmartSwitch_cdc_buffer_tail - SmartSwitch_cdc_buffer + APP_MAX_BUFF_LEN);
        }
    }

    SmartSwitch_extensionComs();
    SmartSwitch_extensionComsRx();
}

void SmartSwitch_SlowTask(void)
{
    uint8_t nsensors = 0;
    uint32_t t[MAX_WATER_TEMP_SENSORS + 1];
    date_st date;
    smartswitch_getdate(&date);

    nsensors =  DS18B20_getTempC(t);

    smartswitch_calculateflow();
    Display_printTime(date.hour, date.min);
    Display_printDate(date.day, date.month);
    Display_printHeat(temp_target > (t[0]/10));
    Display_printTemp( t[0] / 10 , temp_target);
    Display_refresh();

    IO_openRadiatorValve(0, (temp_target > t[0]/10));
    IO_openRadiatorValve(1, (temp_target > t[0]/10));
}

void SmartSwitch_Action(bool presence, bool button)
{
    if( presence ){
        Dimmer_start(0, 100, 3, 1);
        smartswitch_log[smartswitch_logcounter%32] = 0xA0 | (PRESENCE);
        smartswitch_logcounter++;
    }else{
        Dimmer_start(100, 0, 3, 1);
        smartswitch_log[smartswitch_logcounter%32] = 0xB0 | (PRESENCE);
        smartswitch_logcounter++;
    }
    if( button ){
        IO_setLights(!IO_getLights());
        smartswitch_log[smartswitch_logcounter%32] = 0xC0 | (BUTTON_ACTION);
        smartswitch_logcounter++;
    }
}

void SmartSwitch_ExtAction(bool button)
{
    if( button ){
        IO_setLights(!IO_getLights());
        smartswitch_log[smartswitch_logcounter%32] = 0xD0 | (BUTTON_EXT);
        smartswitch_logcounter++;
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
    int_to_str(msg + 16, t[0], 3);
    if( rad[0] == true || rad[1] == true ){
        strcpy(msg+19, ",\nRAD: ON");
    }else{
        strcpy(msg+19, ",\nRAD:OFF");
    }
    if( IO_getLights() ){
        strcpy(msg+28, ",\nLIGHT:ON ");
    }else{
        strcpy(msg+28, ",\nLIGHT:OFF");
    }
    strcpy(msg+39, ",\nDARKNESS:");
    int_to_str(msg + 50, darkness_level , 3);
    strcpy(msg+53, ",\nSHUTTER:");
    int_to_str(msg + 63, shutter[0], 3);
    msg[66] = ';';
    int_to_str(msg + 67, shutter[1], 3);
    msg[70] = ';';
    int_to_str(msg + 71, shutter[2], 3);
    strcpy(msg+74, ",\nRFLOW:");
    int_to_str(msg + 82, flow_l_min[0], 5);
    strcpy(msg+87, "#");
    int_to_str(msg + 88 , flow_l_min[1], 5);
    strcpy(msg+93, ",\nRTEMP:");
    for(uint8_t i = 0; i < nsensors; i++){
        int_to_str(msg + 101 + (4*i), t[i + 1], 3);
        strcpy(msg+104+ (4 * i), "#");
    }
    msg[101 + (4*nsensors)] = '}';
    msg[102 + (4*nsensors)] = '[';
    msg[104 + (4*nsensors)] = '0';
    msg[105 + (4*nsensors)] = '1';
    msg[106 + (4*nsensors)] = ']';
    msg[107 + (4*nsensors)] = '\n';
    msg[108 + (4*nsensors)] = 0;
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


/**
 * @brief Read the bytes recevied and look for the valid configuration
 * pattern. First byte is '#' follow by 4 groups of 3 digits each. Next char
 * should be '$' and then another group of 3 digits. Where the first 4 groups
 * are the ip address and the last one the device Id.
 *
 * #xxxxxxxxxxxx$yyy#. Where x is the ip and y the number ID.
 *
 * Message example: #192168001137$001#
 *
 * @param msg Recevied message
 * @param len Number of received bytes
 *
 * @return 
 */
uint16_t smartswitch_cfg_msg(char *msg, uint16_t len)
{
    uint16_t result = 0;
    for(uint8_t i = 0; i < len; i++){
        if( msg[i] == '#' ){
            if( (len - i) >= USB_CFG_MSG_LEN ){
                if( msg[i+USB_CFG_SPLIT_CHAR_POS] == '$' &&
                    msg[i + USB_CFG_MSG_LEN - 1] == '#'){
                    char str[] = "ACK";
                    char decimalst[4] = {0, 0, 0, 0};
                    strncpy(decimalst, msg + i + 1, 3);
                    settings.ip[0] = __atoi(decimalst);
                    strncpy(decimalst, msg + i + 4, 3);
                    settings.ip[1] = __atoi(decimalst);
                    strncpy(decimalst, msg + i + 7, 3);
                    settings.ip[2] = __atoi(decimalst);
                    strncpy(decimalst, msg + i + 10, 3);
                    settings.ip[3] = __atoi(decimalst);
                    settings.id= __atoi(msg + i + USB_CFG_SPLIT_CHAR_POS + 1);
                    SmartSwitch_cdc_tx(str);
                    result = i + USB_CFG_MSG_LEN;
                    NvM_Write((uint32_t*)(&settings), 0);
                }
            }
        }
        if( i > 0 && msg[i-1] == '#' && msg[i] == '?'){
            char str[USB_CFG_MSG_LEN+1];
            int_to_str(str+1, settings.ip[0], 3);
            int_to_str(str+4, settings.ip[1], 3);
            int_to_str(str+7, settings.ip[2], 3);
            int_to_str(str+10, settings.ip[3], 3);
            int_to_str(str+14, settings.id, 3);
            str[13] = '$';
            str[0] = '#';
            str[17] = '#';
            str[18] = 0;
            SmartSwitch_cdc_tx(str);
            result = i + 1;
        }
    }
    return result;
}

void smartswitch_calculateflow(void)
{
    static uint16_t last_q[2];
    for(uint8_t i = 0; i < 2; i++){
        if( last_q[i] < radiatior_q[i] ){
            flow_l_min[i] = (radiatior_q[i] - last_q[i]) / PULSES_LITER;
        }
        last_q[i] = radiatior_q[i];
    }
}
