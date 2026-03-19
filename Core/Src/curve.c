/*
 * curve.c
 *
 *  Created on: Sep 18, 2024
 *      Author: aiswarya.kumar
 */

/** ###################################################################
**     Filename  : Curve.c
**     Project   : Y1ClusterDiagnostics
**     Processor : MC9S12XHZ256VAG
**     Compiler  : CodeWarrior HCS12X C Compiler
**     Date/Time : 25/01/2018, 14:28
**     Contents  :
**         User source code
**
** ###################################################################*/

/* MODULE Curve */


#include "CurveHeader.h"

// constant declarations -----------------------------------------------------

// Each of these tables may be constructed using any number of entries and any
// width needed thereby providing a process to create translations at any
// level of accuracy needed.


#define ROWS_BATT_VOLT       24U
#define COLS_BATT_VOLT       2U

//Lookup table for Battery Voltage*10 to be display on LCD
const uint32_t wBatt[ROWS_BATT_VOLT][COLS_BATT_VOLT] =
{
     //ADC VALUE    //BATT VOLT*10	//Reference Volt - 4.3
	{	125 	,	90  },
	{	139 	,	100 },
	{	154 	,	110 },
	{	169 	,	120 },
	{	184 	,	130 },
	{	200 	,	140 },
	{	214 	,	150 },
	{	229 	,	160 },
	{	243 	,	170 },
	{	259 	,	180 },
	{	273 	,	190 },
	{	288 	,	200 },
	{	304 	,	210 },
	{	318 	,	220 },
	{	333 	,	230 },
	{	346 	,	240 },
	{	363	    ,	250 },
	{	377 	,	260 },
	{	392 	,	270 },
	{	407 	,	280 },
	{	423 	,	290 },
	{	436 	,	300 },
	{	452 	,	310 },
	{	468 	,	320 },

};


#define ROWS_RES_LEVEL       18U
#define COLS_RES_LEVEL       2U

//Lookup table for  Resistive fuel gauge to calulate level ; for broadcast over CAN
const uint32_t wResLevel[ROWS_RES_LEVEL][COLS_RES_LEVEL] =
{
	//  Res*10  	Level
	{	0 	,	40	},
	{	250 ,	30	},
	{	365	,	56	},
	{	432	,	76	},
	{	486	,	96	},
	{	516 ,	110	},
	{	554	,	130	},
	{	570	,	140	},
	{	600	,	160	},
	{	628 ,	182	},
	{	648	,	200	},
	{	691	,	250	},
	{	725	,	300	},
	{	734 ,	314	},
	{	752	,	350	},
	{	880	,	1000},
	{	920	,	2000},
	{	927 ,	2431},

};


#define ROWS_RES_OIL_TEMP       33U
#define COLS_RES_OIL_TEMP       3U

//used for old hardware ;Lookup table for calculate Resistance ohms * 10 for Fuel,AP,OP gauges
const uint32_t wResOilTEmp[ROWS_RES_OIL_TEMP][COLS_RES_OIL_TEMP] =
{
	{	1u		,1 ,	150 },
	{	118		,8,		150 },// 8 ohms
	{	760		,56,	150 },//150, 56
	{	859		,65,	145 },//145,65
	{	977		,74,	140 },//140,74
	{	1110	,83,	135 },//135,83
	{	1200	,92,	130 },//130,92
	{	1288	,101,	125 },//125,101
	{	1366 	,110,	120 },//120,110
	{	1520	,125,	115 },//115,125
	{	1688 	,140,	110 },//110,140
	{	1880	,161,	105 },//105,161
	{	2080 	,182,	100 },//100,182
	{	2318	,215,	95  },//95,215
	{	2566	,248,	90  },//90,248
	{	2810	,281,	85  },//85,281
	{	3002 	,314,	80  },//80,314
	{	3870	,490,	75  },//75,490
	{	4510	,667,	70  },//70,666.8
	{	4939	,843,	65  },//65,843
	{	5339	,1019,	60  },//60,1019
	{	5600	,1196,	55  },//55,1196
	{	5849	,1372,	50  },//50,1372
	{	6038	,1549,	45  },//45,1548.9
	{	6220	,1725,	40  },//40,1725
	{	6344	,1901,	35  },//35,1901
	{	6470	,2078,	30  },//30,2078
	{	6571	,2254,	25  },//25,2254
	{	6615 	,2431,	20  },//20,2431
	{	6708	,2607,	15  },//15,2607
	{	6788	,2783,	10  },//10,2783
	{	6847	,2960,	5   },//5,2960
	{	6911 	,3136,	0   },//0,3136
};


