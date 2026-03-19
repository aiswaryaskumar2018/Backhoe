/*
 * BattVolt.c
 *
 *  Created on: Sep 20, 2024
 *      Author: aiswarya.kumar
 */

#include "adc.h"
#include "CurveHeader.h"
#include "BattVolt.h"
#include "J1939.h"

uint32_t BattVolt = 0;





#define BattVolt_SIGNAL_AVG              	4
#define BattVolt_HYS							10		//BattVolt


uint16_t BattVoltvalue = 0;


// local declarations --------------------------------------------------------
typedef struct
{
	uint32_t     u32BattVolt;
	uint16_t     u16BattVoltSignalData[BattVolt_SIGNAL_AVG];
	uint16_t	u16EngineOverSpeed;
} BattVolt_DATA_TYPE;

BattVolt_DATA_TYPE sBattVolt = {0x00,0x00,0x00};

/*********************************************************************//**
 *
 * Check CAN msg and CAN lost msg, Do average of CAN signal and calculate
*  Stepper target position for BattVolt gauge
 *
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
void vBattVoltDataProcessing()
{
	uint32_t data = 0;
	uint32_t temp = 0;
	uint8_t i = 0;
    static uint8_t AvgSampleCount = 0;						// Counter for averaging samples


	data = u32GetPGNdata(eBattVolt);
	data = ((data * 50) / 100);

	/* Check CAN message lost */
	if((bGetPGNTimeOut(eBattVolt) == TRUE) || (u32GetPGNdata(eBattVolt) >= BADDATA_2BYTE))
	{
		data = 0;
	}
	else
	{
		//Go back if new sample is not coming

	}

	//Averging of signal
	sBattVolt.u16BattVoltSignalData[AvgSampleCount] = (uint16_t)data;

	if(++AvgSampleCount >= BattVolt_SIGNAL_AVG)
	{
		AvgSampleCount =0;
	}
	data = 0;
	for(i = 0; i < BattVolt_SIGNAL_AVG; i++)
	data = data + sBattVolt.u16BattVoltSignalData[i];
	data = data / BattVolt_SIGNAL_AVG;
	sBattVolt.u32BattVolt = (uint16_t)data;

	BattVoltvalue = (uint16_t)sBattVolt.u32BattVolt;

	if(BattVoltvalue > 999)
	{
		BattVoltvalue = 999;
	}

}



//typedef struct {
//	uint16_t u16SignalData[GAUGE_AVG_SIZE];		//Use for ADC sample averaging
//	uint8_t u8Value;							//Current value of voltmeter
//	uint8_t u8AvgCount;  					//Counter for averaging ADC sample
//} Voltage_TYPE;
//
//Voltage_TYPE sBattVolt = { { 0x00 }, 0x00, 0x00 };
//
//void battery_volt_Process(void)
//{
//
//	uint32_t data = 0;
//	uint32_t BattVolt = 0;
//	static uint16_t PreviousBattVolt = 0;
//	uint8_t i = 0;
//	static uint16_t AvgCount = GAUGE_AVG_SIZE - 1;
//	static uint8_t FirstEntry = TRUE;
//	uint8_t fractionPart = 0;
//	static uint8_t HysFlag = 0;
//
//	static uint32_t CrackingWatingTime = 0;
//	static uint8_t VoltageUpdateFlag = 0;
//
//	data = adc_monitor(5);	//Sample value getting place;
//
//	if (FirstEntry) {
//		for (i = 0; i < GAUGE_AVG_SIZE; i++)
//			sBattVolt.u16SignalData[i] = data;
//		AvgCount = GAUGE_AVG_SIZE - 1;
//
//	}
//
//	sBattVolt.u16SignalData[AvgCount] = data;
//	data = 0;
//	for (i = 0; i < GAUGE_AVG_SIZE; i++)
//		data = data + sBattVolt.u16SignalData[i];
//
//	if (++AvgCount >= GAUGE_AVG_SIZE)
//	{
//		AvgCount = 0;
//		data = data / GAUGE_AVG_SIZE;
//		vGetCurveDataValue(eBattVoltCurve, data, &BattVolt);
//		//This is filter when voltage up down suddenly then its works
//		FirstEntry = FALSE;
//		//fractionPart = BattVolt%10;
//		sBattVolt.u8Value = (uint8_t) (BattVolt / 10);
//	}
//
//
//
//}
//



uint16_t get_battery_volt(void)
{
	return BattVoltvalue;
}
