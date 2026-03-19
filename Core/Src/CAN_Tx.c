/*
 * SpeedoOdo.c
 *
 *  Created on: Sep 24, 2024
 *      Author: dharmendra.singh
 */

#include "CAN_Tx.h"


#define CAN_ID_FUEL_GAUGE_REQUEST 		0x18FEFC21
#define CAN_ID_HOUR_REQUEST 			0x18EA0021
#define CAN_ID_SOOT_LOAD_MASSS			0x18EA0017
#define CAN_ID_TRX_OIL_TEMP_REQUEST		0x18FEF821

static void CAN_TxHourAndSootLoad(void);
static void CAN_Tx_SendMsg_fuelgauge(void);
static void CAN_Tx_SendMsg_Trx_Oil_Temp(void);


uint8_t TxData_SootLoad[3u] = {0x7B,0xFD,0x00};
uint8_t TxData_SootLoadMass[3u] = {0x1C,0xFD,0x00};
uint8_t TxData_HourEicherEngine[3u] = {0xE5,0xFE,0x00};
uint8_t TxData_HourTATAEngine[3u] = {0xE7,0xFE,0x00};



/*********************************************************************//**
 *
 * Send CAN TX message to ECU
 *
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
void CAN_Tx_SendMsg(void)
{
	CAN_TxHourAndSootLoad();
	CAN_Tx_SendMsg_fuelgauge();
	CAN_Tx_SendMsg_Trx_Oil_Temp();
}



static void CAN_TxHourAndSootLoad(void)
{
	static uint32_t temp_var = 0;
	static uint32_t temp_varSootload = 0;

	if(u32GetClockTicks() - temp_var > 5000U)
	{
		TxHeader.Identifier = CAN_ID_HOUR_REQUEST;
		TxHeader.DataLength = 3u;
		HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, (uint8_t *)&TxData_HourEicherEngine); // Hour meter Eicher engine
		temp_var = u32GetClockTicks();
	}

	if(u32GetClockTicks() - temp_varSootload > 2000U)
	{
		/* Condition will add as per engine*/
		if(GetEngineSelectionType() == eBackhoeLoaderKoelEngine74Hp)
		{
			TxHeader.Identifier = CAN_ID_SOOT_LOAD_MASSS;
			TxHeader.DataLength = 3u;
			HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, (uint8_t *)&TxData_SootLoadMass); // SootLoad mass
		}
		else
		{
			TxHeader.Identifier = CAN_ID_SOOT_LOAD_MASSS;
			TxHeader.DataLength = 3u;
			HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, (uint8_t *)&TxData_SootLoad); // SootLoad
		}
		temp_varSootload = u32GetClockTicks();
	}
}

void HourRequestMessageOneTimeSend(void)
{
	TxHeader.Identifier = CAN_ID_HOUR_REQUEST;
	TxHeader.DataLength = 3u;
	HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, (uint8_t *)&TxData_HourEicherEngine); // Hour meter Eicher engine

	if(GetEngineSelectionType() == eBackhoeLoaderKoelEngine74Hp)
	{
		TxHeader.Identifier = CAN_ID_SOOT_LOAD_MASSS;
		TxHeader.DataLength = 3u;
		HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, (uint8_t *)&TxData_SootLoadMass); // SootLoad mass
	}
	else
	{
		TxHeader.Identifier = CAN_ID_SOOT_LOAD_MASSS;
		TxHeader.DataLength = 3u;
		HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, (uint8_t *)&TxData_SootLoad); // SootLoad
	}
}



static void CAN_Tx_SendMsg_fuelgauge(void)
{
	static uint32_t temp_var = 0u;
	uint8_t Data[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	
	Data[1]  = GetFuelCANTxData();
	
	if(u32GetClockTicks() - temp_var > 500U)
	{
		TxHeader.Identifier = CAN_ID_FUEL_GAUGE_REQUEST;
		TxHeader.DataLength = 8u;
		HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, (uint8_t *)&Data); // Fuel Gauge Eicher engine
		temp_var = u32GetClockTicks();
	}

}

static void CAN_Tx_SendMsg_Trx_Oil_Temp(void)
{
	static uint32_t temp_var = 0;
	static uint16_t Temp_value = 0;

    uint8_t Data[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    Temp_value  = GetTransOilTempCANTxData();

	if(u32GetClockTicks() - temp_var > 500U)
	{
		TxHeader.Identifier = CAN_ID_TRX_OIL_TEMP_REQUEST;
		TxHeader.DataLength = 8u;
		Data[4] = Temp_value;    //CAN trx
		Data[5] = Temp_value>>8;    //CAN trx
		HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, (uint8_t *)&Data); // Fuel Gauge Eicher engine
		temp_var = u32GetClockTicks();
	}

}
