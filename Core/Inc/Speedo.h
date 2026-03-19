/*
 * Speedo.h
 *
 *  Created on: Oct 11, 2024
 *      Author: dharmendra.singh
 */

#ifndef INC_SPEEDO_H_
#define INC_SPEEDO_H_



#include "main.h"


#define MAX_FREQ_SPEEDO			2528u
#define MAX_SPEED_FRQ			50u
#define MAX_SPEED_CAN			100u
#define FREQ_OUT_TIME 			490   	 // in ms

#ifdef __cplusplus
extern "C"
{
#endif

uint32_t GetVehicleSpeed(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_SPEEDO_H_ */
