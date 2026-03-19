#include "Odo.h"
#include "extEEPROM.h"
#include "stm32u5xx_it.h"
#include "Screen.h"
#include "HourMeter.h"
#include "TML_14229_UDSH.h"






#define READ_BYTE_SIZE 				16U


#define NO_OF_BLOCK_ODO				6u
#define ODO_LAST_PARAMETER_BLOCK  	255U
#define EOL_PARAMETER_BLOCK  		245U
#define HOUR_PARAMETER_BLOCK  		247U


typedef struct
{
	uint8_t     NutralGearInputType:1;   //Time stamp for sense Ignition
	uint8_t 	ParingBrakeInputType:1;
	uint8_t     EngineSelected:3;   //Time stamp for sense Ignition
	uint8_t 	BaudRate:2;

} VeHICLE_OPTION_TYPE;

VeHICLE_OPTION_TYPE sVehicleOptionContent;

uint32_t AddressToWriteNewData = 0u;
uint8_t EraseFlashStatus = ePass;
static FLASH_EraseInitTypeDef EraseInitStruct;
uint32_t FirstPage1 = 0u, NbOfPages1 = 0u, BankNumber1 = 0u;
uint8_t FlashPageError = 0u;
unsigned int CheckSum = 0;
static uint32_t LastHourDataTripA = 0u;
static uint32_t LastHourDataTripB = 0u;


static uint16_t TOTAL_ODO_ADD  = 0u;
static uint16_t FrameAddress = 0u;
uint8_t BootIdentifire = 0u;

static uint32_t EOL_SpeedoPulsesVal = 0;
static uint16_t EOL_VehicleOptionContentVal = 0;

static uint32_t GetPage(uint32_t Addr);
static uint32_t GetBank(uint32_t Addr);
static void ClearOdoTripA_B_AndSave(void);
static void ClearHourTripA_B_AndSave(void);
static void TftBrightNessDataSave(void);

// Static variable to store the previous brightness value
static uint8_t PrevTempBrightNessValue = 10;  // Initial value to ensure first save happens
// Static variables to hold previous values for comparison
static uint32_t PrevTempOdoData = 0u;
static uint16_t PrevTempTripAData = 0u;
static uint16_t PrevTempTripBData = 0u;

// Static variables to hold the previous values for comparison
static uint8_t PrevFracTripB = 0u;
static uint8_t PrevFracTripA = 0u;
static uint8_t PrevFracOdo = 0u;



void SaveOdoDataInFlash(void)
{
    uint32_t TempOdoData = 0u;
    uint16_t TempTripAData = 0u;
    uint16_t TempTripBData = 0u;


    // Check if any of the flags are TRUE (indicating data updates)
    if((bGetOdoUpdateFlagStatus() == TRUE) || (bGetOdoTripAUpdateStatus() == TRUE) || (bGetOdoTripBUpdateStatus() == TRUE))
    {
        // Get the current ODO and trip data
        TempOdoData = u32GetOdoValue();
        TempTripAData = (uint16_t)u32GetOdoTripAValue();
        TempTripBData = (uint16_t)u32GetOdoTripBValue();

        // Handle maximum ODO value reset (after reaching max, trip will work)
        if (TempOdoData >= 999999u)
        {
            TempOdoData = TempOdoData - 999999u;  // Reset after max value
        }

        // Calculate resolution and address change every 50,000 value
        TempOdoData = TempOdoData / 50000u;  // Every 50000 value address changes
        TOTAL_ODO_ADD = ((TempOdoData * 16u) + 1u);  // Address computation

        // Compare current data with previous data, only write if changed
        if (TempOdoData != PrevTempOdoData || TempTripAData != PrevTempTripAData || TempTripBData != PrevTempTripBData)
        {
            // If the address has changed, write the new address to EEPROM
            if (TOTAL_ODO_ADD != FrameAddress)
            {
                WriteByteEEPROM(ODO_FRAME_ADDRESS + 0, TOTAL_ODO_ADD >> 8u);  // High byte
                WriteByteEEPROM(ODO_FRAME_ADDRESS + 1, TOTAL_ODO_ADD);         // Low byte
                FrameAddress = TOTAL_ODO_ADD;  // Update the frame address
            }

            TempOdoData = u32GetOdoValue();
			// Update previous values to the current ones
			PrevTempOdoData = TempOdoData;
			PrevTempTripAData = TempTripAData;
			PrevTempTripBData = TempTripBData;
            // Refresh the watchdog timer to prevent reset
            HAL_IWDG_Refresh(&hiwdg);

            // Write memory corruption check data (0xAA)
            WriteByteEEPROM(TOTAL_ODO_ADD + 0, 0xAA);
            WriteByteEEPROM(TOTAL_ODO_ADD + 8, 0xAA);

            // Write the actual ODO and trip data to EEPROM
            WriteByteEEPROM(TOTAL_ODO_ADD + 1, TempOdoData >> 16);
            WriteByteEEPROM(TOTAL_ODO_ADD + 2, TempOdoData >> 8);
            WriteByteEEPROM(TOTAL_ODO_ADD + 3, TempOdoData);
            WriteByteEEPROM(TOTAL_ODO_ADD + 4, TempTripAData >> 8);
            WriteByteEEPROM(TOTAL_ODO_ADD + 5, TempTripAData);
            WriteByteEEPROM(TOTAL_ODO_ADD + 6, TempTripBData >> 8);
            WriteByteEEPROM(TOTAL_ODO_ADD + 7, TempTripBData);

            // Write memory corruption check data (0x55)
            WriteByteEEPROM(TOTAL_ODO_ADD + 0, 0x55);
            WriteByteEEPROM(TOTAL_ODO_ADD + 8, 0x55);

            // Refresh the watchdog timer again after writing
            HAL_IWDG_Refresh(&hiwdg);

            // Write a second set of ODO and trip data to EEPROM (backup)
            WriteByteEEPROM(TOTAL_ODO_ADD + 9, TempOdoData >> 16);
            WriteByteEEPROM(TOTAL_ODO_ADD + 10, TempOdoData >> 8);
            WriteByteEEPROM(TOTAL_ODO_ADD + 11, TempOdoData);
            WriteByteEEPROM(TOTAL_ODO_ADD + 12, TempTripAData >> 8);
            WriteByteEEPROM(TOTAL_ODO_ADD + 13, TempTripAData);
            WriteByteEEPROM(TOTAL_ODO_ADD + 14, TempTripBData >> 8);
            WriteByteEEPROM(TOTAL_ODO_ADD + 15, TempTripBData);

            // Save fraction digit information (when ODO value changes)
            SaveFractionDigit();  // Function to save fractional data

            // Clear the update flags after saving
            vSetOdoUpdateFlagStatus(FALSE);
            vSetOdoTripAUpdateStatus(FALSE);
            vSetOdoTripBUpdateStatus(FALSE);
        }
    }

	ClearOdoTripA_B_AndSave(); 		// clear 0 value saving while odo trip reset

}

