/**
 * @file errorlog.h
 * @brief Keeps all the error under the same component
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-07-02
 */

#ifndef ERROR_LOG_H
#define ERROR_LOG_H

#define MAX_HISTORY_LEN 10u

typedef enum{
	SPI_MODULE = 10,
	DS18B20_MODULE = 11,
	NVM_MODULE
}MODULES_t;

typedef struct log_s{
    MODULES_t module;
    uint32_t code;
    uint32_t info;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t sec;
}log_st;

void errorlog_Init(void);
void errorlog_reportError(MODULES_t module, uint32_t info, uint32_t len);
void errorlog_reportDebug(MODULES_t module, uint32_t info, uint32_t len);
uint16_t errorlog_readErrors(uint32_t * add);
uint16_t errorlog_readDebug(uint32_t * add);

/****** NVM ****/
#define NVM_INVALIDBLOCKADD 1

#endif
