

#include "MuxDemux.h"

/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

#define INPUT_DEBOUNCE_TIME			100u		//Time in ms



/****************************************************************************************************************/
/* Type definitions                                                                                             */
/****************************************************************************************************************/
typedef struct
{
	uint32_t u32TimeStamp;
	uint32_t u32DebounceTime;
	uint8_t u8PrevStatus;
	uint8_t u8StatusFlag;
	uint8_t u8PinStatus;
	uint8_t	bStatusChange;
}lampInputType;

lampInputType sLampInputsProp[eMuxInputsMaximum];


typedef struct
{
	uint8_t MuxNuetralInput:1;
	uint8_t MuxPrakeBrakeInput:1;
	uint8_t MuxLowHighInput2:1;
	uint8_t MuxLowHighInput6:1;
	uint8_t	MuxLowHighInput4:1;
	uint8_t	MuxLowHighInput7:1;
	uint8_t	MuxLowHighInput5:1;
}InputDrvienType;

InputDrvienType sLampInputDrvienType = {ACTIVE_HIGH,ACTIVE_LOW,ACTIVE_LOW,ACTIVE_HIGH,ACTIVE_LOW,ACTIVE_HIGH,ACTIVE_LOW};

/****************************************************************************************************************/
/* Local variables                                                                                              */
uint8_t ConfigurableDmuxPinVal[eDmuxWriteMaximum];
uint8_t NonConfigurableDmuxPinVal[eDmuxWriteMaximum];



static void ConfgurableLampInputInit(void);

/*************************************************************************************************************//**
 *
 * Purpose  :  Dmux, Check Mux and Demux input and read and write
 *
 * @param[in]      :None
 * @param[out]:	None
 * @return    : None
 ****************************************************************************************************************/
