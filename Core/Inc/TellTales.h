/* CA78K0R C Source Converter V1.00.03.01 [10 May 2018] */
/*****************************************************************************
 DISCLAIMER
 This software is supplied by Renesas Electronics Corporation and is only
 intended for use with Renesas products. No other uses are authorized. This
 software is owned by Renesas Electronics Corporation and is protected under
 all applicable laws, including copyright laws.
 THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
 LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
 TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
 ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
 FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
 ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
 BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 Renesas reserves the right, without notice, to make changes to this software
 and to discontinue the availability of this software. By using this software,
 you agree to the additional terms and conditions found by accessing the
 following link:
 http://www.renesas.com/disclaimer
 Copyright (C) 2016-2018 Renesas Electronics Corporation. All rights reserved.
******************************************************************************/

#ifndef __TellTales_H
#define __TellTales_H

#include "main.h"  // include CPU file
// constant declarations -----------------------------------------------------


extern TIM_HandleTypeDef htim15;


#define TRUE 	1u
#define FALSE 	0u

#define LIFE_CRITICAL_ALARM			0x01
#define ENGINE_CRITICAL_ALARM		0x02
#define GENRAL_ALARM				0x04
#define TIC_ALARM					0x08
#define TOC_ALARM					0x10
#define CONTINUE_ALARM				0x20
#define BEEP_ALARM					0x40
#define IRRITATE_ALARM				0x80
#define BUZZER_STOP					0x00

typedef enum
{
	efuelGaugeBar0 = 0u,
	efuelGaugeBar1,
	efuelGaugeBar2,
	efuelGaugeBar3,
	efuelGaugeBar4,
	efuelGaugeBar5,
	efuelGaugeBar6,
	efuelGaugeBar7,
	efuelGaugeBar8,
	eTotalfuelGaugeBars,
}eFuelGaugeBarType;

typedef enum
{
	eCoolTempGaugeBar0 = 0u,
	eCoolTempGaugeBar1,
	eCoolTempGaugeBar2,
	eCoolTempGaugeBar3,
	eCoolTempGaugeBar4,
	eCoolTempGaugeBar5,
	eCoolTempGaugeBar6,
	eCoolTempGaugeBar7,
	eCoolTempGaugeBar8,
	eTotalCoolTempGaugeBars,
}eCoolTempGaugeBarType;

// Global declarations --------------------------------------------------------
typedef	struct
{
	uint8_t bAIR_PRESSUR :1;
	uint8_t bFuelGauge :1;
	uint8_t bEngineCoolTempGauge :1;
	uint8_t bTransmissionOilTemp:1;
	uint8_t bPARK_BREAK:1;
	uint8_t bTURN_RIGHT:1;
	uint8_t bTURN_LEFT:1;
	uint8_t bTransOilPrssBar:1;
	uint8_t bBuzzerEnable:1;
	uint8_t u8BuzzerSound;
}BUZZER_TYPE;



extern BUZZER_TYPE	sBuzz_Control;

// Global prototypes ---------------------------------------------------------

/// @addtogroup TELL_TALE_LIBRARY
/// @{
 
/// @defgroup TellTaleHeaderLib Global functions used in Tell tale
//// 
///   
/// @addtogroup TellTaleHeaderLib
/// @{


void vControlTellTales(void);
void vAllLampsOff(void);
void vCheckLamps(void);
uint8_t GetParkBrakeLampBuzzStatus();
uint8_t GetLowFeuelStatus();
void vBuzzerControl(void);
void vBuzzerDisable(void);
void vBuzzerEnable(void);
void vSetBuzzerSoundType(uint8_t);
void _vStopBuzzer();

#ifdef __cplusplus
extern "C"
{
#endif
uint8_t GetMilLampStatus();
uint8_t GetPCDLampStatus();
uint8_t GetSCRTemperingLampStatusCAN(void);
uint8_t GetLowAdBlueLevelLampStatusCAN(void);
uint8_t GetDPFRegenInhibitLampStatusCAN(void);
uint8_t GetTransOIlTempLedOnTFTStatus();

#ifdef __cplusplus
}
#endif
/// @} // endgroup TellTaleHeaderLib

/// @} // endgroup TELL_TALE_LIBRARY

#endif

