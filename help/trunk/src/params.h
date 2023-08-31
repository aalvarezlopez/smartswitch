/// params.h
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
/// Authored by:   AV (17/12/2013)
/// Revised by:
/// Last Version:
///
/// FILE CONTENTS:
/// param.h define struct to store params

#ifndef PARAM_H_
#define PARAM_H_

//REVIEW: "unsigned"?? (is it unsigned long??)
typedef struct {
    unsigned lwakeup;
    unsigned tstab;
    unsigned lstab;
    unsigned nstab;
    unsigned tmeas;
    unsigned tdelay;
    unsigned toff;
    unsigned srate;
    unsigned id;
    unsigned gain;
    unsigned acel;
    unsigned calibration;
    unsigned checksum;
} Params;

#endif /* PARAM_H_ */
