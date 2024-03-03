#include "stdint.h"
#include "stdbool.h"
#include "EtherCard.h"
#include "tcpip.h"
#include "arp.h"

#define ETH_PROTOCOL_UDP 17

bool linkup = false;

void TCPIP_Init(void)
{
}

void TCPIP_Task(void)
{
    static uint8_t counter = 0;
    counter++;
    if(counter > 50){
        sendUdpMessage();
        counter = 0;
    }
    linkup = isLinkUp();
}

void TCPIP_newmessage(const uint8_t * buffer)
{
    if( buffer[9] == ETH_PROTOCOL_UDP ){
        getUdpFrame(buffer);
    }
}

void getUdpFrame(const uint8_t * buffer)
{
    uint16_t srcport;
    uint16_t dstport;
    srcport = buffer[20];
    srcport <<= 8;
    srcport |= buffer[21];
    dstport = buffer[22];
    dstport <<= 8;
    dstport |= buffer[23];
    if( dstport == 12101 ) {
        FluidCtrl_newFrame(buffer+24);
    }
}

void fill_checksum(uint8_t dest, uint8_t off, uint16_t len,uint8_t type)
{
    #if 0
    const uint8_t* ptr = buffer + off;
    uint32_t sum = type==1 ? IP_PROTO_UDP_V+len-8 :
        type==2 ? IP_PROTO_TCP_V+len-8 : 0;
    while(len >1) {
        sum += (uint16_t) (((uint32_t)*ptr<<8)|*(ptr+1));
        ptr+=2;
        len-=2;
    }
    if (len)
        sum += ((uint32_t)*ptr)<<8;
    while (sum>>16)
        sum = (uint16_t) sum + (sum >> 16);
    uint16_t ck = ~ (uint16_t) sum;
    buffer[dest] = ck>>8;
    buffer[dest+1] = ck;
    #endif
}

void sendUdpMessage(void)
{
    uint8_t buffer[64];
    buffer[0] = 0xFF;
    buffer[1] = 0xFF;
    buffer[2] = 0xFF;
    buffer[3] = 0xFF;
    buffer[4] = 0xFF;
    buffer[5] = 0xFF;
    buffer[6] = 0x00;
    buffer[7] = 0xE9;
    buffer[8] = 0x3A;
    buffer[9] = 0x25;
    buffer[10] = 0xC2;
    buffer[11] = 0x29;
    buffer[12] = 0x08;
    buffer[13] = 0x00; // ETH TYPE
    /* *************** IP FRAME **************** */
    buffer[14] = 0x45; //VERSION | IHL
    buffer[15] = 0x00;    // TOS
    buffer[16] = 0x00; // TOTAL LENGTH
    buffer[17] = 0x21;
    buffer[18] = 0xD8; // IDENTIFICATION
    buffer[19] = 0x01;
    buffer[20] = 0x40;  // FLAGS | FRAGMENT OFFSET
    buffer[21] = 0x00; // FRAGMENT OFFSET
    buffer[22] = 0x40; // TTL
    buffer[23] = 17;   // PROTOCOL
    buffer[24] = 0xDE; // HEADER CHKS
    buffer[25] =  0xCE;// HEADER CHKS
    buffer[26] = 192;  // SRC ADD
    buffer[27] = 168; // SRC ADD
    buffer[28] = 1;// SRC ADD
    buffer[29] = 137;// SRC ADD
    buffer[30] =  255; // DST ADD
    buffer[31] =  255;// DST ADD
    buffer[32] =  255;// DST ADD
    buffer[33] =  255;// DST ADD
    /* **************** UDP FRAME ************* */
    buffer[34] =  0x2F;
    buffer[35] =  0x45;
    buffer[36] = 0xD3;
    buffer[37] = 0x76;
    buffer[38] = 0x00; // LENGTH
    buffer[39] =  0x0D; // LENGTH
    buffer[40] = 0x0E; // CRC
    buffer[41] =  0x78;  // CRC
    /*** MSG ****/
    buffer[42] =  'J';
    buffer[43] =  'U';
    buffer[44] =  'A';
    buffer[45] =  'N';
    buffer[46] =  0xa;
    buffer[47] =  0;
    packetSend(47, buffer);
}
