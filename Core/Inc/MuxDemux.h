#ifndef __RD_WR_TIC12400_H
#define __RD_WR_TIC12400_H

#include "main.h"



 
#define	DigitalCase		1u
#define	AnalogCase		0u
  



typedef enum
{
	eDmux_Write_Neutral_EN_Input = 0u,
	eDmux_Write_ParkeBrake_EN_Input,
	eDmux_Write_Low_High_Input2,
	eDmux_Write_Low_High_Input6,
	eDmux_Write_Low_High_Input4,
	eDmux_Write_Low_High_Input7,
	eDmux_Write_Low_High_Input5,
	eDmux_Write_spare_Input1,
	eDmux_Write_spare_Input2,
	eDmux_Write_spare_Input3,
	eDmux_Write_spare_Input4,
	eDmux_Write_spare_Input5,
	eDmux_Write_spare_Input6,
	eDmux_Write_spare_Input7,
	eDmux_Write_spare_Input8,
	eDmux_Write_spare_Input9,
	eDmuxWriteMaximum
};

typedef enum
{
	eMux_Neutral_Input = 0u,
	eMux_ParkeBrake_Input,
	eMux_Low_High_Input2,
	eMux_Low_High_Input6,
	eMux_Low_High_Input4,
	eMux_Low_High_Input7,
	eMux_Low_High_Input5,
	eMux_GlowPlug_Input,
	eMux_SpareAH3_Input,
	eMux_ParkLight_Input,
	eMux_RightTurn_Input,
	eMux_HeadLowLamp_Input,
	eMux_AirFilterChoke_Input,
	eMux_LowBrakeAirPressure_Input,
	eMux_HydrolicOilFilter_Input,
	eMux_spareAL1_Input,
	eMux_BatteryCharging_Input,
	eMux_SpareAH1_Input,
	eMux_SpareAH2_Input,
	eMux_Turtle_Rabit_Input,
	eMux_ReverseGear_Input,
	eMux_LeftTurn_Input,
	eMux_HighBeam_Input,
	eMux_2W_4W_Input,
	eMuxInputsMaximum
};


typedef enum
{
	eMux_Unused_Input1 = 0u,
	eMux_Unused_Input2,
	eMux_Unused_Input3,
	eMux_Unused_Input4,
	eMux_Unused_Input5,
	eMux_Unused_Input6,
	eMux_Unused_Input7,
	eNonConf_Mux_BatteryCharging_Input,
	eNonConf_Mux_SpareAH1_Input,
	eNonConf_Mux_SpareAH2_Input,
	eNonConf_Mux_Turtle_Rabit_Input,
	eNonConf_Mux_ReverseGear_Input,
	eNonConf_Mux_LeftTurn_Input,
	eNonConf_Mux_HighBeam_Input,
	eNonConf_Mux_2W_4W_Input,
	eMux_Unused_Input16,
	eNonConf_MuxInputsMaximum
};

enum
{
    eMuxScan_Neutral_Input = 0u,
    eMuxScan_ParkeBrake_Input,
    eMuxScan_RightTurn_Input,
    eMuxScan_LeftTurn_Input,
    eMuxScan_Low_High_Input2,
    eMuxScan_Low_High_Input6,
    eMuxScan_RightTurn_Input1,
    eMuxScan_LeftTurn_Input1,
    eMuxScan_Low_High_Input4,
    eMuxScan_Low_High_Input7,
    eMuxScan_RightTurn_Input2,
    eMuxScan_LeftTurn_Input2,
    eMuxScan_Low_High_Input5,
    eMuxScan_GlowPlug_BatteryCharging_Input,
    eMuxScan_RightTurn_Input3,
    eMuxScan_LeftTurn_Input3,
	eMuxScan_SpareAH3_SPARE_AH1_Input,
	eMuxScan_ParkLight_SPARE_AH2_Input,
    eMuxScan_RightTurn_Input4,
    eMuxScan_LeftTurn_Input4,
    eMuxScan_RightTurn_Turtle_Rabit_Input,
    eMuxScan_HeadLowLamp_ReverseGear_Input,
    eMuxScan_RightTurn_Input5,
    eMuxScan_LeftTurn_Input5,
    eMuxScan_AirFilterChoke_LeftTurn_Input,
    eMuxScan_LowBrakeAirPressure_HighBeam_Input,
    eMuxScan_RightTurn_Input6,
    eMuxScan_LeftTurn_Input6,
    eMuxScan_HydrolicOilFilter_2W_4W_Input,
    eMuxScan_spareAL1__Unused_Input,
    eMuxScan_RightTurn_Input7,
    eMuxScan_LeftTurn_Input7,
    eMuxScanInputsMaximum
};





#ifdef __cplusplus
extern "C"
{
#endif
void MuxDmuxProcessing(void);
void _vUpdateMuxPinStatus(void);
void _vInputsDebounce(void);
void vMuxLedPrevStateInit(void);
uint8_t GetLeft_Turn_InputStatus(void);
uint8_t GetBattCharging_InputStatus(void);
uint8_t Park_Brake_InputStatus(void);
uint8_t Park_Light_InputStatus(void);
uint8_t Right_Turn_InputStatus(void);
uint8_t Head_Low_Lamp_InputStatus(void); //on TFT
uint8_t GetLow_High4_InputStatus(void);
uint8_t GetLow_High5_InputStatus(void);
uint8_t GetLow_High6_InputStatus(void);
uint8_t GetLow_High7_InputStatus(void);
uint8_t GetLow_High2_InputStatus(void);
uint8_t GetHydrolic_Oil_Filter_InputStatus(void); //on TFT 1 num
uint8_t GetNeutral_InputStatus(void);
uint8_t Get2W_4W_InputStatus(void); //on TFT
uint8_t GetAir_Filter_Choke_InputStatus(void); //on TFT
uint8_t GetSpare_AL1_InputStatus(void);
uint8_t GetSpare_AH1_InputStatus(void);
uint8_t GetSpare_AH2_InputStatus(void);
uint8_t GetReverse_Gear_InputStatus(void); //on TFT
uint8_t GetGlow__Plug_InputStatus(void);
uint8_t GetTurtle_Rabbit_InputStatus(void); //on TFT rabbit and trotroise
uint8_t GetHigh_Beam_InputStatus(void);

#ifdef __cplusplus
}
#endif

#endif 
