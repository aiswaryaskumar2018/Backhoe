
#include "ledDriver5716.h"
#include "TellTales.h"

static uint8_t bSpiLatchSequence = TRUE;
static uint8_t bSpiDataSend = 0;
static uint8_t EmptyRecBuff[4] = {0x00,0x00,0x00,0x00};
void delayus();

//Led driver Initilaization Buffer

uint8_t tlc6c5816_data[TI_L4C_LED_DRIVER_DATA_SIZE] = {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u};


/*****************************************************************************
**  Function name:  vLedDriverInit
**
**  Description:    Initialize Led buffer data ,Led driver driven by SPI communication by ch. SCI10
**
**  Parameters:     none
**
**  Return value:   none
**
*****************************************************************************/
void vLedDriverInit()
{
	 //State 1
	HAL_GPIO_WritePin(GPIOC, LED_DRIVER_CLR_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_DRIVER_G1_GPIO_Port, LED_DRIVER_G1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, LED_DRIVER_G2_Pin, GPIO_PIN_SET);
	//State 2
	HAL_GPIO_WritePin(GPIOC, LED_DRIVER_CLR_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, LED_DRIVER_LATCH_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, (uint8_t*)&tlc6c5816_data,TI_L4C_LED_DRIVER_DATA_SIZE,5);
	vLedDriverSendEndCallback();
	// LATCH = 1;
	//State 3
	HAL_GPIO_WritePin(GPIOC, LED_DRIVER_CLR_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_DRIVER_G1_GPIO_Port, LED_DRIVER_G1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, LED_DRIVER_G2_Pin, GPIO_PIN_RESET);
		  
}


/*****************************************************************************
**  Function name:  vLedDataSendToSpi
**
**  Description:   Send tell-tales data store in u8GSdata buffer to SPI 
**
**  Parameters:     none
**
**  Return value:   none
**
*****************************************************************************/

void vLedDataSendToSpi()
{
	if(bSpiDataSend == FALSE)
	{
		return;
	}
	HAL_GPIO_WritePin(GPIOC, LED_DRIVER_LATCH_Pin, GPIO_PIN_RESET);
	asm("NOP");asm("NOP");
	bSpiDataSend = FALSE;
	HAL_SPI_Transmit(&hspi1, (uint8_t*)&tlc6c5816_data,TI_L4C_LED_DRIVER_DATA_SIZE,5);
	vLedDriverSendEndCallback();
}

/***********************************************************************************************************************
* Function Name: vLedDriverSendEndCallback
* Description  : This function is a callback function of LedDriverInterrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
 void vLedDriverSendEndCallback(void)
{
    /* Start user code. Do not edit comment generated here */
	bSpiDataSend = TRUE;
	
	if(bSpiLatchSequence == TRUE)	
	{
		HAL_GPIO_WritePin(GPIOC, LED_DRIVER_LATCH_Pin, GPIO_PIN_RESET);
		delayus();		//Delay required b/w in latch transaction otherwise led drive malfunction 
		HAL_GPIO_WritePin(GPIOC, LED_DRIVER_LATCH_Pin, GPIO_PIN_SET);
		delayus();
		bSpiLatchSequence = FALSE;
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC, LED_DRIVER_LATCH_Pin, GPIO_PIN_SET);
		delayus();
		HAL_GPIO_WritePin(GPIOC, LED_DRIVER_LATCH_Pin, GPIO_PIN_RESET);
		delayus();
	}

	/* End user code. Do not edit comment generated here */
}

void delayus()
{
	asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
	asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
	asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
	asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
	asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
	asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
	asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
	asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
}
/*****************************************************************************
**  Function name:  vSetLedDriverBlankHigh
**
**  Description:   Set Led driver BLANK pin to HIGH
**
**  Parameters:     None
**
**  Return value:   none
**
*****************************************************************************/

void vSetLedDriverBlankHigh()
{
	uint8_t flag  = FALSE;
	uint16_t LedDriverDataSentTime = 0;

	//Disable Indicator bilink 1 time so we wait here until data not reached 
	//to led driver then set BLANK pin high
	LedDriverDataSentTime = u32GetClockTicks();
	while(flag == FALSE )
	{
		if(u32GetClockTicks() - LedDriverDataSentTime > 1000)
		{
			flag = TRUE;
		}
		else
		{
			flag = bSpiDataSend;
		}
	}

	HAL_GPIO_WritePin(GPIOC, LED_DRIVER_CLR_Pin, GPIO_PIN_SET);
}


/*****************************************************************************
**  Function name:  vSetLedDriverBlankLow
**
**  Description:   Set Led driver BLANK pin to HIGH
**
**  Parameters:     None
**
**  Return value:   none
**
*****************************************************************************/

void vSetLedDriverBlankLow()
{
	HAL_GPIO_WritePin(GPIOC, LED_DRIVER_CLR_Pin, GPIO_PIN_RESET);
}



