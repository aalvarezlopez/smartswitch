/**
 * @file io_pri.h
 * @brief I/O abstraction component private header
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2022-01-17
 */


#ifndef IO_PRI_H
#define IO_PRI_H
#include "sam4s4a.h"
#include "component_pio.h"

/*******************************************************************************
 * Input/output connection
 ******************************************************************************/
#define TOGGLE_LED1() do{\
        if(PIOA->PIO_ODSR & PIO_ODSR_P7){PIOA->PIO_CODR = PIO_ODSR_P7;}\
        else{PIOA->PIO_SODR = PIO_SODR_P7;}}while(0);

#define SET_LED1_ON()  (PIOA->PIO_SODR = PIO_SODR_P7)
#define SET_LED1_OFF() (PIOA->PIO_CODR = PIO_CODR_P7)

#endif
