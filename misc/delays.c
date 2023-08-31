/// delays.c
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
/// Authored by:   Adrian (13/12/2013)
/// Revised by:    JRB (16/12/2013)
/// Last Version:  13/12/2013
///
/// FILE CONTENTS:
/// Delay function and various defines and headers to perform waits

#include "delays.h"

// Delay loop is put to SRAM so that FWS will not affect delay time
void portable_delay_cycles(unsigned long n)
{
    __asm (
        "loop: DMB  \n"
        "SUBS R0, R0, #1  \n"
        "BNE.N loop         "
    );
}
