#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"
#include "EtherCard.h"
#include "enc28j60.h"

uint8_t ethmessage[ENC_MAX_BUF_SIZE];
uint16_t lastMsgType;
uint32_t g_stats_nRxFrames = 0;

#define ETH_TYPE_TCPIP 0x0800

void ethercard_parse(void);

void EtherCard_Init ( void )
{
}

void EtherCard_Task ( void )
{
    if(ENC_getnewentry(ethmessage)){
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
    g_stats_nRxFrames++;
    if( g_stats_nRxFrames > 65500){
        g_stats_nRxFrames = 65500;
    }
}
