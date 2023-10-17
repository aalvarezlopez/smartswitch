/**
 * @file enc28j60.c
 * @brief
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2023-10-06
 */

#include "stdint.h"
#include "stdbool.h"
#include "enc28j60_pri.h"
#include "enc28j60.h"

#include "spi.h"
#include "delays.h"

uint8_t buffer[2048]; //!< Data buffer (shared by receive and transmit)
uint16_t bufferSize; //!< Size of data buffer
bool broadcast_enabled; //!< True if broadcasts enabled (used to allow temporary disable of broadcast for DHCP or other internal functions)
bool promiscuous_enabled; //!< True if promiscuous mode enabled (used to allow temporary disable of promiscuous mode)
uint16_t _lastReadBuflen = 0;

void ENC_Init(void)
{
    uint32_t mac[6] = { 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F};
    EtherCard_begin(512, mac);
    dhcpSetup();
}

void ENC_Task(void)
{
}

 uint32_t Enc28j60Bank;
 uint32_t selectPin;


 uint8_t readOp (uint32_t op, uint32_t address)
{
    uint8_t result;
    uint8_t spicmd_tx[2];
    uint8_t spicmd_rx[2];
    if (address & 0x80) {
        spicmd_tx[0] = op | ((address) & ADDR_MASK);
        spicmd_tx[1] = 0x00;
        spicmd_tx[2] = 0x00;
        SPI_sync_transmission(3, spicmd_tx, spicmd_rx);
        result = spicmd_rx[2];
    }else{
        spicmd_tx[0] = op | (address & ADDR_MASK);
        spicmd_tx[1] = 0x00;
        SPI_sync_transmission(2, spicmd_tx, spicmd_rx);
        result = spicmd_rx[1];
    }
    return result;
}

 void writeOp (uint32_t op, uint32_t address, uint32_t data)
{
    uint8_t spicmd_tx[2];
    uint8_t spicmd_rx[2];
    spicmd_tx[0] = op | (address & ADDR_MASK);
    spicmd_tx[1] = (uint8_t)data;
    SPI_sync_transmission(2, spicmd_tx, spicmd_rx);
}