void MuxDmuxProcessing(void)
{
	static uint32_t	TimeStamp  = 0u;
	static uint8_t Status = 0u;
	static uint8_t FirstEntry = 1u;


	if(FirstEntry)
	{
		TimeStamp = u32GetClockTicks();
		FirstEntry = 0;
		ConfgurableLampInputInit();
	}
	ConfgurableLampInputInit();

	switch(Status)
	{
		case eMuxScan_Neutral_Input :
			/* Read Left and Right Turn Input again --------------------------*/
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC,MUX_Z3_Write_Pin, sLampInputDrvienType.MuxNuetralInput);
			ConfigurableDmuxPinVal[eMux_Neutral_Input] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			break;
		case eMuxScan_ParkeBrake_Input:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC,MUX_Z3_Write_Pin, sLampInputDrvienType.MuxPrakeBrakeInput);
			ConfigurableDmuxPinVal[eMux_ParkeBrake_Input] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			break;
	    case eMuxScan_RightTurn_Input:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			ConfigurableDmuxPinVal[eMux_RightTurn_Input] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			break;
	    case eMuxScan_LeftTurn_Input:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			NonConfigurableDmuxPinVal[eNonConf_Mux_LeftTurn_Input] = HAL_GPIO_ReadPin(GPIOC, Z4_MUX_INPUT_Pin);
			break;
		case eMuxScan_Low_High_Input2:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC,MUX_Z3_Write_Pin, sLampInputDrvienType.MuxLowHighInput2);
			 ConfigurableDmuxPinVal[eMux_Low_High_Input2] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			break;
	    case eMuxScan_Low_High_Input6:
	    	/* Read Left and Right Turn Input again --------------------------*/
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC,MUX_Z3_Write_Pin, sLampInputDrvienType.MuxLowHighInput6);
			ConfigurableDmuxPinVal[eMux_Low_High_Input6] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			break;
	    case eMuxScan_RightTurn_Input1:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			ConfigurableDmuxPinVal[eMux_RightTurn_Input] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			break;
	    case eMuxScan_LeftTurn_Input1:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			NonConfigurableDmuxPinVal[eNonConf_Mux_LeftTurn_Input] = HAL_GPIO_ReadPin(GPIOC, Z4_MUX_INPUT_Pin);
			break;
	    case eMuxScan_Low_High_Input4:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC,MUX_Z3_Write_Pin, sLampInputDrvienType.MuxLowHighInput4);
			ConfigurableDmuxPinVal[eMux_Low_High_Input4] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			break;
	    case eMuxScan_Low_High_Input7:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC,MUX_Z3_Write_Pin, sLampInputDrvienType.MuxLowHighInput7);
			ConfigurableDmuxPinVal[eMux_Low_High_Input7] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			break;
	    case eMuxScan_RightTurn_Input2:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			ConfigurableDmuxPinVal[eMux_RightTurn_Input] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			break;
	    case eMuxScan_LeftTurn_Input2:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			NonConfigurableDmuxPinVal[eNonConf_Mux_LeftTurn_Input] = HAL_GPIO_ReadPin(GPIOC, Z4_MUX_INPUT_Pin);
			break;
		case eMuxScan_Low_High_Input5:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC,MUX_Z3_Write_Pin, sLampInputDrvienType.MuxLowHighInput5);
			ConfigurableDmuxPinVal[eMux_Low_High_Input5] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			break;
	    case eMuxScan_GlowPlug_BatteryCharging_Input:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_RESET);
			ConfigurableDmuxPinVal[eMux_GlowPlug_Input] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			NonConfigurableDmuxPinVal[eNonConf_Mux_BatteryCharging_Input] = HAL_GPIO_ReadPin(GPIOC, Z4_MUX_INPUT_Pin);
			break;
	    case eMuxScan_RightTurn_Input3:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			ConfigurableDmuxPinVal[eMux_RightTurn_Input] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			break;
	    case eMuxScan_LeftTurn_Input3:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			NonConfigurableDmuxPinVal[eNonConf_Mux_LeftTurn_Input] = HAL_GPIO_ReadPin(GPIOC, Z4_MUX_INPUT_Pin);
			break;
	    case eMuxScan_SpareAH3_SPARE_AH1_Input:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			ConfigurableDmuxPinVal[eMux_SpareAH3_Input] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			NonConfigurableDmuxPinVal[eNonConf_Mux_SpareAH1_Input] = HAL_GPIO_ReadPin(GPIOC, Z4_MUX_INPUT_Pin);
			break;
	    case eMuxScan_ParkLight_SPARE_AH2_Input:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			ConfigurableDmuxPinVal[eMux_ParkLight_Input] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			NonConfigurableDmuxPinVal[eNonConf_Mux_SpareAH2_Input] = HAL_GPIO_ReadPin(GPIOC, Z4_MUX_INPUT_Pin);
			break;
	    case eMuxScan_RightTurn_Input4:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			ConfigurableDmuxPinVal[eMux_RightTurn_Input] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			break;
	    case eMuxScan_LeftTurn_Input4:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			NonConfigurableDmuxPinVal[eNonConf_Mux_LeftTurn_Input] = HAL_GPIO_ReadPin(GPIOC, Z4_MUX_INPUT_Pin);
			break;
	    case eMuxScan_RightTurn_Turtle_Rabit_Input:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			ConfigurableDmuxPinVal[eMux_RightTurn_Input] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			NonConfigurableDmuxPinVal[eNonConf_Mux_Turtle_Rabit_Input] = HAL_GPIO_ReadPin(GPIOC, Z4_MUX_INPUT_Pin);
			break;
		case eMuxScan_HeadLowLamp_ReverseGear_Input:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			ConfigurableDmuxPinVal[eMux_HeadLowLamp_Input] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			NonConfigurableDmuxPinVal[eNonConf_Mux_ReverseGear_Input] = HAL_GPIO_ReadPin(GPIOC, Z4_MUX_INPUT_Pin);
			break;
	    case eMuxScan_RightTurn_Input5:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			ConfigurableDmuxPinVal[eMux_RightTurn_Input] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			break;
	    case eMuxScan_LeftTurn_Input5:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			NonConfigurableDmuxPinVal[eNonConf_Mux_LeftTurn_Input] = HAL_GPIO_ReadPin(GPIOC, Z4_MUX_INPUT_Pin);
			break;
		case eMuxScan_AirFilterChoke_LeftTurn_Input:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			ConfigurableDmuxPinVal[eMux_AirFilterChoke_Input] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			NonConfigurableDmuxPinVal[eNonConf_Mux_LeftTurn_Input] = HAL_GPIO_ReadPin(GPIOC, Z4_MUX_INPUT_Pin);
			break;
		case eMuxScan_LowBrakeAirPressure_HighBeam_Input:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			ConfigurableDmuxPinVal[eMux_LowBrakeAirPressure_Input] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			NonConfigurableDmuxPinVal[eNonConf_Mux_HighBeam_Input] = HAL_GPIO_ReadPin(GPIOC, Z4_MUX_INPUT_Pin);
			break;
	    case eMuxScan_RightTurn_Input6:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			ConfigurableDmuxPinVal[eMux_RightTurn_Input] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			break;
	    case eMuxScan_LeftTurn_Input6:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			NonConfigurableDmuxPinVal[eNonConf_Mux_LeftTurn_Input] = HAL_GPIO_ReadPin(GPIOC, Z4_MUX_INPUT_Pin);
			break;
		case eMuxScan_HydrolicOilFilter_2W_4W_Input:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			ConfigurableDmuxPinVal[eMux_HydrolicOilFilter_Input] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			NonConfigurableDmuxPinVal[eNonConf_Mux_2W_4W_Input] = HAL_GPIO_ReadPin(GPIOC, Z4_MUX_INPUT_Pin);
			break;
		case eMuxScan_spareAL1__Unused_Input:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			ConfigurableDmuxPinVal[eMux_spareAL1_Input] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			NonConfigurableDmuxPinVal[eMux_Unused_Input16] = HAL_GPIO_ReadPin(GPIOC, Z4_MUX_INPUT_Pin);
			break;
	    case eMuxScan_RightTurn_Input7:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			ConfigurableDmuxPinVal[eMux_RightTurn_Input] = HAL_GPIO_ReadPin(GPIOC, Z2_MUX_INPUT_READ_Pin);
			break;
	    case eMuxScan_LeftTurn_Input7:
			HAL_GPIO_WritePin(GPIOB, S0_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S1_MUX_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, S2_MUX_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, S3_MUX_Pin, GPIO_PIN_SET);
			NonConfigurableDmuxPinVal[eNonConf_Mux_LeftTurn_Input] = HAL_GPIO_ReadPin(GPIOC, Z4_MUX_INPUT_Pin);
			break;
		default:
			/* NA */
		break;

	}

	if(u32GetClockTicks() - TimeStamp >= 5u)
	{
		Status++;
		if(Status >= eMuxScanInputsMaximum)
		{
			Status  = eMuxScan_Neutral_Input;
		}
		TimeStamp = u32GetClockTicks();
	}
}


