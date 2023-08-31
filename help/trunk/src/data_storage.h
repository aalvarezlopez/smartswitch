/*
 * data_storage.h
 *
 *  Created on: 21/12/2013
 *      Author: JRB
 */

#ifndef DATASTORE_H_
#define DATASTORE_H_

#include "params.h"

#define CONFIG_ADDRESS  0

void LoadParams(Params* p_params);
void SaveParams(Params* p_params);
void ResetParams(Params* p_params);
unsigned CheckSumCalculate(Params* p_params);

#endif /* DATASTORE_H_ */
