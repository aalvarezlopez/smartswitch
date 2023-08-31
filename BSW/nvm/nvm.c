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

#include <stdio.h>
#include "nvm.h"
#include "flash_efc.h"
#include "efc.h"

#define FLASH_ACCESS_MODE_128 0
#define IFLASH_PAGE_SIZE      512
#define NVM_START_ADDRESS     0x430000
#define NVM_SIZE              0x010000
#define NVM_BLOCK_SIZE_BYTES  16
#define NVM_PARTITION_OFFSET  0x3000

#define _VERSION  0x01
#define _BLOCK_ID 0xD0

typedef struct partition_s{
    uint8_t numberOfSectors;   /*Number of NVM pages*/
    uint32_t startofpartition;  /*Flash address where the block is starting*/
    uint8_t sizeofblock;       /*Number of bytes of the data, including the sequence counter.
                                 It has to be divided by NVM_BLOCK_SIZE_BYTES*/
}partition_st;

#define N_PARTITIONS  2

partition_st partition[N_PARTITIONS];

uint8_t *nextFreeBlockAddress[N_PARTITIONS];
uint8_t _lastSequence[N_PARTITIONS];
uint8_t activePage[N_PARTITIONS];

void NvM_Init(void)
{
    uint32_t ul_rc;
    ul_rc = flash_init(FLASH_ACCESS_MODE_128, 6);

    partition[0].numberOfSectors  = 16;
    partition[0].startofpartition = NVM_START_ADDRESS;
    partition[0].sizeofblock = NVM_BLOCK_SIZE_BYTES;

    partition[1].numberOfSectors  = 16;
    partition[1].startofpartition = NVM_START_ADDRESS + NVM_PARTITION_OFFSET;
    partition[1].sizeofblock = (NVM_BLOCK_SIZE_BYTES*5);
}

uint8_t NvM_ReadAll(uint32_t *buffer)
{
    uint8_t *flashpointer = partition[0].startofpartition;
    for(uint8_t npart = 0; npart < N_PARTITIONS; npart++){
        flashpointer = partition[npart].startofpartition;
        nextFreeBlockAddress[npart] = flashpointer;
        for(uint8_t npage = 0; npage < partition[npart].numberOfSectors; npage++){
            if( *(flashpointer + ((npage + 1) * IFLASH_PAGE_SIZE) - partition[npart].sizeofblock)  == 0xFF ){
                activePage[npart] =  npage;
                flashpointer += (npage * IFLASH_PAGE_SIZE);
                break;
            }
        }
        for(uint32_t i = 0;
            i < IFLASH_PAGE_SIZE;
            i=i+partition[npart].sizeofblock){
            if( (*(flashpointer+ (activePage[npart] * IFLASH_PAGE_SIZE) + i)) == 0xFF){
                nextFreeBlockAddress[npart] = flashpointer + (activePage[npart] * IFLASH_PAGE_SIZE) + i;
                break;
            }
            _lastSequence[npart] = *(flashpointer + (activePage[npart] * IFLASH_PAGE_SIZE) + i);
        }
        if(nextFreeBlockAddress[npart] >
            (partition[npart].startofpartition + (partition[npart].numberOfSectors * IFLASH_PAGE_SIZE))){
            nextFreeBlockAddress[npart] = partition[npart].startofpartition;
        }
    }
    return N_PARTITIONS;
}

void NvM_Write(uint32_t * buffer, uint8_t npart)
{
    uint8_t *headerptr;
    headerptr = buffer;
    *headerptr     = _lastSequence[npart];
    *(headerptr+1) = _VERSION;
    *(headerptr+2) = _BLOCK_ID;
    if(((uint32_t)(nextFreeBlockAddress[npart] - NVM_START_ADDRESS) % (IFLASH_PAGE_SIZE * 4) ) == 0){
        flash_unlock(nextFreeBlockAddress[npart],
            nextFreeBlockAddress[npart] + IFLASH_PAGE_SIZE - 1, 0, 0);
        flash_erase_page(nextFreeBlockAddress[npart], IFLASH_ERASE_PAGES_8);
    }

    flash_write(nextFreeBlockAddress[npart], buffer, partition[npart].sizeofblock, 0);
    nextFreeBlockAddress[npart]+=partition[npart].sizeofblock;
    if(nextFreeBlockAddress[npart] >
        (partition[npart].startofpartition + (partition[npart].numberOfSectors * IFLASH_PAGE_SIZE))){
        nextFreeBlockAddress[npart] = partition[npart].startofpartition;
    }
}

uint8_t NvM_GetSequence(uint8_t npart)
{
    return _lastSequence[npart];
}

uint8_t NvM_ReadBlock(uint8_t npart, uint8_t blockid, uint8_t nentries, uint8_t *dout)
{
    uint8_t n = 0;
    uint8_t *flashpointer = nextFreeBlockAddress[npart] - partition[npart].sizeofblock;
    uint32_t startOfPartition = (uint32_t)(nextFreeBlockAddress[npart]) & (uint32_t)(0xFFFFF000);
    for( ; flashpointer >= startOfPartition; flashpointer-=partition[npart].sizeofblock){
        if( (*(flashpointer+1) == _VERSION) && (*(flashpointer+2) == _BLOCK_ID)){
            memcpy( dout + (n*partition[npart].sizeofblock), flashpointer,
                partition[npart].sizeofblock);
            n++;
            if( n >= nentries ){
                break;
            }
        }
    }
    return n;
}