/*********************************************************************//**
 *
 * update  current status of input in buffer
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/

void _vUpdateMuxPinStatus(void)
{
	sLampInputsProp[eMux_Neutral_Input].u8PinStatus = ConfigurableDmuxPinVal[eMux_Neutral_Input];
	sLampInputsProp[eMux_ParkeBrake_Input].u8PinStatus = ConfigurableDmuxPinVal[eMux_ParkeBrake_Input];
	sLampInputsProp[eMux_Low_High_Input2].u8PinStatus = ConfigurableDmuxPinVal[eMux_Low_High_Input2];
	sLampInputsProp[eMux_Low_High_Input6].u8PinStatus =  ConfigurableDmuxPinVal[eMux_Low_High_Input6];
	sLampInputsProp[eMux_Low_High_Input4].u8PinStatus = ConfigurableDmuxPinVal[eMux_Low_High_Input4];
	sLampInputsProp[eMux_Low_High_Input7].u8PinStatus = ConfigurableDmuxPinVal[eMux_Low_High_Input7];
	sLampInputsProp[eMux_Low_High_Input5].u8PinStatus = ConfigurableDmuxPinVal[eMux_Low_High_Input5];
	sLampInputsProp[eMux_GlowPlug_Input].u8PinStatus = ConfigurableDmuxPinVal[eMux_GlowPlug_Input];
	sLampInputsProp[eMux_SpareAH3_Input].u8PinStatus = ConfigurableDmuxPinVal[eMux_SpareAH3_Input];
	sLampInputsProp[eMux_ParkLight_Input].u8PinStatus = ConfigurableDmuxPinVal[eMux_ParkLight_Input];
	sLampInputsProp[eMux_RightTurn_Input].u8PinStatus = ConfigurableDmuxPinVal[eMux_RightTurn_Input];
	sLampInputsProp[eMux_HeadLowLamp_Input].u8PinStatus = ConfigurableDmuxPinVal[eMux_HeadLowLamp_Input];
	sLampInputsProp[eMux_AirFilterChoke_Input].u8PinStatus = ConfigurableDmuxPinVal[eMux_AirFilterChoke_Input];
	sLampInputsProp[eMux_LowBrakeAirPressure_Input].u8PinStatus = ConfigurableDmuxPinVal[eMux_LowBrakeAirPressure_Input];
	sLampInputsProp[eMux_HydrolicOilFilter_Input].u8PinStatus = ConfigurableDmuxPinVal[eMux_HydrolicOilFilter_Input];
	sLampInputsProp[eMux_spareAL1_Input].u8PinStatus = ConfigurableDmuxPinVal[eMux_spareAL1_Input];
	sLampInputsProp[eMux_BatteryCharging_Input].u8PinStatus = NonConfigurableDmuxPinVal[eNonConf_Mux_BatteryCharging_Input];
	sLampInputsProp[eMux_SpareAH1_Input].u8PinStatus = NonConfigurableDmuxPinVal[eNonConf_Mux_SpareAH1_Input];
	sLampInputsProp[eMux_SpareAH2_Input].u8PinStatus = NonConfigurableDmuxPinVal[eNonConf_Mux_SpareAH2_Input];
	sLampInputsProp[eMux_Turtle_Rabit_Input].u8PinStatus = NonConfigurableDmuxPinVal[eNonConf_Mux_Turtle_Rabit_Input];
	sLampInputsProp[eMux_ReverseGear_Input].u8PinStatus = NonConfigurableDmuxPinVal[eNonConf_Mux_ReverseGear_Input];
	sLampInputsProp[eMux_LeftTurn_Input].u8PinStatus = NonConfigurableDmuxPinVal[eNonConf_Mux_LeftTurn_Input];
	sLampInputsProp[eMux_HighBeam_Input].u8PinStatus = NonConfigurableDmuxPinVal[eNonConf_Mux_HighBeam_Input];
	sLampInputsProp[eMux_2W_4W_Input].u8PinStatus = NonConfigurableDmuxPinVal[eNonConf_Mux_2W_4W_Input];

}


/*********************************************************************//**
*
* Check current and previous status of pins and if input changes then update
* current inputs after complet debouncing time.
*
* @param      None
*
* @return     None
*************************************************************************/

