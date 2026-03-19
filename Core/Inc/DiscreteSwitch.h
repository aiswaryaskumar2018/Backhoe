/*
 * DiscreteSwitch.h
 *
 *  Created on: Oct 10, 2024
 *      Author: dharmendra.singh
 */

#ifndef INC_DISCRETESWITCH_H_
#define INC_DISCRETESWITCH_H_


#include "main.h"


typedef enum
{
	ePressed = 0u,
	eReleased,
	eShortPressed,
	eLongPressed,
	eModeUpLongPressed,
	eModeDownLongPressed,
	eModeSetLongPressed,
	eSetUpLongPressed,
	eUpDownLongPressed,
	eInvalidPress,
	eStuckFault

}SW_STATE;



/// @addtogroup SWITCHES_LIB
/// @{

/// @defgroup swLib Private functions used for AMT operations
////
///
/// @addtogroup swLib
/// @{

SW_STATE eGetMODESwitchStatus(void);
SW_STATE eGetSETSwitchStatus(void);
void vResetButtonTimeStamps();
/// @} // endgroup swLib

/// @} // endgroup SWITCHES_LIB


#endif /* INC_DISCRETESWITCH_H_ */
