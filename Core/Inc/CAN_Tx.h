/*
 * SpeedoOdo.h
 *
 *  Created on: Sep 24, 2024
 *      Author: dharmendra.singh
 */

#ifndef INC_CAN_TX_H_
#define INC_CAN_TX_H_


#include "main.h"
#include "stm32u5xx_hal.h"


#ifdef __cplusplus
extern "C"
{
#endif

void CAN_Tx_SendMsg(void);
void HourRequestMessageOneTimeSend(void);

#ifdef __cplusplus
}
#endif




/// @} // endgroup odometerdataLib

/// @} // endgroup ODO_METER_DATA_LIBRARY

#endif /* INC_ODO_H_ */
