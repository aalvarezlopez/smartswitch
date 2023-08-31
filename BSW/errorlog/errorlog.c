/**
 * @file errorlog.c
 * @brief Errors can be reported through this module. Reactions can be defined for each error type
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-07-02
 */

#include "stdint.h"
#include "errorlog.h"

uint8_t errorCounter = 0;

MODULES_t errors[MAX_HISTORY_LEN];

void errorlog_Init(void)
{
}

void errorlog_reportError(MODULES_t code, uint8_t *info, uint8_t len)
{
    errors[ errorCounter % MAX_HISTORY_LEN] = code;
    errorCounter++;
}