void readBuf(uint16_t len, uint8_t *data)
{
    _lastReadBuflen = len;
    uint8_t spicmd_tx[16];
    for(uint16_t i = 0; i < len; i+=15){
        spicmd_tx[0] = ENC28J60_READ_BUF_MEM;
        spicmd_tx[1] = 0x00;
        SPI_sync_transmission(16, spicmd_tx, (uint8_t*)(data + i ));
        for(uint8_t j = 0; j < 15; j++){
            data[ i + j ] = data[ i + j + 1];
        }
        delay_us(100);
    }
}

 void writeBuf(uint16_t len, const uint8_t *data)
{
    uint8_t spicmd_tx[16];
    uint8_t spicmd_rx[16];
    for(uint16_t i = 0; i < len; i+=15){
        spicmd_tx[0] = ENC28J60_WRITE_BUF_MEM;
        memcpy( spicmd_tx + 1, data + i, 15);
        SPI_sync_transmission(16, spicmd_tx, spicmd_rx);
    }
}

 void SetBank (uint32_t address)
{
    if ((address & BANK_MASK) != Enc28j60Bank) {
        writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_BSEL1 | ECON1_BSEL0);
        Enc28j60Bank = address & BANK_MASK;
        writeOp(ENC28J60_BIT_FIELD_SET, ECON1, Enc28j60Bank >> 5);
    }
}

 uint32_t readRegByte (uint32_t address)
{
    SetBank(address);
    return readOp(ENC28J60_READ_CTRL_REG, address);
}

 uint16_t readReg(uint32_t address)
{
    return readRegByte(address) + (readRegByte(address + 1) << 8);
}

 void writeRegByte (uint32_t address, uint32_t data)
{
    SetBank(address);
    writeOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

 void writeReg(uint32_t address, uint16_t data)
{
    writeRegByte(address, data);
    writeRegByte(address + 1, data >> 8);
}

 uint16_t readPhyByte (uint32_t address)
{
    writeRegByte(MIREGADR, address);
    writeRegByte(MICMD, MICMD_MIIRD);
    while (readRegByte(MISTAT) & MISTAT_BUSY){ continue;}
    writeRegByte(MICMD, 0x00);
    return readRegByte(MIRD + 1);
}

 void writePhy (uint32_t address, uint16_t data)
{
    writeRegByte(MIREGADR, address);
    writeReg(MIWR, data);
    while (readRegByte(MISTAT) & MISTAT_BUSY)
        ;
}

uint32_t initialize (uint16_t size, const uint32_t *macaddr)
{
    bufferSize = size;
    writeOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
    delay_ms(4); // errata B7/2
    while (!(readOp(ENC28J60_READ_CTRL_REG, ESTAT) & ESTAT_CLKRDY)){continue;}
    writeReg(ERXST, RXSTART_INIT);
    writeReg(ERXRDPT, RXSTART_INIT);
    writeReg(ERXND, RXSTOP_INIT);
    writeReg(ETXST, TXSTART_INIT);
    writeReg(ETXND, TXSTOP_INIT);
    // Stretch pulses for LED, LED_A=Link, LED_B=activity
    writePhy(PHLCON, 0x476);
    writeRegByte(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN |
                 ERXFCON_BCEN);
    writeReg(EPMM0, 0x303f);
    writeReg(EPMCS, 0xf7f9);
    writeRegByte(MACON1, MACON1_MARXEN);
    writeOp(ENC28J60_BIT_FIELD_SET, MACON3,
            MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);
    writeReg(MAIPG, 0x0C12);
    writeRegByte(MABBIPG, 0x12);
    writeReg(MAMXFL, MAX_FRAMELEN);
    writeRegByte(MAADR5, macaddr[0]);
    writeRegByte(MAADR4, macaddr[1]);
    writeRegByte(MAADR3, macaddr[2]);
    writeRegByte(MAADR2, macaddr[3]);
    writeRegByte(MAADR1, macaddr[4]);
    writeRegByte(MAADR0, macaddr[5]);
    readRegByte(MAADR5);
    readRegByte(MAADR4);
    readRegByte(MAADR3);
    readRegByte(MAADR2);
    readRegByte(MAADR1);
    readRegByte(MAADR0);
    writePhy(PHCON2, PHCON2_HDLDIS);
    SetBank(ECON1);
    writeOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE);
    writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
    uint32_t rev = readRegByte(EREVID);
    // microchip forgot to step the number on the silicon when they
    // released the revision B7. 6 is now rev B7. We still have
    // to see what they do when they release B8. At the moment
    // there is no B8 out yet
    if (rev > 5) { ++rev; }
    return rev;
}

bool isLinkUp()
{
    uint8_t result = readPhyByte(PHSTAT2);
    volatile bool isup = (result & 0x04) != 0;
    return isup;
}

/*
struct __attribute__((__packed__)) transmit_status_vector {
    uint16_t transmitByteCount;
    uint32_t     transmitCollisionCount      :  4;
    uint32_t     transmitCrcError            :  1;
    uint32_t     transmitLengthCheckError    :  1;
    uint32_t     transmitLengthOutRangeError :  1;
    uint32_t     transmitDone                :  1;
    uint32_t     transmitMulticast           :  1;
    uint32_t     transmitBroadcast           :  1;
    uint32_t     transmitPacketDefer         :  1;
    uint32_t     transmitExcessiveDefer      :  1;
    uint32_t     transmitExcessiveCollision  :  1;
    uint32_t     transmitLateCollision       :  1;
    uint32_t     transmitGiant               :  1;
    uint32_t     transmitUnderrun            :  1;
    uint16_t totalTransmitted;
    uint32_t     transmitControlFrame        :  1;
    uint32_t     transmitPauseControlFrame   :  1;
    uint32_t     backpressureApplied         :  1;
    uint32_t     transmitVLAN                :  1;
    uint32_t     zero                        :  4;
};
*/

