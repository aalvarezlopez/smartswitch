/**
 * @file smartswitch.h
 * @brief 
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2024-03-04
 */

#ifndef SMARTSWITCH_H
#define SMARTSWITCH_H
#include "stdint.h"
#include "io.h"

void SmartSwitch_Init(void);
void SmartSwitch_Task(void);
void SmartSwitch_SlowTask(void);
void SmartSwitch_Action(bool presence,bool button);
void SmartSwitch_flowMeter(bool q1, bool q2);
#endif
