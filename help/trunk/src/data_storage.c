/// data_storage.c
///
/// Copyright (C) 2013 INGEN10 Ingenieria SL
/// http://www.ingen10.com
///
/// LEGAL NOTICE:
/// All information contained herein is, and remains property of INGEN10 Ingenieria SL.
/// Dissemination of this information or reproduction of this material is strictly
/// forbidden unless prior written permission is obtained from its owner.
/// ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC PERFORMANCE, OR PUBLIC DISPLAY
/// OF, OR THROUGH USE OF THIS SOURCE CODE IS STRICTLY PROHIBITED, AND IT IS A VIOLATION
/// OF INTERNATIONAL TRADE TREATIES AND LAWS.
/// THE RECEIPT OR POSSESSION OF THIS DOCUMENT DOES NOT CONVEY OR IMPLY ANY RIGHTS.
///
/// Authored by:   JRB (21/12/2013)
/// Revised by:
/// Last Version:
///
/// FILE CONTENTS:
/// Functions for controlling EEPROM (RTCC) storage of calibration and params

#include "data_storage.h"

#include "string_utils.h"
#include "rtcc.h"
#include "debug.h"
#include "configuration.h"

extern int kPgaGainValues[];

/** \section PARAMS STORAGE
 ///////////////////////////////////////////////////////////////////
 * Params struct save and recall from internal EEPROM (RTCC)
 ///////////////////////////////////////////////////////////////////
 */

void LoadParams(Params* p_params)
{
    ReadConfig(p_params, CONFIG_ADDRESS);

    if (p_params->checksum != CheckSumCalculate(p_params)) {
        ResetParams(p_params);
    }
}

void SaveParams(Params* p_params)
{
    p_params->checksum = CheckSumCalculate(p_params);
    WriteConfig(p_params, CONFIG_ADDRESS);
}

void ResetParams(Params* p_params)
{
    p_params->lwakeup = 1000;
    p_params->tstab = 3;
    p_params->lstab = 900;
    p_params->nstab = 5;
    p_params->tmeas = 32000;
    p_params->tdelay = 10;
    p_params->toff = 10;
    p_params->srate = BASE_SAMPLE_RATE;
    p_params->id = 6060;
    p_params->gain = kPgaGainValues[0];     // { 1, 2, 5, 10, 20, 50, 100, 200 };

    p_params->acel = 1;
    p_params->calibration = 204790;
}


unsigned CheckSumCalculate(Params* p_params)
{
    char* pointer;
    unsigned int struct_size;
    int i;
    unsigned int sum = 0;
    pointer = (char*) p_params;
    //get struct size
    struct_size = sizeof(*p_params);
    struct_size -= sizeof(p_params->checksum);

    for (i = 0; i < struct_size; i++) {
        sum += (*pointer);
        pointer++;
    }

    return sum;
}

