/*
 * RpmMeter.h
 *
 *  Created on: Sep 29, 2024
 *      Author: dharmendra.singh
 */

#ifndef INC_RPMMETER_H_
#define INC_RPMMETER_H_

#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif

uint16_t u16GetEngineSpeed(void);
void vRpmDataProcessing();

#ifdef __cplusplus
}
#endif

#endif /* INC_RPMMETER_H_ */