/*************************************************************************************************************//**
 *
 * Purpose  :   TripHour And Tft Bright NessSave,This function save TripHour And Tft Bright NessSave
 *
 * @param[in]:  None
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/
void TripHourAndTftBrightNessSave(void)
{
	ClearHourTripA_B_AndSave();		// Hour saving while hour trip reset
	TftBrightNessDataSave();
}


/*************************************************************************************************************//**
 *
 * Purpose  :   SaveFractionDigit,This function save fraction digit of Hour ,trip and service data and second data
 *
 * @param[in]:  None
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/
void SaveFractionDigit()
{
    uint8_t temp;
    uint8_t FracTripB = 0u;
    uint8_t FracTripA = 0u;
    uint8_t FracOdo = 0u;


    // Get the current fraction values
    FracOdo = u8GetOdoFractionValue();
    FracTripA = u8GetOdoTripAFractionValue();
    FracTripB = u8GetOdoTripBFractionValue();

    // Check if any value has changed
    if (FracOdo != PrevFracOdo || FracTripA != PrevFracTripA || FracTripB != PrevFracTripB)
    {
        // Write 0xaa data for memory corruption checking purpose
        WriteByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 0u, 0xAA);
        WriteByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 3u, 0xAA);

        // Write 0x55 data for memory corruption checking purpose
        temp = ((FracTripA * 10) + FracTripB);
        WriteByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 1u, temp);
        WriteByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 2u, FracOdo);
        WriteByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 0u, 0x55);
        WriteByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 3u, 0x55);

        // Second backup copy of fraction digits
        temp = ((FracTripA * 10) + FracTripB);
        WriteByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 4u, temp);
        WriteByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 5u, FracOdo);

        // Update the previous values
        PrevFracTripB = FracTripB;
        PrevFracTripA = FracTripA;
        PrevFracOdo = FracOdo;
    }
}


static void TftBrightNessDataSave(void)
{
    uint8_t TempBrightNessValue = 0;

    if (GetBrightNessScreenExitStatus() == TRUE)
    {
        TempBrightNessValue = GetBrightnessBarStatus();

        // Only write to EEPROM if the current brightness value is different from the previous value
        if (TempBrightNessValue != PrevTempBrightNessValue)
        {
            // Write 0xaa data for memory data corruption checking purpose
            WriteByteEEPROM(TFT_BRIGHTNESS_ADD + 0u, 0xAA);
            WriteByteEEPROM(TFT_BRIGHTNESS_ADD + 2u, 0xAA);

            // Write the current brightness value to EEPROM
            WriteByteEEPROM(TFT_BRIGHTNESS_ADD + 1u, TempBrightNessValue);

            // Write 0x55 data for memory data corruption checking purpose
            WriteByteEEPROM(TFT_BRIGHTNESS_ADD + 0u, 0x55);
            WriteByteEEPROM(TFT_BRIGHTNESS_ADD + 2u, 0x55);

            // Write the current brightness value again for redundancy or backup
            WriteByteEEPROM(TFT_BRIGHTNESS_ADD + 3u, TempBrightNessValue);

            // Update the previous brightness value
            PrevTempBrightNessValue = TempBrightNessValue;
        }

        // Reset the status after saving
        SetBrightNessScreenExitStatus(FALSE);
    }
}


static void ClearOdoTripA_B_AndSave(void)
{
	static uint8_t OneTimeFlagTripA = TRUE;
	static uint8_t OneTimeFlagTripB = TRUE;


	if((GetTripAResetStatus() == TRUE) && (OneTimeFlagTripA == TRUE)
			&& GetvShowHourOdoSelectOnTFT() == eOdoMeterSelect && GetScreenStatus() == eHomeScreen)
	{
		HAL_IWDG_Refresh(&hiwdg);
		vSetOdoTripAValue(0u);
		/*--------------Fraction Trip A -----------------------*/
		vSetOdoTripAFractionValue(0u);
		SaveFractionDigit();//Fraction save the value
		vSetOdoTripAUpdateStatus(TRUE);
		vSetOdoTripAPulseCounter(0u);
		OneTimeFlagTripA = FALSE;
	}
	else
	{
		OneTimeFlagTripA = TRUE;
	}

	if((GetTripBResetStatus() == TRUE) && (OneTimeFlagTripB == TRUE)
			&& GetvShowHourOdoSelectOnTFT() == eOdoMeterSelect && GetScreenStatus() == eHomeScreen)
	{
		HAL_IWDG_Refresh(&hiwdg);
		vSetOdoTripBValue(0);
		/*--------------Fraction Trip B -----------------------*/
		vSetOdoTripBFractionValue(0);
		SaveFractionDigit();//Fraction save the value
		vSetOdoTripBUpdateStatus(TRUE);
		vSetOdoTripBPulseCounter(0u);
		OneTimeFlagTripB = FALSE;
	}
	else
	{
		OneTimeFlagTripB = TRUE;
	}

}

