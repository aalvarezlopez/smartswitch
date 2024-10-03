/**
 * @file nvm.c
 * @brief Stored data on the FLASH memory
 * @author Adrian Alvarez Lopez
 *
 *
 * Very limited functionality. Write has to be done in 64 bits
 *
 * NVM is allocated en last 65kB (0x10000) of the FLASH memory. Page size is 512 bytes and
 * driver cannot erase less than 8 pages. NVM partitions has to be at least of 
 * 16 pages (8 kB, 2 nvm pages of 4 kb each)
 *
 *
 * FLASH MEMORY
 * 0x400000   START OF CODE AREA
 *        .....
 * 0x42FFFF   END OF CODE AREA
 * 0x430000   START OF DATA AREA
 *        .....
 * 0x43FFFF   END OF DATA AREA
 *
 * @version 1.0.0
 * @date 2022-10-23
 */

#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "nvm.h"
#include "flash_efc.h"
#include "efc.h"
#include "errorlog.h"

#define IFLASH_PAGE_SIZE      512
#ifdef __SAM4S4A__
#define NVM_START_ADDRESS     0x430000
#define __uintlong_t uint32_t
#define NVM_MASK_PAGE 0xFFFFF000
#else
#define __uintlong_t uint64_t
#define NVM_MASK_PAGE 0xFFFFFFFFFFFFF000
extern uint32_t mok_flash_memory[];
#define NVM_START_ADDRESS     ((uint64_t)mok_flash_memory)
#endif
#define NVM_SIZE              0x010000
#define NVM_BLOCK_SIZE_BYTES  16
#define NVM_PARTITION_OFFSET  0x3000

#define _VERSION  0x01
#define _BLOCK_ID 0xD0

#define N_PARTITIONS  1

partition_st partition[N_PARTITIONS];

uint8_t *nextFreeBlockAddress[N_PARTITIONS];
uint8_t _lastSequence[N_PARTITIONS];
uint8_t activePage[N_PARTITIONS];

void NvM_Init(void)
{
    uint32_t ul_rc;
    ul_rc = flash_init(FLASH_ACCESS_MODE_128, 6);

    partition[0].numberOfSectors  = 16;
    partition[0].startofpartition = NVM_START_ADDRESS + NVM_PARTITION_OFFSET;
    partition[0].sizeofblock = (sizeof(nvmSettings_st)/8) + 1;
    partition[0].sizeofblock *= 8;
}

uint8_t NvM_ReadAll(void)
{
    uint8_t *flashpointer;
    for(uint8_t npart = 0; npart < N_PARTITIONS; npart++){
        flashpointer = (uint8_t*)(partition[npart].startofpartition);
        nextFreeBlockAddress[npart] = flashpointer;
        activePage[npart] =  (partition[npart].numberOfSectors / 8) - 1;
        for(uint8_t npage = 0; npage < ((partition[npart].numberOfSectors / 8) - 1); npage++){
            uint32_t * headerptr = (uint32_t*)flashpointer;
            uint32_t * nextheaderptr = (uint32_t*)(flashpointer + ((npage + 1) * IFLASH_PAGE_SIZE * 8));
            uint32_t current_sequence = *(headerptr+NVM_LASTSEQ_OFFSET);
            uint32_t next_sequence = *(nextheaderptr+NVM_LASTSEQ_OFFSET);
            if( *(flashpointer + ((npage + 1) * IFLASH_PAGE_SIZE * 8))  == NVM_VIRGIN_PATTERN ){
                activePage[npart] =  npage;
                flashpointer += (npage * IFLASH_PAGE_SIZE * 8);
                break;
            }else if( current_sequence > next_sequence ){
                activePage[npart] =  npage;
                flashpointer += (npage * IFLASH_PAGE_SIZE * 8);
                break;
            }
        }
        for(uint32_t i = 0;
            i < (IFLASH_PAGE_SIZE * 8);
            i=i+(partition[npart].sizeofblock)){
            uint32_t * headerptr = (uint32_t*)(flashpointer + (activePage[npart] * IFLASH_PAGE_SIZE * 8) + i);
            if( (*(flashpointer+ (activePage[npart] * IFLASH_PAGE_SIZE * 8) + i)) == NVM_VIRGIN_PATTERN){
                nextFreeBlockAddress[npart] = flashpointer + (activePage[npart] * IFLASH_PAGE_SIZE * 8) + i;
                break;
            }
            _lastSequence[npart] = *(headerptr + NVM_LASTSEQ_OFFSET);
        }
        if((__uintlong_t)(nextFreeBlockAddress[npart]) >
            (((__uintlong_t)partition[npart].startofpartition) + (partition[npart].numberOfSectors * IFLASH_PAGE_SIZE * 8))){
            nextFreeBlockAddress[npart] = (uint8_t*)(partition[npart].startofpartition);
        }
    }
    return N_PARTITIONS;
}

