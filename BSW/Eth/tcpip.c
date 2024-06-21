#include "stdint.h"
#include "stdbool.h"
#include "EtherCard.h"
#include "tcpip.h"
#include "arp.h"

#define ETH_PROTOCOL_UDP 17
#define DST_ADD 254

bool linkup = false;
uint32_t ipv4_id = 0x1489;

void TCPIP_Init(void)
{
    char ip[] = {192, 168, 1, 1};
    ARP_sendrequest(ip);
}

void TCPIP_Task(void)
{
    static uint32_t counter = 0;
    counter++;
    if(counter > 100){
        char msg[256];
        SmartSwitch_broadcastMessage(msg);
        broadcastUdpMessage(msg, strlen(msg)+1, 12101, 54134);
        if( !arp_isResolved(1)){
            char ip[] = {192, 168, 1, 1};
            ARP_sendrequest(ip);
        }else if( !arp_isResolved(DST_ADD)){
            char ip[] = {192, 168, 1, DST_ADD};
            ARP_sendrequest(ip);
        }
        counter = 0;
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
    if( dstport == 54135 ) {
        char rply[256];
        SmartSwitch_newFrame(buffer+28, rply);
        sendUdpMessage(DST_ADD, rply, strlen(rply)+1, 12101, 54135);
    }
}

uint16_t chksum(uint16_t sum, const uint8_t *data, uint16_t len)
{
  uint16_t t;
  const uint8_t *dataptr;
  const uint8_t *last_byte;

  dataptr = data;
  last_byte = data + len - 1;

  while(dataptr < last_byte) {
    t = (dataptr[0] << 8) + dataptr[1];
    sum += t;
    if(sum < t) {
      sum++;
    }
    dataptr += 2;
  }

  if(dataptr == last_byte) {
    t = (dataptr[0] << 8) + 0;
    sum += t;
    if(sum < t) {
      sum++;
    }
  }

  return sum;
}

void sendUdpMessage(uint8_t ip, uint8_t *msg, uint16_t len, uint16_t dstprt, uint16_t srcprt)
{
    uint16_t crc;
    uint8_t buffer[512];
    uint8_t pseudo_header[12];
    uint16_t total_length = 20 + 8 + len;
    uint16_t udp_length = 8 + len;
    /* DST MAC */
    if(arp_getMAC(ip, buffer)){
        /* SRC MAC */
        buffer[6] = 0x00; buffer[7]  = 0xE9; buffer[8]  = 0x3A;
        buffer[9] = 0x25; buffer[10] = 0xC2; buffer[11] = 0x29;
        /* ETH TYPE - IPv4 */
        buffer[12] = 0x08;
        buffer[13] = 0x00;
        /* *************** IP FRAME **************** */
        /* VERSION | IHL | TOS */
        buffer[14] = 0x45; buffer[15] = 0x00;
        /* TOTAL LENGTH */
        buffer[16] = total_length >> 8; buffer[17] = (total_length & 0xFF);
        /* UNIQUE ID */
        buffer[18] = ipv4_id >> 8;
        buffer[19] = ipv4_id & 0xFF;
        /* FLAGS | FRAGMENT OFFSET | TTL | PROTOCOL (UDP) */
        buffer[20] = 0x40; buffer[21] = 0x00; buffer[22] = 0x40; buffer[23] = 17;
        /* HEADER CHECKSUM */
        buffer[24] = 0x00;
        buffer[25] = 0x00;
        /* SRC ADDRESS */
        buffer[26] = 192; buffer[27] = 168; buffer[28] = 1; buffer[29] = MYIP;
        /* DST ADDRESS */
        buffer[30] = 192; buffer[31] = 168; buffer[32] = 1; buffer[33] = ip;
        /* **************** UDP FRAME ************* */
        buffer[34] = srcprt >> 8; buffer[35] = srcprt & 0xFF;
        buffer[36] = dstprt >> 8; buffer[37] = dstprt & 0xFF;
        buffer[38] = (udp_length) >> 8;
        buffer[39] = (udp_length) & 0xFF;
        /* CRC */
        buffer[40] = 0x00; buffer[41] = 0x00;
        /*** MSG ****/
        memcpy( buffer + 42, msg, len);
        crc =  chksum( 0, buffer + 14, 20);
        crc = ~crc;
        buffer[24] = crc >> 8;
        buffer[25] = crc & 0xFF;

        memcpy(pseudo_header, buffer + 26, 8);
        pseudo_header[8] = 0; pseudo_header[9] = 0x11;
        pseudo_header[10] = (udp_length >> 8); pseudo_header[11] = (udp_length) & 0xFF;
        crc =  chksum( 0, pseudo_header, 12);
        crc =  chksum( crc, buffer + 34, 8 + len);
        crc = ~crc;
        buffer[40] = crc >> 8;
        buffer[41] = crc & 0xFF;
        packetSend(42 + len, buffer);

        ipv4_id = ipv4_id + len;
    }
}

void broadcastUdpMessage(uint8_t *msg, uint16_t len, uint16_t dstprt, uint16_t srcprt)
{
    uint16_t crc;
    uint8_t buffer[512];
    uint8_t pseudo_header[12];
    uint16_t total_length = 20 + 8 + len;
    uint16_t udp_length = 8 + len;
    /* DST MAC */
    buffer[0] = 0xFF; buffer[1] = 0xFF; buffer[2] = 0xFF;
    buffer[3] = 0xFF; buffer[4] = 0xFF; buffer[5] = 0xFF;
    /* SRC MAC */
    buffer[6] = 0x00; buffer[7]  = 0xE9; buffer[8]  = 0x3A;
    buffer[9] = 0x25; buffer[10] = 0xC2; buffer[11] = 0x29;
    /* ETH TYPE - IPv4 */
    buffer[12] = 0x08;
    buffer[13] = 0x00;
    /* *************** IP FRAME **************** */
    /* VERSION | IHL | TOS */
    buffer[14] = 0x45; buffer[15] = 0x00;
    /* TOTAL LENGTH */
    buffer[16] = total_length >> 8; buffer[17] = (total_length & 0xFF);
    /* UNIQUE ID */
    buffer[18] = ipv4_id >> 8;
    buffer[19] = ipv4_id & 0xFF;
    /* FLAGS | FRAGMENT OFFSET | TTL | PROTOCOL (UDP) */
    buffer[20] = 0x40; buffer[21] = 0x00; buffer[22] = 0x40; buffer[23] = 17;
    /* HEADER CHECKSUM */
    buffer[24] = 0x00;
    buffer[25] = 0x00;
    /* SRC ADDRESS */
    buffer[26] = 192; buffer[27] = 168; buffer[28] = 1; buffer[29] = MYIP;
    /* DST ADDRESS */
    buffer[30] = 255; buffer[31] = 255; buffer[32] = 255; buffer[33] = 255;
    /* **************** UDP FRAME ************* */
    buffer[34] = srcprt >> 8; buffer[35] = srcprt & 0xFF;
    buffer[36] = dstprt >> 8; buffer[37] = dstprt & 0xFF;
    buffer[38] = (udp_length) >> 8;
    buffer[39] = (udp_length) & 0xFF;
    /* CRC */
    buffer[40] = 0x00; buffer[41] = 0x00;
    /*** MSG ****/
    memcpy( buffer + 42, msg, len);
    crc =  chksum( 0, buffer + 14, 20);
    crc = ~crc;
    buffer[24] = crc >> 8;
    buffer[25] = crc & 0xFF;

    memcpy(pseudo_header, buffer + 26, 8);
    pseudo_header[8] = 0; pseudo_header[9] = 0x11;
    pseudo_header[10] = (udp_length >> 8); pseudo_header[11] = (udp_length) & 0xFF;
    crc =  chksum( 0, pseudo_header, 12);
    crc =  chksum( crc, buffer + 34, 8 + len);
    crc = ~crc;
    buffer[40] = crc >> 8;
    buffer[41] = crc & 0xFF;
    packetSend(42 + len, buffer);

    ipv4_id = ipv4_id + len;
}
