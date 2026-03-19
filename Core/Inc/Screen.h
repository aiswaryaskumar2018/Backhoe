/*
 * Screen.h
 *
 *  Created on: Oct 11, 2024
 *      Author: dharmendra.singh
 */

#ifndef INC_SCREEN_H_
#define INC_SCREEN_H_


#include "main.h"





#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
	eScreenTripA = 0u,
	eScreenTripB,
	eTotalScreenTrip
};

typedef enum
{
	eOdoMeterSelect = 0u,
	eHourMeterSelect,
	eTotalMeter
};

typedef enum
{
	eHomeScreen = 0,
	eMenuScreen,
	eBrightnessScreen,
	ePCodeScreen,
	eTotalScreen
};

typedef enum
{
	eBrightnessSelectState = 0u,
	ePCodeSelectState,
	eExitState,
	eTotalScreenSelect
};



uint8_t GetvShowTrip_A_B_DataOnTFT(void);
uint8_t GetTripBResetStatus(void);
uint8_t GetTripAResetStatus(void);
void TripA_B_Reset(void);
uint8_t GetScreenStatus(void);
uint8_t GetOptionStatus(void);
uint8_t GetArrowStatus(void);
uint8_t GetBrightnessBarStatus(void);
void SetBrightnessBarStatus(uint8_t Value);
void ScreensProcessed(void);
uint8_t GetReturnStatus(void);
uint8_t GetvShowHourOdoSelectOnTFT();
/*Below Function declaration available in Pcode & Brightness screen */
void SetBrightnessScreenEnterStatus(uint8_t Status);
uint8_t GetBrightnessScreenEnterStatus(void);
void SetPcodeScreenEnterStatus(uint8_t Status);
uint8_t GetPcodeScreenEnterStatus(void);
uint8_t GetBrightNessScreenExitStatus(void);
void SetBrightNessScreenExitStatus(uint8_t Status);
uint8_t GetCountSetButtonForPcode(void);
void SetCountSetButtonForPcode(uint8_t Count);

#ifdef __cplusplus
}
#endif
#endif /* INC_SCREEN_H_ */