#define ROWS_RES_1       30U
#define COLS_RES_1       2U

//used for New hardware ;Lookup table for calculate Resistance ohms*10 for Fuel,AP,OP gauges
const uint32_t wResFuel[ROWS_RES_1][COLS_RES_1] =
{
	{	3  	,	0	},
	{	800	    ,	10	},
	{	880	    ,	12	},
	{	1130	,	15	},
	{	1450  	,	20 	},
	{	2053  	,	30 	},
	{	2550  	,	40 	},
	{	2984	,   50  },
	{	3262	,   57  },
	{	3705 	,	70 	},
	{	3995 	,	80 	},
	{	4270 	,	90 	},
	{	4590 	,	104 },
	{	4812 	,	114 },
	{	4995 	,	124	},
	{	5185 	,	134	},
	{	5355 	,	145	},
	{	5500 	,	155	},
	{	5630 	,	165	},
	{	5775 	,	175	},
	{	5810 	,	178	},
	{	5830 	,	180	},
	{	5854 	,	182	},
	{	5864 	,	183	},
	{	5877 	,	184	},
	{	6048 	,	200	},
	{	7109 	,	350	},
	{	7597 	,	480	},
	{	7655 	,	500	},
	{	7796 	,	550	},

};

#define ROWS_FUEL_GAUGE_LEVEL       8U
#define COLS_FUEL_GAUGE_LEVEL       2U
//Lookup table for Capacitive fuel gauge to caculate level ; for broadcast over CAN
const uint32_t wFuelGaugeLevel[ROWS_FUEL_GAUGE_LEVEL][COLS_FUEL_GAUGE_LEVEL] =
{
	//  ADC Value  	CAN Tx Val
	{	15 	 	,	0		},//0% 10ohms
	{	57 		,	62		},//25% 57ohms
	{	80		,	94   	},//37% 80ohms
	{	104		,	125		},//50% 104ohms
	{	124		,	156	    },//62% 124ohms
	{	145		,	187		},//75% 145ohms
	{	165		,	218	    },//87% 165ohms
	{	180		,	250		}, //100% 184ohms

};

#define ROWS_AP_LEVEL       28U
#define COLS_AP_LEVEL       2U

//Lookup table for Capacitive fuel gauge to calulate level ; for broadcast over CAN
const uint32_t wApPressureADCLevel[ROWS_AP_LEVEL][COLS_AP_LEVEL] =
{
	{	3  	,	0	},
	{	800	    ,	10	},
	{	1450  	,	20 	},
	{	2053  	,	30 	},
	{	2550  	,	40 	},
	{	2984	,   50  },
	{	3262	,   57  },
	{	3705 	,	70 	},
	{	3995 	,	80 	},
	{	4270 	,	90 	},
	{	4590 	,	104 },
	{	4812 	,	114 },
	{	4995 	,	124	},
	{	5185 	,	134	},
	{	5355 	,	145	},
	{	5500 	,	155	},
	{	5630 	,	165	},
	{	5775 	,	175	},
	{	5810 	,	178	},
	{	5830 	,	180	},
	{	5854 	,	182	},
	{	5864 	,	183	},
	{	5880 	,	184	},
	{	6048 	,	200	},
	{	7109 	,	350	},
	{	7597 	,	480	},
	{	7655 	,	500	},
	{	7796 	,	550	},


};




#define ADC_VOLT_ROWS	51U
#define ADC_VOLT_COLS	2U