void _vInputsDebounce(void)
{
	uint8_t i;

	for(i = eMux_Neutral_Input; i < eMuxInputsMaximum; i++)
	{
		if(sLampInputsProp[i].u8PinStatus == sLampInputsProp[i].u8PrevStatus)
		{
			sLampInputsProp[i].u32TimeStamp = u32GetClockTicks();
			sLampInputsProp[i].bStatusChange = FALSE;
		}
		else if(sLampInputsProp[i].u8PinStatus != sLampInputsProp[i].u8PrevStatus
		&& sLampInputsProp[i].bStatusChange == FALSE)
		{
			sLampInputsProp[i].u32TimeStamp = u32GetClockTicks();
			sLampInputsProp[i].bStatusChange = TRUE;
		}

		if(u32GetClockTicks() - sLampInputsProp[i].u32TimeStamp > sLampInputsProp[i].u32DebounceTime
		&& sLampInputsProp[i].bStatusChange == TRUE)
		{
			sLampInputsProp[i].u8PrevStatus = sLampInputsProp[i].u8PinStatus;
			sLampInputsProp[i].u32TimeStamp = u32GetClockTicks();
			sLampInputsProp[i].bStatusChange = FALSE;
		}
		if(sLampInputsProp[i].u8PrevStatus)
		{
			sLampInputsProp[i].u8StatusFlag = HIGH;
		}
		else
		{
			sLampInputsProp[i].u8StatusFlag = LOW;
		}
	}
}

/*********************************************************************//**
*
* Update current status of pins in previous inputs buffer
*
* @param      None
*
* @return     None
*************************************************************************/

void vMuxLedPrevStateInit(void)
{
	uint8_t i;

	_vUpdateMuxPinStatus();

	for(i = eMux_Neutral_Input; i < eMuxInputsMaximum ; i++ )
	{
		if((i == eMux_LeftTurn_Input) || (i == eMux_RightTurn_Input))
		{
			sLampInputsProp[i].u8PrevStatus = sLampInputsProp[i].u8PinStatus;
			sLampInputsProp[i].u32DebounceTime = 2U;
		}
		else
		{
			sLampInputsProp[i].u8PrevStatus = sLampInputsProp[i].u8PinStatus;
			sLampInputsProp[i].u32DebounceTime = INPUT_DEBOUNCE_TIME;
		}
	}

}



