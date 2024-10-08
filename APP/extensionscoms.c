/**
 * @file extensioncoms.c
 * @brief 
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2024-10-05
 */

#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "uart.h"
#include "extensionscoms.h"

static uint8_t currentId = 0;

void SmartSwitch_extensionComs(void)
{
    uint8_t rxcounter;
    char rxmsg[64];
    char message[]= "SHS090x{dimmer:100}";
    message[6] = currentId + '1';
    UART_tx( message, strlen(message));
    rxcounter = UART_flush(rxmsg);
    currentId = currentId < (SHS_MAX_SATELLITES - 1) ?
        (currentId + 1) : 0;
}

void SmartSwitch_extensionsComs_Init(void)
{
    currentId = 0;
}

void SmartSwitch_extensionComsRx(void)
{
    char rxmsg[32];
    if(UART_flush(rxmsg)){
        SmartSwitch_extensionComs_Parser(rxmsg);
    }
}

void SmartSwitch_extensionComs_Parser(char *msg)
{
    char *ptr = strstr(msg, "SHS09");
    if( ptr != 0){
        uint8_t satId = (uint8_t)*(ptr+5) - '0';
        if(strstr(ptr, "}")){
            if( *(ptr +  7) == 'T' &&
                *(ptr +  8) == 'R' &&
                *(ptr +  9) == 'U' &&
                *(ptr + 10) == 'E'){
                SmartSwitch_Action(false, true);
            }
        }
    }
}