static void ClearHourTripA_B_AndSave(void)
{
	static uint8_t OneTimeFlagTripA = TRUE;
	static uint8_t OneTimeFlagTripB = TRUE;
	static uint32_t LastHourTempA = 0;
	static uint32_t LastHourTempB = 0;
	uint32_t HourData = 0;


	if(((GetTripAResetStatus() == TRUE) && (OneTimeFlagTripA == TRUE)
			&& GetvShowHourOdoSelectOnTFT() == eHourMeterSelect && GetScreenStatus() == eHomeScreen) || GetHourTripAOverFlowFlag() == TRUE)
	{
		HourData = GetHourDataValue();							/* Hour data save -- */
		LastHourTempA = HourData;

		HAL_IWDG_Refresh(&hiwdg);
		/* write 0x55 data for memory data curruption checking purpose */
		WriteByteEEPROM(TOTAL_HOUR_A_ADD + 0u,0xAA);
		WriteByteEEPROM(TOTAL_HOUR_A_ADD + 4u,0xAA);

		WriteByteEEPROM(TOTAL_HOUR_A_ADD + 1u,HourData>>16);
		WriteByteEEPROM(TOTAL_HOUR_A_ADD + 2u,HourData>>8);
		WriteByteEEPROM(TOTAL_HOUR_A_ADD + 3u,HourData);
		/* write 0x55 data for memory data curruption checking purpose */
		WriteByteEEPROM(TOTAL_HOUR_A_ADD + 0u,0x55);
		WriteByteEEPROM(TOTAL_HOUR_A_ADD + 4u,0x55);
		HAL_IWDG_Refresh(&hiwdg);

		WriteByteEEPROM(TOTAL_HOUR_A_ADD + 5u,HourData>>16);
		WriteByteEEPROM(TOTAL_HOUR_A_ADD + 6u,HourData>>8);
		WriteByteEEPROM(TOTAL_HOUR_A_ADD + 7u,HourData);

		HAL_IWDG_Refresh(&hiwdg);
		LastHourDataTripA = HourData;
		SetTripAHourValue(0u);
		SetHourTripAOverFlowFlag(FALSE);
		OneTimeFlagTripA = FALSE;
	}
	else
	{
		OneTimeFlagTripA = TRUE;
	}

	if(((GetTripBResetStatus() == TRUE) && (OneTimeFlagTripB == TRUE)
			&& GetvShowHourOdoSelectOnTFT() == eHourMeterSelect && GetScreenStatus() == eHomeScreen) || GetHourTripBOverFlowFlag() == TRUE)
	{
		HourData = GetHourDataValue();							/* Hour data save -- */
		LastHourTempB = HourData;
		HAL_IWDG_Refresh(&hiwdg);
		/* write 0x55 data for memory data curruption checking purpose */
		WriteByteEEPROM(TOTAL_HOUR_B_ADD + 0u,0xAA);
		WriteByteEEPROM(TOTAL_HOUR_B_ADD + 4u,0xAA);

		WriteByteEEPROM(TOTAL_HOUR_B_ADD + 1u,HourData>>16);
		WriteByteEEPROM(TOTAL_HOUR_B_ADD + 2u,HourData>>8);
		WriteByteEEPROM(TOTAL_HOUR_B_ADD + 3u,HourData);
		/* write 0x55 data for memory data curruption checking purpose */
		WriteByteEEPROM(TOTAL_HOUR_B_ADD + 0u,0x55);
		WriteByteEEPROM(TOTAL_HOUR_B_ADD + 4u,0x55);
		HAL_IWDG_Refresh(&hiwdg);

		WriteByteEEPROM(TOTAL_HOUR_B_ADD + 5u,HourData>>16);
		WriteByteEEPROM(TOTAL_HOUR_B_ADD + 6u,HourData>>8);
		WriteByteEEPROM(TOTAL_HOUR_B_ADD + 7u,HourData);

		HAL_IWDG_Refresh(&hiwdg);
		LastHourDataTripB = HourData;
		SetTripBHourValue(0u);
		SetHourTripBOverFlowFlag(FALSE);
		OneTimeFlagTripB = FALSE;
	}
	else
	{
		OneTimeFlagTripB = TRUE;
	}
}