struct transmit_status_vector {
    uint32_t uint32_ts[7];
};

#if ETHERCARD_SEND_PIPELINING
    #define BREAKORCONTINUE retry=0; continue;
#else
    #define BREAKORCONTINUE break;
#endif

void packetSend(uint16_t len)
{
    uint32_t retry = 0;
    uint32_t eir_stat = readRegByte(ESTAT);
    while (1) {
        // latest errata sheet: DS80349C
        // always reset transmit logic (Errata Issue 12)
        // the Microchip TCP/IP stack implementation used to first check
        // whether TXERIF is set and only then reset the transmit logic
        // but this has been changed in later versions; possibly they
        // have a reason for this; they don't mention this in the errata
        // sheet
        writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
        writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
        writeOp(ENC28J60_BIT_FIELD_CLR, EIR, EIR_TXERIF | EIR_TXIF);
        // prepare new transmission
        if (retry == 0) {
            writeReg(EWRPT, TXSTART_INIT);
            writeReg(ETXND, TXSTART_INIT + len + 1);
            writeOp(ENC28J60_WRITE_BUF_MEM, 0, 0x02);
            eir_stat = readRegByte(ESTAT);
            writeBuf(len, buffer);
            eir_stat = readRegByte(ESTAT);
        }
        // initiate transmission
        writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
#if ETHERCARD_SEND_PIPELINING
        if (retry == 0) { return; }
#endif
        // wait until transmission has finished; referring to the data sheet and
        // to the errata (Errata Issue 13; Example 1) you only need to wait until either
        // TXIF or TXERIF gets set; however this leads to hangs; apparently Microchip
        // realized this and in later implementations of their tcp/ip stack they introduced
        // a counter to avoid hangs; of course they didn't update the errata sheet
        uint16_t count = 0;
        uint32_t eir_reg = readRegByte(EIR);
        while ((eir_reg & (EIR_TXIF | EIR_TXERIF)) == 0){
            count++;
            delay_us(10);
            if( count > 1000){
                // cancel previous transmission if stuck
                writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
                // Check whether the chip thinks that a late collision occurred; the chip
                // may be wrong (Errata Issue 13); therefore we retry. We could check
                // LATECOL in the ESTAT register in order to find out whether the chip
                // thinks a late collision occurred but (Errata Issue 15) tells us that
                // this is not working. Therefore we check TSV
                struct transmit_status_vector tsv;
                uint16_t etxnd = readReg(ETXND);
                writeReg(ERDPT, etxnd + 1);
                readBuf(sizeof(tsv), (uint8_t *) &tsv);
                // LATECOL is bit number 29 in TSV (starting from 0)
                if (!((readRegByte(EIR) & EIR_TXERIF) &&
                        (tsv.uint32_ts[3] & 1 << 5) /*tsv.transmitLateCollision*/) || retry > 16U) {
                    // there was some error but no LATECOL so we do not repeat
                    break;
                }
                retry++;
                break;
            }
            eir_reg = readRegByte(EIR);
            eir_stat = readRegByte(ESTAT);
        }
        if ((eir_reg & EIR_TXIF ) != 0){
            break;
        }
    }
}