uint8_t GetLeft_Turn_InputStatus(void)
{
	if(sLampInputsProp[eMux_LeftTurn_Input].u8StatusFlag == 0u)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

uint8_t GetBattCharging_InputStatus(void)
{
	if(sLampInputsProp[eMux_BatteryCharging_Input].u8StatusFlag == 0u)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


uint8_t Park_Light_InputStatus(void)
{
	if(sLampInputsProp[eMux_ParkLight_Input].u8StatusFlag == 0u)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

uint8_t Right_Turn_InputStatus(void)
{
	if(sLampInputsProp[eMux_RightTurn_Input].u8StatusFlag == 0u)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

uint8_t Head_Low_Lamp_InputStatus(void)
{
	if(sLampInputsProp[eMux_HeadLowLamp_Input].u8StatusFlag == 0u)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

uint8_t GetLow_High4_InputStatus(void)
{
	if(sLampInputsProp[eMux_Low_High_Input4].u8StatusFlag == 0u)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

uint8_t GetLow_High5_InputStatus(void)
{
	if(sLampInputsProp[eMux_Low_High_Input5].u8StatusFlag == 0u)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

uint8_t GetLow_High6_InputStatus(void)
{
	if(sLampInputsProp[eMux_Low_High_Input6].u8StatusFlag == 0u)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

uint8_t GetLow_High7_InputStatus(void)
{
	if(sLampInputsProp[eMux_Low_High_Input7].u8StatusFlag == 0u)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

uint8_t GetLow_High2_InputStatus(void)
{
	if(sLampInputsProp[eMux_Low_High_Input2].u8StatusFlag == 0u)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/* This is use for Prake brake input ----------------------*/
uint8_t Park_Brake_InputStatus(void)
{
	if(GetParkingBrakeInputType() == eActiveHigh)
	{
		if(sLampInputsProp[eMux_ParkeBrake_Input].u8StatusFlag == 1u)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if(sLampInputsProp[eMux_ParkeBrake_Input].u8StatusFlag == 0u)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

}

uint8_t GetHydrolic_Oil_Filter_InputStatus(void)
{
	if(sLampInputsProp[eMux_HydrolicOilFilter_Input].u8StatusFlag == 0u)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/* This is use for Nutral input ----------------------*/
uint8_t GetNeutral_InputStatus(void)
{
	if(GetNutralGearInputType() == eActiveHigh)
	{
		if(sLampInputsProp[eMux_Neutral_Input].u8StatusFlag == 1u)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if(sLampInputsProp[eMux_Neutral_Input].u8StatusFlag == 0u)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}

uint8_t Get2W_4W_InputStatus(void)
{
	if(sLampInputsProp[eMux_2W_4W_Input].u8StatusFlag == 0u)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

uint8_t GetAir_Filter_Choke_InputStatus(void)
{
	if(sLampInputsProp[eMux_AirFilterChoke_Input].u8StatusFlag == 0u)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

uint8_t GetSpare_AL1_InputStatus(void)
{
	if(sLampInputsProp[eMux_spareAL1_Input].u8StatusFlag == 0u)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

uint8_t GetSpare_AH1_InputStatus(void)
{
	if(sLampInputsProp[eMux_SpareAH1_Input].u8StatusFlag == 0u)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

uint8_t GetSpare_AH2_InputStatus(void)
{
	if(sLampInputsProp[eMux_SpareAH2_Input].u8StatusFlag == 0u)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

uint8_t GetReverse_Gear_InputStatus(void)
{
	if(sLampInputsProp[eMux_ReverseGear_Input].u8StatusFlag == 0u)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

uint8_t GetGlow__Plug_InputStatus(void)
{
	if(sLampInputsProp[eMux_GlowPlug_Input].u8StatusFlag == 1u)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

uint8_t GetTurtle_Rabbit_InputStatus(void)
{
	if(sLampInputsProp[eMux_Turtle_Rabit_Input].u8StatusFlag == 0u)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

uint8_t GetHigh_Beam_InputStatus(void)
{
	if(sLampInputsProp[eMux_HighBeam_Input].u8StatusFlag == 0u)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
	

static void ConfgurableLampInputInit(void)
{
	if(GetNutralGearInputType() == eActiveHigh)
	{
		sLampInputDrvienType.MuxNuetralInput = HIGH;
	}
	else
	{
		sLampInputDrvienType.MuxNuetralInput = LOW;
	}

	if(GetParkingBrakeInputType() == eActiveHigh)
	{
		sLampInputDrvienType.MuxPrakeBrakeInput = HIGH;
	}
	else
	{
		sLampInputDrvienType.MuxPrakeBrakeInput = LOW;
	}
}