uint32_t GetLastSavedHourValueForTripA(void)
{
	return LastHourDataTripA;
}

uint32_t GetLastSavedHourValueForTripB(void)
{
	return LastHourDataTripB;
}

void SetLastSavedHourValueForTripA(uint32_t Value)
{
	LastHourDataTripA = Value;
}

void SetLastSavedHourValueForTripB(uint32_t Value)
{
	LastHourDataTripB = Value;
}



void ReadBootIdentifireParameter(void)
{
	uint8_t temp;
	uint8_t temp1;
	uint8_t BootValue = 0;

	temp = ReadByteEEPROM(BOOTLOADER_IDEN_ADD);
	temp1 = ReadByteEEPROM(BOOTLOADER_IDEN_ADD + 2u);
	if(temp == 0x55 && temp1 == 0x55)
	{
		BootValue = ReadByteEEPROM(BOOTLOADER_IDEN_ADD + 1u);
	}
	else
	{
		if(temp == 0xFF && temp1 == 0xFF)
		{
			BootValue = eApplicationMode;
		}
		else
		{
			BootValue = ReadByteEEPROM(BOOTLOADER_IDEN_ADD + 3u);
		}
	}

	vSaveBootloaderState(BootValue);

}

void SaveBootIdentifireParameter(void)
{
	uint8_t TempData = 0u;

	TempData = u8GetBootloaderState();
	/* write 0xaa data for memory data curruption checking purpose*/
	WriteByteEEPROM(BOOTLOADER_IDEN_ADD + 0u,0xAA);
	WriteByteEEPROM(BOOTLOADER_IDEN_ADD + 2u,0xAA);

	WriteByteEEPROM(BOOTLOADER_IDEN_ADD + 1u,TempData);
	/* write 0x55 data for memory data curruption checking purpose */
	WriteByteEEPROM(BOOTLOADER_IDEN_ADD + 0,0x55);
	WriteByteEEPROM(BOOTLOADER_IDEN_ADD + 2u,0x55);

	WriteByteEEPROM(BOOTLOADER_IDEN_ADD + 3u,TempData);

}

uint8_t u8GetBootloaderState()
{
	return BootIdentifire;
}

void vSaveBootloaderState(uint8_t data)
{
	BootIdentifire = data;
}


/*-----------------------------------------------------------------------*/
void SetEolSpeedoPulsesInFlash(uint32_t Val)
{
	EOL_SpeedoPulsesVal = Val;
}

void SetEolVeicleOptionContentValueInFlash(uint16_t Val)
{
	EOL_VehicleOptionContentVal = Val;
}

uint32_t GetEolSpeedoPulsesInFlash()
{
	return EOL_SpeedoPulsesVal;
}

uint16_t GetEolVeicleOptionContentValueInFlash()
{
	return EOL_VehicleOptionContentVal;
}





