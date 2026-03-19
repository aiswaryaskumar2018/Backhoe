/*
 * DieselParticulateFilter.c
 *
 *  Created on: Oct 24, 2024
 *      Author: dharmendra.singh
 */




/*
 * Dpf.c
 *
 *  Created on: Sep 28, 2024
 *      Author: dharmendra.singh
 */

#include "J1939.h"
#include "DieselParticulateFilter.h"


#define Dpf_SIGNAL_AVG              4
#define Dpf_HYS						10		//Dpf


uint16_t Dpfvalue = 0;



// local declarations --------------------------------------------------------
typedef struct
{
	uint16_t     u16EngineDpf;
	uint16_t     u16DpfSignalData[Dpf_SIGNAL_AVG];
	uint16_t	u16EngineOverSpeed;
} Dpf_DATA_TYPE;

Dpf_DATA_TYPE sDpf = {0x00,0x00,0x00};

/*********************************************************************//**
 *
 * Check CAN msg and CAN lost msg, Do average of CAN signal and calculate
*  Stepper target position for Dpf gauge
 *
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
void vDpfDataProcessing()
{
	uint32_t data = 0u;
	uint32_t temp = 0u;
	uint8_t i = 0u;
    static uint8_t AvgSampleCount = 0u;						// Counter for averaging samples

	data = u32GetPGNdata(eDieselParticulateFilter);


	//Check CAN message lost
	if(bGetPGNTimeOut(eDieselParticulateFilter) || data >= BADDATA_2BYTE)
	{
		data = 0;
	}
	else
	{
		//Go back if new sample is not coming
		if(! bGetPGNDataReady(eDieselParticulateFilter))
		{
			return;
		}
		ClearPGNDataReady(eDieselParticulateFilter);
	}

	//Averging of signal
	sDpf.u16DpfSignalData[AvgSampleCount] = (uint16_t)data;

	if(++AvgSampleCount >= Dpf_SIGNAL_AVG)
	{
		AvgSampleCount =0;
	}
	data = 0;
	for(i = 0; i < Dpf_SIGNAL_AVG; i++)
	data = data + sDpf.u16DpfSignalData[i];
	data = data / Dpf_SIGNAL_AVG;
	sDpf.u16EngineDpf = (uint16_t)data;

	Dpfvalue = sDpf.u16EngineDpf;


}

/*********************************************************************//**
 *
 * Current engine speed of vehicle
 *
 * @param	None
 *
 * @return	Current Engine speed
 *************************************************************************/

uint16_t u16GetDpfValue(void)
{
	return Dpfvalue;
}





