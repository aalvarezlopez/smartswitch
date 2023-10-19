#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"
#include "EtherCard.h"
#include "enc28j60.h"

uint8_t ethmessage[ENC_MAX_BUF_SIZE];
uint16_t lastMsgType;
uint32_t g_stats_nRxFrames = 0;
uint16_t srcport, dstport;
uint32_t dhcpmessage = 0;
uint32_t appmessage = 0;

#define ETH_TYPE_TCPIP 0x0800
#define ETH_PROTOCOL_UDP 17

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
        if( ethmessage[23] = ETH_PROTOCOL_UDP ){
            srcport = ethmessage[34];
            srcport <<= 8;
            srcport |= ethmessage[35];
            dstport = ethmessage[36];
            dstport <<= 8;
            dstport |= ethmessage[37];
            if( (srcport == 67 || srcport == 68) &&
                (dstport == 67 || dstport == 68) ){
                dhcpmessage  = ethmessage[42];
                dhcpmessage <<= 8;
                dhcpmessage  = ethmessage[43];
                dhcpmessage <<= 8;
                dhcpmessage  = ethmessage[44];
                dhcpmessage <<= 8;
                dhcpmessage  = ethmessage[45];
            } else if( dstport == 12101 ) {
                appmessage  = ethmessage[42];
                appmessage <<= 8;
                appmessage  = ethmessage[43];
                appmessage <<= 8;
                appmessage  = ethmessage[44];
                appmessage <<= 8;
                appmessage  = ethmessage[45];
            }
        }
    }

    g_stats_nRxFrames++;
    if( g_stats_nRxFrames > 65500){
        g_stats_nRxFrames = 65500;
    }
}