/* Vehicle Option content general function for value set or get ---------------------*/
void SetNutralGearInputType(uint8_t Value)
{
	sVehicleOptionContent.NutralGearInputType = Value;
}

uint8_t GetNutralGearInputType(void)
{
	return sVehicleOptionContent.NutralGearInputType;
}

void SetParkingBrakeInputType(uint8_t Value)
{
	sVehicleOptionContent.ParingBrakeInputType = Value;
}

uint8_t GetParkingBrakeInputType(void)
{
	return sVehicleOptionContent.ParingBrakeInputType;
}

void SetBaudRateType(uint8_t Value)
{
	sVehicleOptionContent.BaudRate = Value;
}

uint8_t GetBaudRateType(void)
{
	return sVehicleOptionContent.BaudRate;
}

void SetEngineSelectionType(uint8_t Value)
{
	sVehicleOptionContent.EngineSelected = Value;
}

uint8_t GetEngineSelectionType(void)
{
	return sVehicleOptionContent.EngineSelected;
}

void GetEOl_ParemetersRead(void)
{
	uint32_t TempSpeedoPulses = 0u;
	uint16_t TempVehicleOptionConVal = 0u;
   //Declare the variables
	uint8_t TempNutral, TempPrakBrake, TempBaudRate, TempEngineSlect;
	uint8_t tempCheckVOC = 0u;
	uint8_t tempCheckVOC1 = 0u;
	uint8_t tempCheckSPD = 0u;
	uint8_t tempCheckSPD1 = 0u;


	/*--------------------------Vehicle option content ----------------------------*/
	tempCheckVOC = ReadByteEEPROM(EOL_VEH_OP_ADD);
	tempCheckVOC1 = ReadByteEEPROM(EOL_VEH_OP_ADD + 3u);
	if(tempCheckVOC == 0x55 && tempCheckVOC1 == 0x55)
	{
		TempVehicleOptionConVal = read_int(EOL_VEH_OP_ADD + 1u);
	}
	else
	{
		if(tempCheckVOC == 0xFF && tempCheckVOC1 == 0xFF)
		{
			TempVehicleOptionConVal = 0x8AFF; // set deafault value
		}
		else
		{
			TempVehicleOptionConVal = read_int(EOL_VEH_OP_ADD + 4u);
		}
	}

	/*--------------------------Speedo Pulses ----------------------------*/
	tempCheckSPD = ReadByteEEPROM(EOL_SPD_PULSE_ADD);
	tempCheckSPD1 = ReadByteEEPROM(EOL_SPD_PULSE_ADD + 4u);
	if(tempCheckSPD == 0x55 && tempCheckSPD1 == 0x55)
	{
		TempSpeedoPulses = read_long(EOL_SPD_PULSE_ADD + 1u);
	}
	else
	{
		if(tempCheckSPD == 0xFF && tempCheckSPD1 == 0xFF)
		{
			TempSpeedoPulses = 181980u; // set default pulses
		}
		else
		{
			TempSpeedoPulses = read_long(EOL_SPD_PULSE_ADD + 5u);
		}
	}

	vSetSpeedoPulses(TempSpeedoPulses);		// Default pulses 181980
	SetEolSpeedoPulsesInFlash(TempSpeedoPulses);
	SetEolVeicleOptionContentValueInFlash(TempVehicleOptionConVal);
	// Clear the values and then set the required bits
	//TempNutral = (TempVehicleOptionConVal >> 8) & 1u; 			// Extract the 8th bit for TempNutral
	//TempPrakBrake = (TempVehicleOptionConVal >> 9) & 1u; 		// Extract the 9st bit for TempPrakBrake
	TempEngineSlect = (TempVehicleOptionConVal >> 10) & 0x7; 	// Extract the 10nd, 11rd, and 12th bits for TempEngineSlect (0x07 = 00000111)
	//TempBaudRate = (TempVehicleOptionConVal >> 13) & 0x3; 		// Extract the 13th and 14th bits for TempBaudRate (0x03 = 00000011)

	SetNutralGearInputType(eActiveHigh);
	SetParkingBrakeInputType(eActiveLow);
	SetBaudRateType(e250);
	if(TempEngineSlect >= eBachoeLoaderTotalEngine)
	{
		TempEngineSlect = eBackhoeLoaderEngineM_M74Hp;
	}
	SetEngineSelectionType(TempEngineSlect);
}
/*************************************************************************************************************//**
 *
 * Purpose  :   memory_init,This function is use to initializing the Total Hour & TRIP,Service hour variable
 *            	eeprom from last stored data
 *
 * @param[in]:  None
 * @param[out]:	None
 * @return     	None
 ****************************************************************************************************************/
