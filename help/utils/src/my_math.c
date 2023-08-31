/// my_math.c

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
/// Authored by:   Adrian (05/02/2014)
/// Revised by:
/// Last Version:  05/02/2014
///
/// FILE CONTENTS:
/// Default header to be included in most C/C++ source code files.

#include "sam4lc4c.h"
#include "delays.h"

/** \brief
 * Square root calculation by Newton-Raphson method
 *
 * \param   value:
 *          number of max iterations
 */
unsigned int sqrt_newtoon(unsigned int value, unsigned int iterations)
{
    unsigned int value_xn, value_xn1, error, result;

    if (value > 65536) {
        return 0;
    }

    value_xn1 = value / 2;

    error = (value_xn1 * value_xn1) - value;

    while (iterations > 0) {
        value_xn = value_xn1 - ((error) / (2 * value_xn1));
        error = (value_xn * value_xn) - value;

        if (value_xn1 == value_xn) {
            return value_xn;
        }

        value_xn1 = value_xn;

        iterations --;

    }

    DebugWrite("Max iter  \r\n");
    return value_xn;
}
