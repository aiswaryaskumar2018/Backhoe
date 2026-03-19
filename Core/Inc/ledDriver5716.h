
#ifndef __LedDriver5716_H
#define __LedDriver5716_H


#include "main.h"



// Pins declarations ---------------------------------------------------------



// Constant declarations -----------------------------------------------------

//#define GS_DATA_SIZE 72

#define TI_L4C_LED_DRIVER_DATA_SIZE 	9u

extern SPI_HandleTypeDef hspi1;
extern uint8_t tlc6c5816_data[TI_L4C_LED_DRIVER_DATA_SIZE];




/// @addtogroup LED_DRIVER_LIBRARY
/// @{
 
/// @defgroup LedDriverLib Global functions used in LCD files
//// 
///  
 
/// @addtogroup LedDriverLib
/// @{

void vLedDataSendToSpi();
void vLedDriverInit();
void vLedDriverSendEndCallback(void);
void vSetLedDriverBlankHigh(void);
void vSetLedDriverBlankLow(void);

/// @} // endgroup LedDriverLib

/// @} // endgroup LED_DRIVER_LIBRARY


#endif
