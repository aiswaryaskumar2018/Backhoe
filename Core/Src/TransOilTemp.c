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
#include "adc.h"
#include "CurveHeader.h"
#include "TransOilTemp.h"
#include "J1939.h"
#include "main.h"


// Constant declarations -----------------------------------------------------




#define OPEN_CKT_VALUE    				6911
#define SHORT_CKT_VALUE    				124
#define HYSTERSIS_GAP_ADC_COUNT			10u


/* Constant declarations -----------------------------------------------------*/
#define TRANS_OIL_TEMP_SIGNAL_AVG_SIZE		16u
#define TRANS_OIL_TEMP_ADC_AVG_SIZE			16u

#define TRANS_OIL_TEMP_BAR_ON_1			20u
#define TRANS_OIL_TEMP_BAR_ON_2			80u
#define TRANS_OIL_TEMP_BAR_ON_3			100u	//146u
#define TRANS_OIL_TEMP_BAR_ON_4			110u	//124u		//122u
#define TRANS_OIL_TEMP_BAR_ON_5			120u		//110u		//103u
#define TRANS_OIL_TEMP_BAR_ON_6			150u		//96u		//87u

#define TRANS_OIL_TEMP_BAR_OFF_1 		20u
#define TRANS_OIL_TEMP_BAR_OFF_2		TRANS_OIL_TEMP_BAR_ON_2 - 1u
#define TRANS_OIL_TEMP_BAR_OFF_3		TRANS_OIL_TEMP_BAR_ON_3 - 1u
#define TRANS_OIL_TEMP_BAR_OFF_4		TRANS_OIL_TEMP_BAR_ON_4 - 1u
#define TRANS_OIL_TEMP_BAR_OFF_5		TRANS_OIL_TEMP_BAR_ON_5 - 1u
#define TRANS_OIL_TEMP_BAR_OFF_6		TRANS_OIL_TEMP_BAR_ON_6 - 1u

#define CAN_ID_TRX_OIL_TEMP_REQUEST 			0x18FEF821



/* local declarations --------------------------------------------------------*/
struct
{
    uint32_t    u32TrasOilValue;      						/* Hold Currect value of Gauge */
    uint32_t    u32TRANS_OIL_TEMPSignalData[TRANS_OIL_TEMP_SIGNAL_AVG_SIZE];    /* Array for averaging signal samples */
    uint16_t	u16OpenCircuit;								/* Hold open circuit Value  */
    uint8_t     u8AvgCount;                    			    /* used in averaging samples */
    uint8_t     bWarningBuzzerStatus;							/* set True  for switchout */
    uint8_t		u8TRANS_OIL_TEMPBarNo;
    uint16_t	u16TRANS_OIL_TEMPSignalValue;
    uint16_t	u16TRANS_OIL_TEMPPreviousSignal;
    uint32_t    u32TRANS_OIL_TEMPSignalADCData[TRANS_OIL_TEMP_ADC_AVG_SIZE];    /* Array for Averaging Signal Samples */
} sTransOilTemp = { 0, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},	0,	0,	FALSE,0u,0,0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

uint8_t TxData_TransOilTemperature[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};


void vTransOIL_TempAnlaogProcess(void);
uint16_t GetAveragingAdcValue(uint16_t val);
uint16_t TempAdcValue = 0;
uint8_t OpenCkt_Flag = FALSE;
uint32_t Temp_value = 0;
uint32_t PreviousDataTransOilTemp = 0;
uint8_t hys_val = 0;




