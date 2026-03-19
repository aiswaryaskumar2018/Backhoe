/*
 * RpmMeter.c
 *
 *  Created on: Sep 29, 2024
 *      Author: dharmendra.singh
 */
#include "J1939.h"
#include "RpmMeter.h"


#define RPM_HYS							10		//RPM
#define MAX_RPM_VAL						3000u

uint16_t RPMvalue = 0u;
uint16_t PreviousDataRPM = 0u;


/*********************************************************************//**
 *
 * Check CAN msg and CAN lost msg, Do average of CAN signal and calculate
*  Stepper target position for RPM gauge
 *
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
void vRpmDataProcessing()
{
	uint32_t data = 0;

	data = u32GetPGNdata(eJTachoMeter);
	//Check CAN message lost
	if(bGetPGNTimeOut(eJTachoMeter) || data >= BADDATA_2BYTE)
	{
		data = 0;
	}
	else
	{

	}

	RPMvalue = (uint16_t)((data * 125)/1000u);

  	if (RPMvalue > PreviousDataRPM) /* Plus side */
  	{
  		if ((RPMvalue - PreviousDataRPM) > RPM_HYS || RPMvalue >= MAX_RPM_VAL) // Allow small rise if ≥3000
  		{
  			PreviousDataRPM = RPMvalue;
  		}
  		else
  		{
  			RPMvalue = PreviousDataRPM;
  		}
  	}
  	else if (RPMvalue < PreviousDataRPM) /* Minus side */
  	{
  		if ((PreviousDataRPM - RPMvalue) > RPM_HYS || RPMvalue < 1u) // Allow small drop if ≤=1
  		{
  			PreviousDataRPM = RPMvalue;
  		}
  		else
  		{
  			RPMvalue = PreviousDataRPM;
  		}
  	}
  	else
  	{
  		/* No change */
  	}

	if(RPMvalue > MAX_RPM_VAL)
	{
		RPMvalue = MAX_RPM_VAL;
	}

}

/*********************************************************************//**
 *
 * Current engine speed of vehicle
 *
 * @param	None
 *
 * @return	Current Engine speed
 *************************************************************************/
uint16_t u16GetEngineSpeed(void)
{
	return RPMvalue;
}

