/**
 * @file smartswitch.h
 * @brief 
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2024-03-04
 */

#ifndef SMARTSWITCH_H
#define SMARTSWITCH_H
#include "stdint.h"
#include "io.h"

#define MAX_WATER_TEMP_SENSORS 4u

#define APP_MAX_BUFF_LEN 512u

#define USB_CFG_MSG_LEN 18u
#define USB_CFG_SPLIT_CHAR_POS 13u

void SmartSwitch_Init(void);
void SmartSwitch_Task(void);
void SmartSwitch_SlowTask(void);
void SmartSwitch_Action(bool presence,bool button);
void SmartSwitch_ExtAction(bool button);
void SmartSwitch_flowMeter(bool q1, bool q2);
void SmartSwitch_broadcastMessage(char * const msg);
void SmartSwitch_statusMessage(char * const msg);
void SmartSwitch_newFrame(const uint8_t * msg, char * rply);
void SmartSwitch_extensionComs(void);
void SmartSwitch_cdc_tx(char *str);
void SmartSwitch_cdc_byte_ready(uint8_t port);
bool SmartSwitch_getUsbStatus(void);
uint16_t smartswitch_cfg_msg(char *msg, uint16_t len); 


#endif