void ReadOdoDataAndFractionValue(void)
{
	uint8_t temp;
	uint8_t temp1;
	uint32_t OdoData = 0;
	uint16_t TripAData = 0;
	uint16_t TripBData = 0;
	uint8_t TripBFrac = 0;
	uint8_t TripAFrac = 0;
	uint8_t OdoFrac = 0;


	temp = ReadByteEEPROM(ODO_FRAC_TRIAB_F_ADD);
	temp1 = ReadByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 3u);
	if(temp == 0x55 && temp1 == 0x55)
	{
		temp = ReadByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 1u);
		TripAFrac =  temp / 10u;
		TripBFrac =  temp % 10u;
		OdoFrac = ReadByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 2u);
	}
	else
	{
		if(temp == 0xff && temp1 == 0xff) // when freash eeprom
		{
			TripAFrac = 0u;
			TripBFrac = 0u;
			OdoFrac = 0u;
		}
		else
		{
			temp = ReadByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 4u);
			TripAFrac =  temp / 10u;
			TripBFrac =  temp % 10u;
			OdoFrac = ReadByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 5u);
		}
	}

	vSetOdoFractionValue(OdoFrac);
	vSetOdoTripBFractionValue(TripBFrac);
	vSetOdoTripAFractionValue(TripAFrac);

	// Static variables to hold the previous values for comparison
	PrevFracTripB = TripBFrac;
	PrevFracTripA = TripAFrac;
	PrevFracOdo = OdoFrac;

	FrameAddress = read_int(ODO_FRAME_ADDRESS);
	if(FrameAddress == 0xFFFF)
	{
		FrameAddress = 1;
	}
	TOTAL_ODO_ADD = FrameAddress;
	temp = ReadByteEEPROM(TOTAL_ODO_ADD);
	temp1 = ReadByteEEPROM(TOTAL_ODO_ADD + 8u);
	if(temp == 0x55 && temp1 == 0x55)
	{
		OdoData = read_long(TOTAL_ODO_ADD + 1u);
		TripAData  = read_int(TOTAL_ODO_ADD + 4u);
		TripBData = read_int(TOTAL_ODO_ADD + 6u);
	}
	else
	{
		if(temp == 0xff && temp1 == 0xff) // when new eeprom
		{
			OdoData = 0u;
			TripAData  = 0u;
			TripBData = 0u;
		}
		else
		{
			OdoData = read_long(TOTAL_ODO_ADD + 9u);
			TripAData  = read_int(TOTAL_ODO_ADD + 12u);
			TripBData = read_int(TOTAL_ODO_ADD + 14u);
		}
	}
	// Static variables to hold previous values for comparison
	PrevTempOdoData = OdoData;
	PrevTempTripAData = TripAData;
	PrevTempTripBData = TripBData;

	vSetOdoValue(OdoData);
	vSetOdoTripAValue(TripAData);
	vSetOdoTripBValue(TripBData);
	HAL_IWDG_Refresh(&hiwdg);

}

void ReadTftBrightnessValue(void)
{
	uint8_t temp;
	uint8_t temp1;
	uint8_t TempBrightNessValue = 0;

	temp = ReadByteEEPROM(TFT_BRIGHTNESS_ADD);
	temp1 = ReadByteEEPROM(TFT_BRIGHTNESS_ADD + 2u);
	if(temp == 0x55 && temp1 == 0x55)
	{
		TempBrightNessValue = ReadByteEEPROM(TFT_BRIGHTNESS_ADD + 1u);
	}
	else
	{
		if(temp == 0xFF && temp1 == 0xFF)
		{
			TempBrightNessValue = 10u;
		}
		else
		{
			TempBrightNessValue = ReadByteEEPROM(TFT_BRIGHTNESS_ADD + 3u);
		}
	}

	PrevTempBrightNessValue = TempBrightNessValue;
	SetBrightnessBarStatus(TempBrightNessValue);
}

void ReadHourDataSystemParameter(void)
{
	uint8_t tempA = 0;
	uint8_t tempA1 = 0;
	uint8_t tempB = 0;
	uint8_t tempB1 = 0;

	/*--------------------------TRIP A HOUR READ ----------------------------*/
	tempA = ReadByteEEPROM(TOTAL_HOUR_A_ADD);
	tempA1 = ReadByteEEPROM(TOTAL_HOUR_A_ADD + 4u);
	if(tempA == 0x55 && tempA1 == 0x55)
	{
		LastHourDataTripA = read_long(TOTAL_HOUR_A_ADD + 1u);
	}
	else
	{
		if(tempA == 0xFF && tempA1 == 0xFF)
		{
			LastHourDataTripA = 0u;
		}
		else
		{
			LastHourDataTripA = read_long(TOTAL_HOUR_A_ADD + 5u);
		}
	}

	/*--------------------------TRIP B HOUR READ ----------------------------*/
	tempB = ReadByteEEPROM(TOTAL_HOUR_B_ADD);
	tempB1 = ReadByteEEPROM(TOTAL_HOUR_B_ADD + 4u);
	if(tempB == 0x55 && tempB1 == 0x55)
	{
		LastHourDataTripB = read_long(TOTAL_HOUR_B_ADD + 1u);
	}
	else
	{
		if(tempB == 0xFF && tempB1 == 0xFF)
		{
			LastHourDataTripB = 0u;
		}
		else
		{
			LastHourDataTripB = read_long(TOTAL_HOUR_B_ADD + 5u);
		}

	}

}