uint16_t packetReceive()
{
    static uint16_t gNextPacketPtr = RXSTART_INIT;
    static bool     unreleasedPacket = false;
    uint16_t len = 0;
    if (unreleasedPacket) {
        if (gNextPacketPtr == 0) {
            writeReg(ERXRDPT, RXSTOP_INIT);
        } else {
            writeReg(ERXRDPT, gNextPacketPtr - 1);
        }
        unreleasedPacket = false;
    }
    if (readRegByte(EPKTCNT) > 0) {
        writeReg(ERDPT, gNextPacketPtr);
        struct  {
            uint16_t nextPacket;
            uint16_t uint32_tCount;
            uint16_t status;
        } header;
        readBuf(sizeof header, (uint8_t *) &header);
        gNextPacketPtr  = header.nextPacket;
        len = header.uint32_tCount - 4; //remove the CRC count
        if (len > bufferSize - 1) {
            len = bufferSize - 1;
        }
        if ((header.status & 0x80) == 0) {
            len = 0;
        } else {
            readBuf(len, buffer);
        }
        buffer[len] = 0;
        unreleasedPacket = true;
        writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
    }
    return len;
}

uint32_t peekin (uint32_t page, uint32_t off)
{
    uint32_t result = 0;
    uint16_t destPos = SCRATCH_START + (page << SCRATCH_PAGE_SHIFT) + off;
    if (SCRATCH_START <= destPos && destPos < SCRATCH_LIMIT) {
        writeReg(ERDPT, destPos);
        readBuf(1, &result);
    }
    return result;
}

// Contributed by Alex M. Based on code from: http://blog.derouineau.fr
//                  /2011/07/putting-enc28j60-ethernet-controler-in-sleep-mode/
void powerDown()
{
    writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_RXEN);
    while (readRegByte(ESTAT) & ESTAT_RXBUSY);
    while (readRegByte(ECON1) & ECON1_TXRTS);
    writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_VRPS);
    writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PWRSV);
}

void powerUp()
{
    writeOp(ENC28J60_BIT_FIELD_CLR, ECON2, ECON2_PWRSV);
    while (!(readRegByte(ESTAT) & ESTAT_CLKRDY));
    writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
}

void enableBroadcast (bool temporary)
{
    writeRegByte(ERXFCON, readRegByte(ERXFCON) | ERXFCON_BCEN);
    if (!temporary) {
        broadcast_enabled = true;
    }
}

void disableBroadcast (bool temporary)
{
    if (!temporary) {
        broadcast_enabled = false;
    }
    if (!broadcast_enabled) {
        writeRegByte(ERXFCON, readRegByte(ERXFCON) & ~ERXFCON_BCEN);
    }
}

void enableMulticast ()
{
    writeRegByte(ERXFCON, readRegByte(ERXFCON) | ERXFCON_MCEN);
}

void disableMulticast ()
{
    writeRegByte(ERXFCON, readRegByte(ERXFCON) & ~ERXFCON_MCEN);
}

void enablePromiscuous (bool temporary)
{
    writeRegByte(ERXFCON, readRegByte(ERXFCON) & ERXFCON_CRCEN);
    if (!temporary) {
        promiscuous_enabled = true;
    }
}

void disablePromiscuous (bool temporary)
{
    if (!temporary) {
        promiscuous_enabled = false;
    }
    if (!promiscuous_enabled) {
        writeRegByte(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN |
            ERXFCON_BCEN);
    }
}

