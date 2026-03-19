/** ###################################################################
**     Filename  : CurveHeader.h
**     Project   : Y1ClusterDiagnostics
**     Processor : MC9S12XHZ256VAG
**     Compiler  : CodeWarrior HCS12X C Compiler
**     Date/Time : 25/01/2018, 14:28
**     Contents  :
**         User source code
**
** ###################################################################*/

#ifndef __CurveHeader_H
#define __CurveHeader_H

/* MODULE CurveHeader */
#include "main.h"

// public interface ---------------------------------------------------------
typedef enum
{
    eBattVoltCurve = 0U,
	eOilTempResCurve,
    eResFuelCurve,
	eLevel_OilTemp,
    eAirPressureSensor,
    eCapacitiveFuelVolt,
    eFuelCAN_TX,
    eNumofCurveTypes
} eCurveType;

void vGetCurveDataValue(eCurveType, uint32_t, uint32_t *);
 
/* END CurveHeader */

#endif

