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
#ifndef __TRANS_OIL_TEMP_H
#define __TRANS_OIL_TEMP_H

#define GAUGE_AVG_SIZE 	16
#define GAUGE_AVG_SIZE				16
#define FILTER_VOLTAGE				10     //volt 1.0
#define VOLTAGE_HYS_UP				6      //volt 0.6
#define VOLTAGE_HYS_DOWN			4      //volt 0.4
#define VOLT_VARIATION_TIME 		3000


#include "stm32u5xx_hal.h"



#ifdef __cplusplus
extern "C"
{
#endif
extern FDCAN_HandleTypeDef hfdcan1;

void OilTempGauge(void);
uint8_t GetTransOilTempVaLue(void);
uint8_t GetActualOilTempValue(void);
void vTransOIL_TempAnlaogProcess(void);
uint8_t GetTransOilTempWarningBuzzerStatus();
uint8_t GetTransOilTempBarValueInplaceOfAdBlueGauge(void);
uint16_t GetTransOilTempCANTxData(void);


#ifdef __cplusplus
}
#endif


#endif