uint32_t doBIST ( uint32_t csPin)
{
 #define RANDOM_FILL     0b0000
 #define ADDRESS_FILL    0b0100
 #define PATTERN_SHIFT   0b1000
 #define RANDOM_RACE     0b1100
    // init
    selectPin = csPin;
    writeOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
    delay_ms(2); // errata B7/2
    while (!(readOp(ENC28J60_READ_CTRL_REG, ESTAT) & ESTAT_CLKRDY)) ;
    // now we can start the memory test
    uint16_t macResult;
    uint16_t bitsResult;
    // clear some of the registers registers
    writeRegByte(ECON1, 0);
    writeReg(EDMAST, 0);
    // Set up necessary pointers for the DMA to calculate over the entire memory
    writeReg(EDMAND, 0x1FFFu);
    writeReg(ERXND, 0x1FFFu);
    // Enable Test Mode and do an Address Fill
    SetBank(EBSTCON);
    writeRegByte(EBSTCON, EBSTCON_TME | EBSTCON_BISTST | ADDRESS_FILL);
    // wait for BISTST to be reset, only after that are we actually ready to
    // start the test
    // this was undocumented :(
    while (readOp(ENC28J60_READ_CTRL_REG, EBSTCON) & EBSTCON_BISTST);
    writeOp(ENC28J60_BIT_FIELD_CLR, EBSTCON, EBSTCON_TME);
    // now start the actual reading an calculating the checksum until the end is
    // reached
    writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_DMAST | ECON1_CSUMEN);
    SetBank(EDMACS);
    while (readOp(ENC28J60_READ_CTRL_REG, ECON1) & ECON1_DMAST);
    macResult = readReg(EDMACS);
    bitsResult = readReg(EBSTCS);
    // Compare the results
    // 0xF807 should always be generated in Address fill mode
    if ((macResult != bitsResult) || (bitsResult != 0xF807)) {
        return 0;
    }
    // reset test flag
    writeOp(ENC28J60_BIT_FIELD_CLR, EBSTCON, EBSTCON_TME);
    // Now start the BIST with random data test, and also keep on swapping the
    // DMA/BIST memory ports.
    writeRegByte(EBSTSD, 0b10101010 | 0xAF);
    writeRegByte(EBSTCON, EBSTCON_TME | EBSTCON_PSEL | EBSTCON_BISTST |
        RANDOM_FILL);
    // wait for BISTST to be reset, only after that are we actually ready to
    // start the test
    // this was undocumented :(
    while (readOp(ENC28J60_READ_CTRL_REG, EBSTCON) & EBSTCON_BISTST);
    writeOp(ENC28J60_BIT_FIELD_CLR, EBSTCON, EBSTCON_TME);
    // now start the actual reading an calculating the checksum until the end is
    // reached
    writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_DMAST | ECON1_CSUMEN);
    SetBank(EDMACS);
    while (readOp(ENC28J60_READ_CTRL_REG, ECON1) & ECON1_DMAST);
    macResult = readReg(EDMACS);
    bitsResult = readReg(EBSTCS);
    // The checksum should be equal
    return macResult == bitsResult;
}


void memcpy_to_enc(uint16_t dest, void *source, int16_t num)
{
    writeReg(EWRPT, dest);
    writeBuf(num, (uint32_t *) source);
}

void memcpy_from_enc(void *dest, uint16_t source, int16_t num)
{
    writeReg(ERDPT, source);
    readBuf(num, (uint32_t *) dest);
}

uint16_t endRam = ENC_HEAP_END;
uint16_t enc_malloc(uint16_t size)
{
    if (endRam - size >= ENC_HEAP_START) {
        endRam -= size;
        return endRam;
    }
    return 0;
}

uint16_t enc_freemem()
{
    return endRam - ENC_HEAP_START;
}

uint16_t readPacketSlice(char *dest, int16_t maxlength, int16_t packetOffset)
{
    uint16_t erxrdpt = readReg(ERXRDPT);
    int16_t packetLength;
    memcpy_from_enc((char *) &packetLength, (erxrdpt + 3) % (RXSTOP_INIT + 1), 2);
    packetLength -= 4; // remove crc
    int16_t uint32_tsToCopy = packetLength - packetOffset;
    if (uint32_tsToCopy > maxlength) { uint32_tsToCopy = maxlength; }
    if (uint32_tsToCopy <= 0) { uint32_tsToCopy = 0; }
    int16_t startofSlice = (erxrdpt + 7 + packetOffset) % (RXSTOP_INIT + 1);
    memcpy_from_enc(dest, startofSlice, uint32_tsToCopy);
    dest[uint32_tsToCopy] = 0;
    return uint32_tsToCopy;
}

uint32_t* tcpOffset () { return buffer + 0x36; } //!< Pointer to the start of TCP payload
