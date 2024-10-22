/**
 * @file dimmer.c
 * @brief Handle dimmer percentage
 * @author Adrian Alvarez Lopez
 * @version 1.0.0
 * @date 2024-10-22
 */


#include "stdio.h"
#include "dimmer.h"
#include "io.h"

static uint8_t dimmer_targetLevel = 0;
static uint8_t dimmer_currentLevel = 0;
static uint8_t dimmer_currentstep = 0;
static uint8_t dimmer_currentSpeed = 0;
static uint8_t dimmer_counter = 0;

void Dimmer_Init(void)
{
    IO_setDimmer(0);
}

void Dimmer_Task(void)
{
    if( dimmer_targetLevel > dimmer_currentLevel ){
        dimmer_currentLevel = ( dimmer_counter > dimmer_currentSpeed ) ?
            dimmer_currentLevel + dimmer_currentstep : 
            dimmer_currentLevel;
    }else if (dimmer_targetLevel < dimmer_currentLevel){
        dimmer_currentLevel = ( dimmer_counter > dimmer_currentSpeed ) ?
            dimmer_currentLevel - dimmer_currentstep : 
            dimmer_currentLevel;
    }else{
    }
    IO_setDimmer(dimmer_currentLevel);
    dimmer_counter = (dimmer_counter > dimmer_currentSpeed) ? 0 : dimmer_counter + 1;
}

void Dimmer_setLevel(uint8_t level)
{
    dimmer_currentstep = 0;
    dimmer_currentSpeed = 0;
    dimmer_currentLevel = level;
    dimmer_targetLevel = 0;
    IO_setDimmer(dimmer_currentLevel);
}

void Dimmer_start(uint8_t init, uint8_t end, uint8_t step, uint8_t speed)
{
    if( end != dimmer_currentLevel ){
        if( dimmer_currentLevel == dimmer_targetLevel){
            dimmer_currentLevel = init;
        }
        dimmer_currentstep = step;
        dimmer_currentSpeed = speed;
        dimmer_targetLevel = end;
    }
    IO_setDimmer(dimmer_currentLevel);
}

uint8_t Dimmer_getTarget(void)
{
    return dimmer_targetLevel;
}