//Lookup table for  Capacitive fuel gauge to calulate voltage ; for broadcast over CAN
const uint32_t wAdcToVoltage[ADC_VOLT_ROWS][ADC_VOLT_COLS] =
{
	 //ADC    //Volt (mV)
	{	0	,	0		},
	{	22	,	100		},
	{	44	,	200		},
	{	67	,	300		},
	{	90	,	400		},
	{	112	,	500		},
	{	134	,	600		},
	{	157	,	700		},
	{	179	,	800		},
	{	202	,	900		},
	{	224	,	1000	},
	{	247	,	1100	},
	{	269	,	1200	},
	{	292	,	1300	},
	{	314	,	1400	},
	{	336	,	1500	},
	{	359	,	1600	},
	{	381	,	1700	},
	{	404	,	1800	},
	{	426	,	1900	},
	{	449	,	2000	},
	{	471	,	2100	},
	{	494	,	2200	},
	{	516	,	2300	},
	{	539	,	2400	},
	{	561	,	2500	},
	{	583	,	2600	},
	{	606	,	2700	},
	{	628	,	2800	},
	{	651	,	2900	},
	{	673	,	3000	},//
	{	651	,	3100	},//
	{	545	,	3200	},
	{	742	,	3300	},//
	{	763	,	3400	},//
	{	787	,	3500	},//
	{	808	,	3600	},//
	{	831	,	3700	},//
	{	854	,	3800	},//
	{	875	,	3900	},//
	{	897	,	4000	},//
	{	917	,	4100	},//
	{	931	,	4200	},//
	{	941	,	4300	},//
	{	946	,	4400	},
	{	951	,	4500	},
	{	954	,	4600	},
	{	957	,	4700	},
	{	959	,	4800	},//
	{	961	,	4900	},
	{	963	,	5000	},
};



// local declarations --------------------------------------------------------
// These are variables used for the linear interprolation calculation.
// They are defined here to save stack space during use.
int32_t   R1;
int32_t   R2;
int32_t   R3;
int32_t   V1;
int32_t   V2;
int32_t   V3;

int32_t overrange;

// private prototypes --------------------------------------------------------
static int16_t _iGetTableValue(uint32_t const *, uint32_t, uint32_t *, uint8_t, uint8_t, uint8_t);


/*****************************************************************************
 **  Description:
 **      Returns the value from specified table.
 **
 ** ---------------------------------------------------------------------------
 **  Parameters:     table ID,
 **                  resistance value
 **                  pointer to word to store the value
 **
 **  Return value:   amount of overrange as:
 **                      0   returned value is within the table range,
 **                      < 0 returned value is below the lowest value in the table,
 **                      > 0 returned value is above the highest value in the table
 **
 *****************************************************************************/
void vGetCurveDataValue(eCurveType curve, uint32_t inVal, uint32_t *value)
{
	//int overrange; // overrange can use in future if required
	if(inVal == 0)
	{
		inVal = 1;
	}
	switch (curve)
	{
		case eBattVoltCurve :
			overrange = _iGetTableValue((uint32_t *)wBatt, inVal, value, ROWS_BATT_VOLT, COLS_BATT_VOLT, 1);

			break;
		case eLevel_OilTemp :
			overrange = _iGetTableValue((uint32_t *)wResOilTEmp, inVal, value, ROWS_RES_OIL_TEMP, COLS_RES_OIL_TEMP, 2);

		break;
		case eResFuelCurve :
				overrange = _iGetTableValue((uint32_t *)wResFuel, inVal, value, ROWS_RES_1, COLS_RES_1, 1);
		break;
		case eOilTempResCurve :
			overrange = _iGetTableValue((uint32_t *)wResOilTEmp, inVal, value, ROWS_RES_LEVEL, COLS_RES_LEVEL, 1);

		break;
		case eAirPressureSensor :
			overrange = _iGetTableValue((uint32_t *)wApPressureADCLevel, inVal, value, ROWS_AP_LEVEL, COLS_AP_LEVEL, 1);

		break;
		case eCapacitiveFuelVolt :
			overrange = _iGetTableValue((uint32_t *)wAdcToVoltage, inVal, value, ADC_VOLT_ROWS, ADC_VOLT_COLS, 1);

		break;
		case eFuelCAN_TX :
			overrange = _iGetTableValue((uint32_t *)wFuelGaugeLevel, inVal, value, ROWS_FUEL_GAUGE_LEVEL, COLS_FUEL_GAUGE_LEVEL, 1);

		break;

		default :
			*value = (uint32_t)-1;
			overrange = 0;
			break;
		}

}