void NvM_Write(uint32_t * buffer, uint8_t npart)
{
    uint32_t *headerptr;
    uint32_t *endaddress;
    uint32_t addressfordebug = nextFreeBlockAddress[npart];
    _lastSequence[npart]++;
    headerptr = buffer;
    *(headerptr+NVM_VERSION_OFFSET) = _VERSION;
    *(headerptr+NVM_BLOCK_ID_OFFSET) = _BLOCK_ID;
    *(headerptr+NVM_LASTSEQ_OFFSET) = _lastSequence[npart];
    endaddress = (uint32_t*)(nextFreeBlockAddress[npart] + partition[npart].sizeofblock);
    if( ((__uintlong_t)(nextFreeBlockAddress[npart] - partition[npart].startofpartition) / (IFLASH_PAGE_SIZE * 8)) !=
        (((__uintlong_t)endaddress - partition[npart].startofpartition) / (IFLASH_PAGE_SIZE * 8))){
        __uintlong_t npage = ((__uintlong_t)nextFreeBlockAddress[npart] - partition[npart].startofpartition) / ( IFLASH_PAGE_SIZE * 8);
        if( (npage + 1) >= ( partition[npart].numberOfSectors / 8 )){
            nextFreeBlockAddress[npart] = (uint8_t*)(partition[npart].startofpartition);
        }else{
            nextFreeBlockAddress[npart] = (uint8_t*)(partition[npart].startofpartition +  ((npage + 1) * (IFLASH_PAGE_SIZE * 8)));
        }

        flash_unlock((__uintlong_t)(nextFreeBlockAddress[npart]),
            (uint32_t)(nextFreeBlockAddress[npart]) + IFLASH_PAGE_SIZE - 1, 0, 0);
        flash_erase_page((__uintlong_t)(nextFreeBlockAddress[npart]), IFLASH_ERASE_PAGES_8);
    }

    if( nextFreeBlockAddress[npart] > 0x500000 || nextFreeBlockAddress[npart] < 0x400000 ){
        errorlog_reportDebug(NVM_MODULE, NVM_INVALIDBLOCKADD, addressfordebug);
        nextFreeBlockAddress[npart] = (uint8_t*)(partition[npart].startofpartition);
    }else{
        flash_write((__uintlong_t)(nextFreeBlockAddress[npart]), buffer, partition[npart].sizeofblock, 0);
        nextFreeBlockAddress[npart]+=partition[npart].sizeofblock;
        if((uint32_t)(nextFreeBlockAddress[npart]) >
            (partition[npart].startofpartition + (partition[npart].numberOfSectors * IFLASH_PAGE_SIZE))){
            nextFreeBlockAddress[npart] = (uint8_t*)(partition[npart].startofpartition);
        }
    }
}

uint8_t NvM_GetSequence(uint8_t npart)
{
    return _lastSequence[npart];
}

uint8_t NvM_ReadBlock(uint8_t npart, uint8_t nbytes, uint8_t nentries, uint8_t *dout)
{
    uint8_t n = 0;
    uint32_t *flashpointer = (uint32_t*)((uint8_t*)nextFreeBlockAddress[npart] - partition[npart].sizeofblock);
    __uintlong_t startOfPartition = partition[npart].startofpartition;
    for( ; (__uintlong_t)flashpointer >= startOfPartition; flashpointer-=partition[npart].sizeofblock){
        if( (*(flashpointer+NVM_VERSION_OFFSET) == _VERSION) && (*(flashpointer+NVM_BLOCK_ID_OFFSET) == _BLOCK_ID)){
            memcpy( dout + (n*(uint16_t)nbytes), flashpointer,
                nbytes);
            n++;
            if( n >= nentries ){
                break;
            }
        }
    }
    return n;
}