void vTransOIL_TempAnlaogProcess(void)
{
	uint8_t i = 0;
    uint8_t j = 0;
    uint32_t AdcValue = 0;
    uint32_t Tempdata = 0;
    uint32_t data = 0;
    static uint8_t AvgCount = 0;
    static uint8_t AvgCountAdc = 0;
    static uint8_t FirstEntry = TRUE;



	// Take ADC value ------------------------------------------------*/
    Tempdata = GetADCValueTransOilTemp();
    AdcValue =  GetAveragingAdcValue(Tempdata);

//	SetAdcValueForDisplay(AdcValue);

    if(FirstEntry == TRUE)
    {
        for(j = 0; j < TRANS_OIL_TEMP_ADC_AVG_SIZE; j++)
            sTransOilTemp.u32TRANS_OIL_TEMPSignalADCData[j]  = AdcValue;
    }
    sTransOilTemp.u32TRANS_OIL_TEMPSignalADCData[AvgCountAdc] = AdcValue;
    // calculate moving average of 8 samples
    AdcValue = 0U;
    for(j = 0U; j < TRANS_OIL_TEMP_ADC_AVG_SIZE; j++)
    {
        AdcValue = AdcValue + sTransOilTemp.u32TRANS_OIL_TEMPSignalADCData[j];
    }

    AdcValue = AdcValue / TRANS_OIL_TEMP_ADC_AVG_SIZE;
    if(++AvgCountAdc >= TRANS_OIL_TEMP_ADC_AVG_SIZE)
    {
        AvgCountAdc = 0U;
    }

    data = AdcValue;

    /* Calculate resistance Input------------- */
    if(FirstEntry == TRUE)
    {
        for(i = 0; i < TRANS_OIL_TEMP_SIGNAL_AVG_SIZE; i++)
        {
            sTransOilTemp.u32TRANS_OIL_TEMPSignalData[i]  = data;
        }
        FirstEntry = FALSE;
    }

    sTransOilTemp.u32TRANS_OIL_TEMPSignalData[AvgCount] = data;
    /* Average of ADC samples ------------*/
    data = 0u;

    for(i = 0; i < TRANS_OIL_TEMP_SIGNAL_AVG_SIZE; i++)
        data = data + sTransOilTemp.u32TRANS_OIL_TEMPSignalData[i];
    data = data / TRANS_OIL_TEMP_SIGNAL_AVG_SIZE;

  	if(++AvgCount >= TRANS_OIL_TEMP_SIGNAL_AVG_SIZE)
    {
        for(i = 0; i < TRANS_OIL_TEMP_SIGNAL_AVG_SIZE - 1; i++)
            sTransOilTemp.u32TRANS_OIL_TEMPSignalData[i]=  sTransOilTemp.u32TRANS_OIL_TEMPSignalData[i+1];
        AvgCount = TRANS_OIL_TEMP_SIGNAL_AVG_SIZE - 1;
    }
  	
  	vGetCurveDataValue(eLevel_OilTemp,data,&sTransOilTemp.u16TRANS_OIL_TEMPSignalValue); /* Pass ADC avg. value for find resistance value from curve*/

	if (sTransOilTemp.u16TRANS_OIL_TEMPSignalValue > 20u)
	{
		hys_val = 1;
	}
	else
	{
		hys_val = 2;
	}

  	if (sTransOilTemp.u16TRANS_OIL_TEMPSignalValue > PreviousDataTransOilTemp) /* Plus side */
  	{
  		if ((sTransOilTemp.u16TRANS_OIL_TEMPSignalValue - PreviousDataTransOilTemp) > hys_val ||
  		    sTransOilTemp.u16TRANS_OIL_TEMPSignalValue >= 150u) // Allow small rise if ≥150
  		{
  			PreviousDataTransOilTemp = sTransOilTemp.u16TRANS_OIL_TEMPSignalValue;
  		}
  		else
  		{
  			sTransOilTemp.u16TRANS_OIL_TEMPSignalValue = PreviousDataTransOilTemp;
  		}
  	}
  	else if (sTransOilTemp.u16TRANS_OIL_TEMPSignalValue < PreviousDataTransOilTemp) /* Minus side */
  	{
  		if ((PreviousDataTransOilTemp - sTransOilTemp.u16TRANS_OIL_TEMPSignalValue) > hys_val ||
  		    sTransOilTemp.u16TRANS_OIL_TEMPSignalValue <= 0u) // Allow small drop if ≤20
  		{
  			PreviousDataTransOilTemp = sTransOilTemp.u16TRANS_OIL_TEMPSignalValue;
  		}
  		else
  		{
  			sTransOilTemp.u16TRANS_OIL_TEMPSignalValue = PreviousDataTransOilTemp;
  		}
  	}
  	else
  	{
  		/* No change */
  	}

    sTransOilTemp.u32TrasOilValue =  sTransOilTemp.u16TRANS_OIL_TEMPSignalValue;
    Temp_value =  sTransOilTemp.u32TrasOilValue;
    Temp_value = (Temp_value + 273u);
    Temp_value = ((Temp_value * 100000u) / 3125u);

    if(sTransOilTemp.u32TrasOilValue >= 110 && OpenCkt_Flag == FALSE)
    {
    	sTransOilTemp.bWarningBuzzerStatus = eStatusON;
    }
    else if(sTransOilTemp.u32TrasOilValue <= 100 && OpenCkt_Flag == FALSE)
    {
    	sTransOilTemp.bWarningBuzzerStatus = eStatusOff;
    }

    if(OpenCkt_Flag == FALSE)
    {
		/* Asscending order -----------------------------------------------------------------*/
		//if(sTransOilTemp.u16TRANS_OIL_TEMPSignalValue !=  (uint16_t)sTransOilTemp.u16TRANS_OIL_TEMPSignalValue)
		//{
			//sTransOilTemp.u16TRANS_OIL_TEMPSignalValue = sTransOilTemp.u16TRANS_OIL_TEMPSignalValue;
			//Ascending inputs
			if(sTransOilTemp.u8TRANS_OIL_TEMPBarNo == 1u && sTransOilTemp.u16TRANS_OIL_TEMPSignalValue < TRANS_OIL_TEMP_BAR_ON_1)
				sTransOilTemp.u8TRANS_OIL_TEMPBarNo = 0;
			else if(sTransOilTemp.u8TRANS_OIL_TEMPBarNo <=1u && sTransOilTemp.u16TRANS_OIL_TEMPSignalValue >= TRANS_OIL_TEMP_BAR_ON_1 && sTransOilTemp.u16TRANS_OIL_TEMPSignalValue < TRANS_OIL_TEMP_BAR_ON_2 )
				sTransOilTemp.u8TRANS_OIL_TEMPBarNo =1;
			else if(sTransOilTemp.u8TRANS_OIL_TEMPBarNo <=2u && sTransOilTemp.u16TRANS_OIL_TEMPSignalValue >= TRANS_OIL_TEMP_BAR_ON_2 && sTransOilTemp.u16TRANS_OIL_TEMPSignalValue < TRANS_OIL_TEMP_BAR_ON_3 )
				sTransOilTemp.u8TRANS_OIL_TEMPBarNo =2;
			else if(sTransOilTemp.u8TRANS_OIL_TEMPBarNo <=3u && sTransOilTemp.u16TRANS_OIL_TEMPSignalValue >= TRANS_OIL_TEMP_BAR_ON_3 && sTransOilTemp.u16TRANS_OIL_TEMPSignalValue < TRANS_OIL_TEMP_BAR_ON_4 )
				sTransOilTemp.u8TRANS_OIL_TEMPBarNo =3;
			else if(sTransOilTemp.u8TRANS_OIL_TEMPBarNo <=4u && sTransOilTemp.u16TRANS_OIL_TEMPSignalValue >= TRANS_OIL_TEMP_BAR_ON_4 && sTransOilTemp.u16TRANS_OIL_TEMPSignalValue < TRANS_OIL_TEMP_BAR_ON_5 )
				sTransOilTemp.u8TRANS_OIL_TEMPBarNo =4;
			else if(sTransOilTemp.u8TRANS_OIL_TEMPBarNo <=5u && sTransOilTemp.u16TRANS_OIL_TEMPSignalValue >= TRANS_OIL_TEMP_BAR_ON_5 && sTransOilTemp.u16TRANS_OIL_TEMPSignalValue < TRANS_OIL_TEMP_BAR_ON_6 )
				sTransOilTemp.u8TRANS_OIL_TEMPBarNo =5;
			else if(sTransOilTemp.u8TRANS_OIL_TEMPBarNo <=6u && sTransOilTemp.u16TRANS_OIL_TEMPSignalValue >= TRANS_OIL_TEMP_BAR_ON_6 )
				sTransOilTemp.u8TRANS_OIL_TEMPBarNo =6u;

			//Descending inputs
			else if(sTransOilTemp.u8TRANS_OIL_TEMPBarNo > 0u && sTransOilTemp.u16TRANS_OIL_TEMPSignalValue <= TRANS_OIL_TEMP_BAR_OFF_1)
				sTransOilTemp.u8TRANS_OIL_TEMPBarNo = 0u;
			else if(sTransOilTemp.u8TRANS_OIL_TEMPBarNo > 1u && sTransOilTemp.u16TRANS_OIL_TEMPSignalValue <= TRANS_OIL_TEMP_BAR_OFF_2)
				sTransOilTemp.u8TRANS_OIL_TEMPBarNo = 1u;
			else if(sTransOilTemp.u8TRANS_OIL_TEMPBarNo > 2u && sTransOilTemp.u16TRANS_OIL_TEMPSignalValue <= TRANS_OIL_TEMP_BAR_OFF_3)
				sTransOilTemp.u8TRANS_OIL_TEMPBarNo = 2u;
			else if(sTransOilTemp.u8TRANS_OIL_TEMPBarNo > 3u && sTransOilTemp.u16TRANS_OIL_TEMPSignalValue <= TRANS_OIL_TEMP_BAR_OFF_4)
				sTransOilTemp.u8TRANS_OIL_TEMPBarNo = 3u;
			else if(sTransOilTemp.u8TRANS_OIL_TEMPBarNo > 4u && sTransOilTemp.u16TRANS_OIL_TEMPSignalValue <= TRANS_OIL_TEMP_BAR_OFF_5)
				sTransOilTemp.u8TRANS_OIL_TEMPBarNo = 4u;
			else if(sTransOilTemp.u8TRANS_OIL_TEMPBarNo > 5u && sTransOilTemp.u16TRANS_OIL_TEMPSignalValue <= TRANS_OIL_TEMP_BAR_OFF_6)
				sTransOilTemp.u8TRANS_OIL_TEMPBarNo = 5u;

		//}
    }


}


uint16_t GetTransOilTempCANTxData(void)
{
	return Temp_value;
}


uint8_t GetTransOilTempVaLue(void)
{
	return (uint8_t)sTransOilTemp.u32TrasOilValue;
}

uint8_t GetTransOilTempWarningBuzzerStatus()
{
	return sTransOilTemp.bWarningBuzzerStatus;
}


uint8_t GetTransOilTempBarValueInplaceOfAdBlueGauge(void)
{
	return sTransOilTemp.u8TRANS_OIL_TEMPBarNo;
}

uint16_t GetAveragingAdcValue(uint16_t val)
{
	uint32_t sum = 0u;

	for (int i = 0; i < 40; i++)
	{
		sum += val;
	}
	return (uint16_t)(sum / 40);
}

