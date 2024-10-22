/**
 * @file dimmer.h
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2024-10-22
 */

#ifndef DIMMER_H
#define DIMMER_H

void Dimmer_Init(void);
void Dimmer_Task(void);
void Dimmer_setLevel(uint8_t level);
void Dimmer_start(uint8_t init, uint8_t end, uint8_t step, uint8_t speed);
uint8_t Dimmer_getTarget(void);
#endif