/*****************************************************************************
 **  Description:
 **      This functi0n finds the closest match in the table for the given
 **      value. Linear interprolation is performed between values. It will
 **      set the amount of overrange.
 **
 **      What this function will handle:
 **          Tables of any size up to 256 x 256
 **          Tables with increasing or decreasing values in any of the columns.
 **
 **      Linear Interprolation Algorithm
 **           table where,
 **                  R = the resistance or other input value
 **                  V = the converted value (pressure, temperature, etc)
 **                 R  |  V
 **                 -------
 **                 R1 | V1 (table values)
 **   (input value) R2 | V2 (the calculated output value)
 **                 R3 | V3 (table values)
 **
 **                  (R2 - R1) * (V3 - V1)
 **          V2 =    --------------------- + V1
 **                        (R3 - R1)
 **
 **      What this function will not handle:
 **          Tables with negative numbers.
 **
 ** ---------------------------------------------------------------------------
 **  Parameters:     table pointer,
 **                  input value,
 **                  pointer to word for value storage
 **                  number of rows in the table,
 **                  number of columns in the table,
 **                  column index
 **
 **  Return value:   an integer indicating the percent of +- overrange
 **
 *****************************************************************************/
static int16_t _iGetTableValue(uint32_t const  *tabPtr,  uint32_t inVal,  uint32_t *valPtr, uint8_t numOfRows, uint8_t numOfCols, uint8_t colIndex)
{
	int16_t i;
	uint8_t withinTableLimits = FALSE;

	if(inVal == 0)
	{
		inVal = 1;
	}
	if (tabPtr[numOfCols] > tabPtr[0])
	{
		// Table first column is increasing
		if (inVal <= tabPtr[0])
		{
			// Get the value and set the amount of overrange
			*valPtr = tabPtr[0 + colIndex];
			overrange = (((int32_t)inVal - (int32_t)tabPtr[0]) * 100) / (int32_t)tabPtr[0];
		}
		else if (inVal >= tabPtr[(numOfRows-1) * numOfCols])
		{
			// Get the value and set the amount of overrange
			*valPtr = tabPtr[(numOfRows-1) * numOfCols + colIndex];
			overrange = (((int32_t)inVal - (int32_t)tabPtr[(numOfRows-1) * numOfCols]) * 100) / (int32_t)tabPtr[(numOfRows-1) * numOfCols];
		}
		else {
			// Set the index
			withinTableLimits = TRUE;
			i = 0;
			while (tabPtr[i * numOfCols] < inVal)
				++i;
		}
	}
	else {
		// Table first column is decreasing
		if (inVal >= tabPtr[0])
		{
			// Get the value and set the amount of overrange
			*valPtr = tabPtr[0 + colIndex];
			overrange = (((int32_t)inVal - (int32_t)tabPtr[0]) * 100) / (int32_t)tabPtr[0];
		}
		else if (inVal <= tabPtr[(numOfRows-1) * numOfCols])
		{
			// Get the value and set the amount of overrange
			*valPtr = tabPtr[(numOfRows-1) * numOfCols + colIndex];
			overrange = (((int32_t)inVal - (int32_t)tabPtr[(numOfRows-1) * numOfCols]) * 100) / (int32_t)tabPtr[(numOfRows-1) * numOfCols];
		}
		else {
			// Set the index
			withinTableLimits = TRUE;
			i = 0;
			while (tabPtr[i * numOfCols] > inVal)
				++i;
		}
	}

	if (withinTableLimits)
	{
		// It is not beyond the table limits
		overrange = 0;

		i *= numOfCols; // index of R1

		// Use these variables to save stack space during calculation
		R1 = (int32_t)tabPtr[i];
		R3 = (int32_t)tabPtr[i-numOfCols];
		V1 = (int32_t)tabPtr[i+colIndex];
		V3 = (int32_t)tabPtr[i-numOfCols+colIndex];
		R2 = (int32_t)inVal;

		V2 = ((100 * ((R2 - R1) * (V3 - V1))) / (R3 - R1)) + (100 * V1);
		*valPtr = (int32_t)(V2 / 100);
	}

	return (int32_t)overrange;
}



/* END Curve */



