/*
 * SootLoad.h
 *
 *  Created on: Sep 28, 2024
 *      Author: dharmendra.singh
 */

#ifndef INC_SOOTLOAD_H_
#define INC_SOOTLOAD_H_


#include "main.h"


#ifdef __cplusplus
extern "C"
{
#endif


void vSootLoadDataProcessing();
uint16_t u16GetSootLoadValue(void);
uint8_t u8GetSootLoadBadDataTimeoutStatus(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_SOOTLOAD_H_ */
