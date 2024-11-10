#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"
#include "EtherCard.h"
#include "enc28j60.h"

uint8_t ethmessage[ENC_MAX_BUF_SIZE];
uint16_t lastMsgType;
uint32_t g_stats_nRxFrames = 0;
static uint8_t ethercard_lowestIpAddr = 0;

#define ETH_TYPE_TCPIP 0x0800
#define ETH_TYPE_ARP   0x0806


void ethercard_parse(void);

void EtherCard_Init ( void )
{
    ethercard_lowestIpAddr = SmartSwitch_getLowestIpAddr();
}

void EtherCard_Task ( void )
{
    while(ENC_getnewentry(ethmessage)){
        ethercard_parse();
    }
}

void ethercard_parse(void)
{
    lastMsgType = ethmessage[12];
    lastMsgType <<= 8;
    lastMsgType |= ethmessage[13];

    if( lastMsgType == ETH_TYPE_TCPIP ){
        TCPIP_newmessage(ethmessage + 14);
    }
    if( lastMsgType == ETH_TYPE_ARP ){
        if( ethmessage[38] == 192 &&
            ethmessage[39] == 168 &&
            ethmessage[40] == 1 &&
            ethmessage[41] == ethercard_lowestIpAddr){
            ARP_sendreplyrouter(ethmessage+28, ethmessage+22);
        }
    }
    g_stats_nRxFrames++;
    if( g_stats_nRxFrames > 65500){
        g_stats_nRxFrames = 65500;
    }
}