void AllparameterWriteToZero(void)
{
	uint32_t u32Temp = 0u;
	uint16_t u16Temp = 0u;
	uint32_t u32TempOdo = 0u;
	uint32_t u32TempHourA = 0u;
	uint32_t u32TempHourB = 0u;
	uint16_t u16TempTripA = 0u;
	uint16_t u16TempripB = 0u;

	u32Temp = 0u;
	u16Temp = 0u;
	u32TempOdo = 0u;
	u32TempHourA = 0u;
	u32TempHourB = 0u;
	u16TempTripA = 0u;
	u16TempripB = 0u;

	/* write 0xaa data for memory data curruption checking purpose*/
	WriteByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 0u,0xAA);
	WriteByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 3u,0xAA);
	/* write 0x55 data for memory data curruption checking purpose */

	WriteByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 0u,0x55);
	WriteByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 1u,0);
	WriteByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 2u,0);
	WriteByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 3u,0x55);

	// Second backup copy of fraction digit */
	WriteByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 4u,0);
	WriteByteEEPROM(ODO_FRAC_TRIAB_F_ADD + 5u,0);
	FrameAddress = read_int(ODO_FRAME_ADDRESS);
	if(FrameAddress == 0xFFFF)
	{
		FrameAddress = 1u;
	}
	TOTAL_ODO_ADD = FrameAddress;
	WriteByteEEPROM(TOTAL_ODO_ADD + 0,0xAA);
	WriteByteEEPROM(TOTAL_ODO_ADD + 8,0xAA);
	/* write 0x55 data for memory data curruption checking purpose */
	WriteByteEEPROM(TOTAL_ODO_ADD + 0,0x55);
	WriteByteEEPROM(TOTAL_ODO_ADD + 8,0x55);
	WriteByteEEPROM(TOTAL_ODO_ADD + 1,u32TempOdo>>16);
	WriteByteEEPROM(TOTAL_ODO_ADD + 2,u32TempOdo>>8);
	WriteByteEEPROM(TOTAL_ODO_ADD + 3,u32TempOdo);
	WriteByteEEPROM(TOTAL_ODO_ADD + 4,u16TempTripA>>8);
	WriteByteEEPROM(TOTAL_ODO_ADD + 5,u16TempTripA);
	WriteByteEEPROM(TOTAL_ODO_ADD + 6,u16TempripB>>8);
	WriteByteEEPROM(TOTAL_ODO_ADD + 7,u16TempripB);

	asm("NOP");asm("NOP");
	HAL_IWDG_Refresh(&hiwdg);
	WriteByteEEPROM(TOTAL_ODO_ADD + 9,u32TempOdo>>16);
	WriteByteEEPROM(TOTAL_ODO_ADD + 10,u32TempOdo>>8);
	WriteByteEEPROM(TOTAL_ODO_ADD + 11,u32TempOdo);
	WriteByteEEPROM(TOTAL_ODO_ADD + 12,u16TempTripA>>8);
	WriteByteEEPROM(TOTAL_ODO_ADD + 13,u16TempTripA);
	WriteByteEEPROM(TOTAL_ODO_ADD + 14,u16TempripB>>8);
	WriteByteEEPROM(TOTAL_ODO_ADD + 15,u16TempripB);
	asm("NOP");asm("NOP");

	/* write 0x55 data for memory data curruption checking purpose */
	WriteByteEEPROM(TOTAL_HOUR_A_ADD + 0u,0xAA);
	WriteByteEEPROM(TOTAL_HOUR_A_ADD + 4u,0xAA);
	/* write 0x55 data for memory data curruption checking purpose */
	WriteByteEEPROM(TOTAL_HOUR_A_ADD + 0u,0x55);
	WriteByteEEPROM(TOTAL_HOUR_A_ADD + 4u,0x55);
	WriteByteEEPROM(TOTAL_HOUR_A_ADD + 1u,u32TempHourA>>16);
	WriteByteEEPROM(TOTAL_HOUR_A_ADD + 2u,u32TempHourA>>8);
	WriteByteEEPROM(TOTAL_HOUR_A_ADD + 3u,u32TempHourA);

	HAL_IWDG_Refresh(&hiwdg);

	WriteByteEEPROM(TOTAL_HOUR_A_ADD + 5u,u32TempHourA>>16);
	WriteByteEEPROM(TOTAL_HOUR_A_ADD + 6u,u32TempHourA>>8);
	WriteByteEEPROM(TOTAL_HOUR_A_ADD + 7u,u32TempHourA);

	/* write 0x55 data for memory data curruption checking purpose */
	WriteByteEEPROM(TOTAL_HOUR_B_ADD + 0u,0xAA);
	WriteByteEEPROM(TOTAL_HOUR_B_ADD + 4u,0xAA);
	/* write 0x55 data for memory data curruption checking purpose */
	WriteByteEEPROM(TOTAL_HOUR_B_ADD + 0u,0x55);
	WriteByteEEPROM(TOTAL_HOUR_B_ADD + 4u,0x55);
	WriteByteEEPROM(TOTAL_HOUR_B_ADD + 1u,u32TempHourB>>16);
	WriteByteEEPROM(TOTAL_HOUR_B_ADD + 2u,u32TempHourB>>8);
	WriteByteEEPROM(TOTAL_HOUR_B_ADD + 3u,u32TempHourB);

	HAL_IWDG_Refresh(&hiwdg);

	WriteByteEEPROM(TOTAL_HOUR_B_ADD + 5u,u32TempHourB>>16);
	WriteByteEEPROM(TOTAL_HOUR_B_ADD + 6u,u32TempHourB>>8);
	WriteByteEEPROM(TOTAL_HOUR_B_ADD + 7u,u32TempHourB);

	/* write 0xaa data for memory data curruption checking purpose*/
	WriteByteEEPROM(EOL_VEH_OP_ADD + 0,0xAA);
	WriteByteEEPROM(EOL_VEH_OP_ADD + 3,0xAA);
	/* write 0x55 data for memory data curruption checking purpose */
	WriteByteEEPROM(EOL_VEH_OP_ADD + 0,0x55);
	WriteByteEEPROM(EOL_VEH_OP_ADD + 3,0x55);
	u16Temp = 0xCAFF;
	WriteByteEEPROM(EOL_VEH_OP_ADD + 1,u16Temp>>8);
	WriteByteEEPROM(EOL_VEH_OP_ADD + 2,u16Temp);

	HAL_IWDG_Refresh(&hiwdg);
	u16Temp = 0xCAFF;
	WriteByteEEPROM(EOL_VEH_OP_ADD + 4,u16Temp>>8);
	WriteByteEEPROM(EOL_VEH_OP_ADD + 5,u16Temp);

	/* write 0xaa data for memory data curruption checking purpose*/
	WriteByteEEPROM(EOL_SPD_PULSE_ADD + 0,0xAA);
	WriteByteEEPROM(EOL_SPD_PULSE_ADD + 4,0xAA);
	/* write 0x55 data for memory data curruption checking purpose */
	WriteByteEEPROM(EOL_SPD_PULSE_ADD + 0,0x55);
	WriteByteEEPROM(EOL_SPD_PULSE_ADD + 4,0x55);
	u32Temp = 0x02C6DC;
	WriteByteEEPROM(EOL_SPD_PULSE_ADD + 1,u32Temp>>16);
	WriteByteEEPROM(EOL_SPD_PULSE_ADD + 2,u32Temp>>8);
	WriteByteEEPROM(EOL_SPD_PULSE_ADD + 3,u32Temp);

	HAL_IWDG_Refresh(&hiwdg);
	u32Temp = 0x02C6DC;
	WriteByteEEPROM(EOL_SPD_PULSE_ADD + 5,u32Temp>>16);
	WriteByteEEPROM(EOL_SPD_PULSE_ADD + 6,u32Temp>>8);
	WriteByteEEPROM(EOL_SPD_PULSE_ADD + 7,u32Temp);

	/* write 0xaa data for memory data curruption checking purpose*/
	WriteByteEEPROM(TFT_BRIGHTNESS_ADD + 0u,0xAA);
	WriteByteEEPROM(TFT_BRIGHTNESS_ADD + 2u,0xAA);
	/* write 0x55 data for memory data curruption checking purpose */
	WriteByteEEPROM(TFT_BRIGHTNESS_ADD + 0,0x55);
	WriteByteEEPROM(TFT_BRIGHTNESS_ADD + 1u,10u);
	WriteByteEEPROM(TFT_BRIGHTNESS_ADD + 2u,0x55);

	WriteByteEEPROM(TFT_BRIGHTNESS_ADD + 3u,10u);
	u16Temp = 1u;
	WriteByteEEPROM(ODO_FRAME_ADDRESS,u16Temp>>8);
	WriteByteEEPROM(ODO_FRAME_ADDRESS + 1,u16Temp);

// to be add here frame addedrss
	asm("NOP");asm("NOP");

}





