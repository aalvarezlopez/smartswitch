/*
 * debug.h
 *
 *  Created on: 03/12/2013
 *      Author: Adrian
 */

//#define DEBUG

#ifndef DEBUG_H_
#define DEBUG_H_

#include "usb_protocol_cdc.h"

#define MAXSTRINGSIZE   100
#define MAXDIGITLNG 8

#ifdef GLOBALSFORME
    //Global variables
    bool main_b_cdc_enable = false;
    volatile bool main_b_msc_enable = false;
    int char_received_flag;
#else
    extern bool main_b_cdc_enable;
    extern volatile bool main_b_msc_enable;
    extern int char_received_flag;
#endif

void InitDebug();
void UsbPuts(const char* string);
void DebugWrite(const char* string);

void DebugHexWrite(const char* string, int value);
void DebugDecWrite(const char* string, int value);

void DebugPrintDec(int value);
void PrintDec(int value);

void WaitOpenSerial();
void DebugPrintInit();
void UsbPuts(const char* string);
void DebugHourWrite(int seconds, int minutes, int hour);
void DebugDateWrite(int day, int month, int year);

#endif /* DEBUG_H_ */
