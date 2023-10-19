#include "stdint.h"
#include "stdbool.h"
#include "EtherCard.h"
#include "tcpip.h"

void TCPIP_Init(void)
{
}

void TCPIP_Task(void)
{
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
