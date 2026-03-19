#include <gui/screen_screen/screenView.hpp>
#include <MuxDemux.h>

#include "Speedo.h"
#include "main.h"
#include "adc.h"
#include "FuelGauze.h"
#include "BattVolt.h"
#include "TransOilTemp.h"
#include "TransOilPressreure.h"
#include "AirPressureGauge.h"
#include "DefGauge.h"
#include "EngineOilPress.h"
#include "EngineCoolTemp.h"
#include "HourMeter.h"
#include "RpmMeter.h"
#include "SootLoad.h"
#include "TellTales.h"
#include "Odo.h"
#include "Screen.h"
#include "DieselParticulateFilter.h"
#include "POPUPMessages.h"
#include "extEEPROM.h"


DTCLookupTable PreLookUpptr74Hp;
DTCLookupTable PreLookUpptr101Hp;
DTCLookupTable PreLookUpptr49Hp;
DTCLookupTable PreLookUpptr130Hp;

DTCLookupTable *PcodeStartingAddrMainScreen = NULL;
uint16_t* FaultArrayIndexSpnFmiAddrMainScreen;
unsigned char GearValue[4] = {'D','F','N','R'};
int drive_mode_flag=0;
unsigned int limit=0;
bool AF_choke_flag=0;
bool selfCheck = TRUE;
bool PCD_LAMP_flag=FALSE;                         //To enable PCD Lamp make this true
uint32_t Processounter =0;
static uint8_t FirstEntry = TRUE;


const char* PopUpMessageDisplay[eTotalSP2Faults] =  //storing data in ROM(Internal flash as its not changing)
{
	"                       ",
	"     DO MANUAL RGN     ",
	"   MANUAL RGN ACTIVE   ",
	"   MANUAL RGN ABORT    ",
	"  MANUAL RGN COMPLETE  ",
	"    DO SERVICE RGN     ",
	"   RGN STOP PRESSED    "
};

typedef enum
{
	eTransOilTempBar_0 = 0u,
	eTransOilTempBar_1,
	eTransOilTempBar_2,
	eTransOilTempBar_3,
	eTransOilTempBar_4,
	eTransOilTempBar_5,
	eTransOilTempBar_6,
	eTransOilTempBar_7,
	eTransOilTempBar_8,
	eTransOilTempBar_9,
	eTransOilTempBar_10,
	eTransOilTempTotalGaugeBars
};

typedef enum
{
	eHourMeter = 0u,
	eSpeedoOdo,
};

typedef enum
{
	eDisable = 0u,
	eEnable,
	eInvailed
};

typedef enum
{
	eEngOilPressureBar_0 = 0u,
	eEngOilPressureBar_1,
	eEngOilPressureBar_2,
	eEngOilPressureBar_3,
	eEngOilPressureBar_4,
	eEngOilPressureBar_5,
	eEngOilPressureBar_6,
	eEngOilPressureBar_7,
	eEngOilPressureBar_8,
	eEngOilPressureBar_9,
	eEngOilPressureBar_10,
	eEngOilPressureTotalBars
};

screenView::screenView()
{

}

void screenView::setupScreen()
{
    screenViewBase::setupScreen();

    if(Get_exitFlagStatus() == false)
    {
  		box1.setVisible(true);
  		box1.invalidate();
  		textureMapper3.setVisible(true);
  		textureMapper3.invalidate();
	}
	else
	{
		box1.setVisible(false);
		box1.invalidate();
		textureMapper3.setVisible(false);
		textureMapper3.invalidate();
		HomeScreenParameterProcessed();		// this is only for one time init.
	}
    FirstEntry = TRUE;
    PcodeStartingAddrMainScreen = GetPcodeSpnFmiChildScreen();
    FaultArrayIndexSpnFmiAddrMainScreen = GetFaultArrayIndexSpnFmiIndexNo();
}

void screenView::tearDownScreen()
{
    screenViewBase::tearDownScreen();
}



void screenView::handleTickEvent()
{

	static uint8_t OneTimeFlag  = TRUE;
	static uint8_t FirstEntryForAnimation = TRUE;


	if( GetSelfCheckFlagCompleteStatus() == FALSE && OneTimeFlag == TRUE  )
	{
		box1.setVisible(false);
		box1.invalidate();

		//textureMapper1.setVisible(false);
		//textureMapper1.invalidate();
		textureMapper3.setVisible(false);
		textureMapper3.invalidate();
		FirstEntryForAnimation = FALSE;
		OneTimeFlag = FALSE;
		SetExitFlagStatus(TRUE);
		HomeScreenParameterProcessed();
	}

	else if(FirstEntryForAnimation == FALSE )
	{
		++Processounter;
		if(Processounter >= 1u)
		{
			Processounter = 0;
			HomeScreenParameterProcessed();
		}
	}
}

void screenView::HomeScreenParameterProcessed()
{
	Engine_Oil_Bar();
	Trans_Oil_Temp_bar();                    //
	AF_choke();
	FourWheel_drive();
	PCD_Lamp();
	MIL_lamp();
	DPF_lamp();
	Slow_Vehicle();
	SCRtempering();
	FilterClogged();
	scr_fluid();
	tell_tale();
	OdoSpeedoTextHourDisplay();
	incrementText_rpm();
	incrementText_soot_load();
	ShowTransOilTemp();
	showBattvolt();
	PcodeDTC_SPN_FMI_label();
	SelectMenuScreen();
	PopUpMessageDisplay_label();
	EngineCoolantLevelStatus();
}

void screenView::SelectMenuScreen()
{
	static uint8_t screenStatus = 0;

	screenStatus = GetScreenStatus();
	if( GetScreenStatus() == eMenuScreen )
	{
		change_to_menu();
	}
}

void screenView::EngineCoolantLevelStatus()
{
	unsigned int Value = 0;
	unsigned int ValueCoolTemp = 0;

	ValueCoolTemp = GetEngineCoolTempValue();

	degree_icon_1.setVisible(true);
	degree_icon_1.invalidate();
	CoolTempVal.setVisible(true);
	CoolTempVal.invalidate();

	if(GetEngineCoolTempWarningLampStatus() == TRUE)
	{
		EngCoolRedLed.setVisible(true);
		EngCoolRedLed.invalidate();
		EngCoolTempWhiteLed.setVisible(false);
		EngCoolTempWhiteLed.invalidate();
	}
	else
	{
		EngCoolTempWhiteLed.setVisible(true);
		EngCoolTempWhiteLed.invalidate();
		EngCoolRedLed.setVisible(false);
		EngCoolRedLed.invalidate();
	}

	Unicode::snprintf(CoolTempValBuffer, COOLTEMPVAL_SIZE,"%u",ValueCoolTemp);
	CoolTempVal.invalidate();
}


void screenView::AF_choke()
{
	if(GetAir_Filter_Choke_InputStatus() == TRUE)
	{
		AF_choke_enable.setVisible(true);
		AF_choke_enable.invalidate();
	}
	else
	{
		AF_choke_enable.setVisible(false);
		AF_choke_enable.invalidate();
	}
}

void screenView::FourWheel_drive()
{
	if(Get2W_4W_InputStatus() == TRUE)
	{
		FourWheel_drive_enable.setVisible(true);
		FourWheel_drive_enable.invalidate();
	}
	else
	{
		FourWheel_drive_enable.setVisible(false);
		FourWheel_drive_enable.invalidate();
	}
}

void screenView::PCD_Lamp()
{

	if(GetEngineSelectionType() == eBackhoeLoaderEngineM_M74Hp)
	{
		if(GetPCDLampStatus() == TRUE)
		{
			PCD_lamp_enable.setVisible(true);
			PCD_lamp_enable.invalidate();
		}
		else
		{
			PCD_lamp_enable.setVisible(false);
			PCD_lamp_enable.invalidate();
		}
	}
	else
	{
		PCD_lamp_enable.setVisible(false);
		PCD_lamp_enable.invalidate();
		PCD_lamp_disable.setVisible(false);
		PCD_lamp_disable.invalidate();
	}

}


void screenView::MIL_lamp()
{
	uint8_t Status = 0;

	Status = GetMilLampStatus();
	 MIL_lamp_enable.setVisible(Status);  // uper line 4  number
	 MIL_lamp_enable.invalidate();
}

void screenView::DPF_lamp()
{
	uint8_t Status = FALSE;

	Status = GetDPFRegenInhibitLampStatusCAN();

	DPF_lamp_enable.setVisible(Status);
	DPF_lamp_enable.invalidate();
}

void screenView::Slow_Vehicle()
{

}

void screenView::SCRtempering()
{
	uint8_t Status = FALSE;

	Status = FALSE;//GetSCRTemperingLampStatusCAN();

	scr_tempering_enable.setVisible(Status);  // uper line 2 number lamp
	scr_tempering_enable.invalidate();
	scr_tempering_disable.setVisible(Status);  // uper line 2 number lamp
	scr_tempering_disable.invalidate();
}

void screenView::FilterClogged()
{
	if(GetHydrolic_Oil_Filter_InputStatus() == TRUE)
	{
		filter_clogged_enable.setVisible(true);
		filter_clogged_enable.invalidate();
	}
	else
	{
		filter_clogged_enable.setVisible(false);
		filter_clogged_enable.invalidate();
	}
}

void screenView::scr_fluid()
{

}

void screenView::engine_oil_pressure()
{
	if(GetEngineSelectionType() == eBackhoeLoaderEngineTata74Hp)
	{
		engine_oil_pressure_img.setVisible(false);
		engine_oil_pressure_img.invalidate();
	}
	else
	{
//		if(GetEngineOilLevelGaugePercentage() == TRUE)
//				{
					engine_oil_pressure_img.setVisible(true);
					engine_oil_pressure_img.invalidate();
//				}
//				else
//				{
//					engine_oil_pressure_img.setVisible(false);
//					engine_oil_pressure_img.invalidate();
//				}
	}
}

void screenView::tell_tale()
{

}

void screenView::Trans_Oil_Temp_bar()
{
	uint8_t Bar = 0;
	uint8_t Status =  FALSE;

	Bar = GetTransOilTempBarValueInplaceOfAdBlueGauge();

		scr_fluid_enable.setVisible(false);  // Adblue warning LAMP
		scr_fluid_enable.invalidate();

		Status = GetTransOIlTempLedOnTFTStatus();
		if(Status)
				{
					TransOIlTempRedLed.setVisible(true);
					TransOIlTempRedLed.invalidate();
					TransOilTempWhite.setVisible(false);
					TransOilTempWhite.invalidate();
					degree_tt_on.setVisible(true);
					degree_tt_on.invalidate();
					degree_tt.setVisible(false);
					degree_tt.invalidate();
				}
				else
				{
					TransOIlTempRedLed.setVisible(false);
					TransOIlTempRedLed.invalidate();
					TransOilTempWhite.setVisible(true);
					TransOilTempWhite.invalidate();
					degree_tt_on.setVisible(false);
					degree_tt_on.invalidate();
					degree_tt.setVisible(true);
					degree_tt.invalidate();
				}

//	}


	switch(Bar)
	{
		case eTransOilTempBar_1:
			Trans_Oil_Temp_first.setVisible(true);
			Trans_Oil_Temp_first.invalidate();
			 Trans_Oil_Temp_second.setVisible(false);
			 Trans_Oil_Temp_second.invalidate();
			 Trans_Oil_Temp_third.setVisible(false);
			 Trans_Oil_Temp_third.invalidate();
			 Trans_Oil_Temp_fourth.setVisible(false);
			 Trans_Oil_Temp_fourth.invalidate();
			 Trans_Oil_Temp_fifth.setVisible(false);
			 Trans_Oil_Temp_fifth.invalidate();
			 Trans_Oil_Temp_sixth.setVisible(false);
			 Trans_Oil_Temp_sixth.invalidate();
		break;
		case eTransOilTempBar_2:
			Trans_Oil_Temp_first.setVisible(true);
			Trans_Oil_Temp_first.invalidate();
			 Trans_Oil_Temp_second.setVisible(true);
			 Trans_Oil_Temp_second.invalidate();
			 Trans_Oil_Temp_third.setVisible(false);
			 Trans_Oil_Temp_third.invalidate();
			 Trans_Oil_Temp_fourth.setVisible(false);
			 Trans_Oil_Temp_fourth.invalidate();
			 Trans_Oil_Temp_fifth.setVisible(false);
			 Trans_Oil_Temp_fifth.invalidate();
			 Trans_Oil_Temp_sixth.setVisible(false);
			 Trans_Oil_Temp_sixth.invalidate();
		break;
		case eTransOilTempBar_3:
			Trans_Oil_Temp_first.setVisible(true);
			Trans_Oil_Temp_first.invalidate();
			 Trans_Oil_Temp_second.setVisible(true);
			 Trans_Oil_Temp_second.invalidate();
			 Trans_Oil_Temp_third.setVisible(true);
			 Trans_Oil_Temp_third.invalidate();
			 Trans_Oil_Temp_fourth.setVisible(false);
			 Trans_Oil_Temp_fourth.invalidate();
			 Trans_Oil_Temp_fifth.setVisible(false);
			 Trans_Oil_Temp_fifth.invalidate();
			 Trans_Oil_Temp_sixth.setVisible(false);
			 Trans_Oil_Temp_sixth.invalidate();
		 break;
		case eTransOilTempBar_4:
			Trans_Oil_Temp_first.setVisible(true);
			Trans_Oil_Temp_first.invalidate();
			 Trans_Oil_Temp_second.setVisible(true);
			 Trans_Oil_Temp_second.invalidate();
			 Trans_Oil_Temp_third.setVisible(true);
			 Trans_Oil_Temp_third.invalidate();
			 Trans_Oil_Temp_fourth.setVisible(true);
			 Trans_Oil_Temp_fourth.invalidate();
			 Trans_Oil_Temp_fifth.setVisible(false);
			 Trans_Oil_Temp_fifth.invalidate();
			 Trans_Oil_Temp_sixth.setVisible(false);
			 Trans_Oil_Temp_sixth.invalidate();
		break;
		case eTransOilTempBar_5:
			Trans_Oil_Temp_first.setVisible(true);
			Trans_Oil_Temp_first.invalidate();
			 Trans_Oil_Temp_second.setVisible(true);
			 Trans_Oil_Temp_second.invalidate();
			 Trans_Oil_Temp_third.setVisible(true);
			 Trans_Oil_Temp_third.invalidate();
			 Trans_Oil_Temp_fourth.setVisible(true);
			 Trans_Oil_Temp_fourth.invalidate();
			 Trans_Oil_Temp_fifth.setVisible(true);
			 Trans_Oil_Temp_fifth.invalidate();
			 Trans_Oil_Temp_sixth.setVisible(false);
			 Trans_Oil_Temp_sixth.invalidate();
		 break;
		case eTransOilTempBar_6:
			Trans_Oil_Temp_first.setVisible(true);
			Trans_Oil_Temp_first.invalidate();
			 Trans_Oil_Temp_second.setVisible(true);
			 Trans_Oil_Temp_second.invalidate();
			 Trans_Oil_Temp_third.setVisible(true);
			 Trans_Oil_Temp_third.invalidate();
			 Trans_Oil_Temp_fourth.setVisible(true);
			 Trans_Oil_Temp_fourth.invalidate();
			 Trans_Oil_Temp_fifth.setVisible(true);
			 Trans_Oil_Temp_fifth.invalidate();
			 Trans_Oil_Temp_sixth.setVisible(true);
			 Trans_Oil_Temp_sixth.invalidate();
		break;

		default:
			Trans_Oil_Temp_first.setVisible(false);
			Trans_Oil_Temp_first.invalidate();
			Trans_Oil_Temp_second.setVisible(false);
			Trans_Oil_Temp_second.invalidate();
			Trans_Oil_Temp_third.setVisible(false);
			Trans_Oil_Temp_third.invalidate();
			Trans_Oil_Temp_fourth.setVisible(false);
			Trans_Oil_Temp_fourth.invalidate();
			Trans_Oil_Temp_fifth.setVisible(false);
			Trans_Oil_Temp_fifth.invalidate();
			Trans_Oil_Temp_sixth.setVisible(false);
			Trans_Oil_Temp_sixth.invalidate();

		break;
	}

}

void screenView::Engine_Oil_Bar()
{
	uint8_t Bar = 0;

	Bar = GetEngineOilLevelGaugeBars();
	if(GetEngineSelectionType() == eBackhoeLoaderKoelEngine74Hp || GetEngineSelectionType() == eBackhoeLoaderEngineM_M74Hp)
	{
		    engine_oil_pressure_img.setVisible(true);
			engine_oil_pressure_img.invalidate();
			switch(Bar)
			{
				case eEngOilPressureBar_1:
					EngOil1_Blue.setVisible(false);
					EngOil1_Blue.invalidate();
					EngineOil_2Blue.setVisible(false);
					EngineOil_2Blue.invalidate();
					engine_oil_bar_one.setVisible(true);
					engine_oil_bar_one.invalidate();
					engine_oil_bar_two.setVisible(false);
					engine_oil_bar_two.invalidate();
					engine_oil_bar_three.setVisible(false);
					engine_oil_bar_three.invalidate();
					engine_oil_bar_four.setVisible(false);
					engine_oil_bar_four.invalidate();
					engine_oil_bar_five.setVisible(false);
					engine_oil_bar_five.invalidate();
					engine_oil_bar_six.setVisible(false);
					engine_oil_bar_six.invalidate();
					engine_oil_bar_seven.setVisible(false);
					engine_oil_bar_seven.invalidate();
					engine_oil_bar_eight.setVisible(false);
					engine_oil_bar_eight.invalidate();
					engine_oil_bar_nine.setVisible(false);
					engine_oil_bar_nine.invalidate();
					engine_oil_bar_ten.setVisible(false);
					engine_oil_bar_ten.invalidate();

				break;
				case eEngOilPressureBar_2:
					EngOil1_Blue.setVisible(false);
					EngOil1_Blue.invalidate();
					EngineOil_2Blue.setVisible(false);
					EngineOil_2Blue.invalidate();
					engine_oil_bar_one.setVisible(true);
					engine_oil_bar_one.invalidate();
					engine_oil_bar_two.setVisible(true);
					engine_oil_bar_two.invalidate();
					engine_oil_bar_three.setVisible(false);
					engine_oil_bar_three.invalidate();
					engine_oil_bar_four.setVisible(false);
					engine_oil_bar_four.invalidate();
					engine_oil_bar_five.setVisible(false);
					engine_oil_bar_five.invalidate();
					engine_oil_bar_six.setVisible(false);
					engine_oil_bar_six.invalidate();
					engine_oil_bar_seven.setVisible(false);
					engine_oil_bar_seven.invalidate();
					engine_oil_bar_eight.setVisible(false);
					engine_oil_bar_eight.invalidate();
					engine_oil_bar_nine.setVisible(false);
					engine_oil_bar_nine.invalidate();
					engine_oil_bar_ten.setVisible(false);
					engine_oil_bar_ten.invalidate();

				break;
				case eEngOilPressureBar_3:
					EngOil1_Blue.setVisible(true);
					EngOil1_Blue.invalidate();
					EngineOil_2Blue.setVisible(true);
					EngineOil_2Blue.invalidate();
					engine_oil_bar_one.setVisible(false);
					engine_oil_bar_one.invalidate();
					engine_oil_bar_two.setVisible(false);
					engine_oil_bar_two.invalidate();
					engine_oil_bar_three.setVisible(true);
					engine_oil_bar_three.invalidate();
					engine_oil_bar_four.setVisible(false);
					engine_oil_bar_four.invalidate();
					engine_oil_bar_five.setVisible(false);
					engine_oil_bar_five.invalidate();
					engine_oil_bar_six.setVisible(false);
					engine_oil_bar_six.invalidate();
					engine_oil_bar_seven.setVisible(false);
					engine_oil_bar_seven.invalidate();
					engine_oil_bar_eight.setVisible(false);
					engine_oil_bar_eight.invalidate();
					engine_oil_bar_nine.setVisible(false);
					engine_oil_bar_nine.invalidate();
					engine_oil_bar_ten.setVisible(false);
					engine_oil_bar_ten.invalidate();

				break;
				case eEngOilPressureBar_4:
					EngOil1_Blue.setVisible(true);
					EngOil1_Blue.invalidate();
					EngineOil_2Blue.setVisible(true);
					EngineOil_2Blue.invalidate();
					engine_oil_bar_one.setVisible(false);
					engine_oil_bar_one.invalidate();
					engine_oil_bar_two.setVisible(false);
					engine_oil_bar_two.invalidate();
					engine_oil_bar_three.setVisible(true);
					engine_oil_bar_three.invalidate();
					engine_oil_bar_four.setVisible(true);
					engine_oil_bar_four.invalidate();
					engine_oil_bar_five.setVisible(false);
					engine_oil_bar_five.invalidate();
					engine_oil_bar_six.setVisible(false);
					engine_oil_bar_six.invalidate();
					engine_oil_bar_seven.setVisible(false);
					engine_oil_bar_seven.invalidate();
					engine_oil_bar_eight.setVisible(false);
					engine_oil_bar_eight.invalidate();
					engine_oil_bar_nine.setVisible(false);
					engine_oil_bar_nine.invalidate();
					engine_oil_bar_ten.setVisible(false);
					engine_oil_bar_ten.invalidate();

				break;

				case eEngOilPressureBar_5:
					EngOil1_Blue.setVisible(true);
					EngOil1_Blue.invalidate();
					EngineOil_2Blue.setVisible(true);
					EngineOil_2Blue.invalidate();
					engine_oil_bar_one.setVisible(false);
					engine_oil_bar_one.invalidate();
					engine_oil_bar_two.setVisible(false);
					engine_oil_bar_two.invalidate();
					engine_oil_bar_three.setVisible(true);
					engine_oil_bar_three.invalidate();
					engine_oil_bar_four.setVisible(true);
					engine_oil_bar_four.invalidate();
					engine_oil_bar_five.setVisible(true);
					engine_oil_bar_five.invalidate();
					engine_oil_bar_six.setVisible(false);
					engine_oil_bar_six.invalidate();
					engine_oil_bar_seven.setVisible(false);
					engine_oil_bar_seven.invalidate();
					engine_oil_bar_eight.setVisible(false);
					engine_oil_bar_eight.invalidate();
					engine_oil_bar_nine.setVisible(false);
					engine_oil_bar_nine.invalidate();
					engine_oil_bar_ten.setVisible(false);
					engine_oil_bar_ten.invalidate();

				break;

				case eEngOilPressureBar_6:
					EngOil1_Blue.setVisible(true);
					EngOil1_Blue.invalidate();
					EngineOil_2Blue.setVisible(true);
					EngineOil_2Blue.invalidate();
					engine_oil_bar_one.setVisible(false);
					engine_oil_bar_one.invalidate();
					engine_oil_bar_two.setVisible(false);
					engine_oil_bar_two.invalidate();
					engine_oil_bar_three.setVisible(true);
					engine_oil_bar_three.invalidate();
					engine_oil_bar_four.setVisible(true);
					engine_oil_bar_four.invalidate();
					engine_oil_bar_five.setVisible(true);
					engine_oil_bar_five.invalidate();
					engine_oil_bar_six.setVisible(true);
					engine_oil_bar_six.invalidate();
					engine_oil_bar_seven.setVisible(false);
					engine_oil_bar_seven.invalidate();
					engine_oil_bar_eight.setVisible(false);
					engine_oil_bar_eight.invalidate();
					engine_oil_bar_nine.setVisible(false);
					engine_oil_bar_nine.invalidate();
					engine_oil_bar_ten.setVisible(false);
					engine_oil_bar_ten.invalidate();
				break;

				case eEngOilPressureBar_7:
					EngOil1_Blue.setVisible(true);
					EngOil1_Blue.invalidate();
					EngineOil_2Blue.setVisible(true);
					EngineOil_2Blue.invalidate();
					engine_oil_bar_one.setVisible(false);
					engine_oil_bar_one.invalidate();
					engine_oil_bar_two.setVisible(false);
					engine_oil_bar_two.invalidate();
					engine_oil_bar_three.setVisible(true);
					engine_oil_bar_three.invalidate();
					engine_oil_bar_four.setVisible(true);
					engine_oil_bar_four.invalidate();
					engine_oil_bar_five.setVisible(true);
					engine_oil_bar_five.invalidate();
					engine_oil_bar_six.setVisible(true);
					engine_oil_bar_six.invalidate();
					engine_oil_bar_seven.setVisible(true);
					engine_oil_bar_seven.invalidate();
					engine_oil_bar_eight.setVisible(false);
					engine_oil_bar_eight.invalidate();
					engine_oil_bar_nine.setVisible(false);
					engine_oil_bar_nine.invalidate();
					engine_oil_bar_ten.setVisible(false);
					engine_oil_bar_ten.invalidate();

				break;

				case eEngOilPressureBar_8:
					EngOil1_Blue.setVisible(true);
					EngOil1_Blue.invalidate();
					EngineOil_2Blue.setVisible(true);
					EngineOil_2Blue.invalidate();
					engine_oil_bar_one.setVisible(false);
					engine_oil_bar_one.invalidate();
					engine_oil_bar_two.setVisible(false);
					engine_oil_bar_two.invalidate();
					engine_oil_bar_three.setVisible(true);
					engine_oil_bar_three.invalidate();
					engine_oil_bar_four.setVisible(true);
					engine_oil_bar_four.invalidate();
					engine_oil_bar_five.setVisible(true);
					engine_oil_bar_five.invalidate();
					engine_oil_bar_six.setVisible(true);
					engine_oil_bar_six.invalidate();
					engine_oil_bar_seven.setVisible(true);
					engine_oil_bar_seven.invalidate();
					engine_oil_bar_eight.setVisible(true);
					engine_oil_bar_eight.invalidate();
					engine_oil_bar_nine.setVisible(false);
					engine_oil_bar_nine.invalidate();
					engine_oil_bar_ten.setVisible(false);
					engine_oil_bar_ten.invalidate();

				break;
				case eEngOilPressureBar_9:

					EngOil1_Blue.setVisible(true);
					EngOil1_Blue.invalidate();
					EngineOil_2Blue.setVisible(true);
					EngineOil_2Blue.invalidate();
					engine_oil_bar_one.setVisible(false);
					engine_oil_bar_one.invalidate();
					engine_oil_bar_two.setVisible(false);
					engine_oil_bar_two.invalidate();
					engine_oil_bar_three.setVisible(true);
					engine_oil_bar_three.invalidate();
					engine_oil_bar_four.setVisible(true);
					engine_oil_bar_four.invalidate();
					engine_oil_bar_five.setVisible(true);
					engine_oil_bar_five.invalidate();
					engine_oil_bar_six.setVisible(true);
					engine_oil_bar_six.invalidate();
					engine_oil_bar_seven.setVisible(true);
					engine_oil_bar_seven.invalidate();
					engine_oil_bar_eight.setVisible(true);
					engine_oil_bar_eight.invalidate();
					engine_oil_bar_nine.setVisible(true);
					engine_oil_bar_nine.invalidate();
					engine_oil_bar_ten.setVisible(false);
					engine_oil_bar_ten.invalidate();

				break;

				case eEngOilPressureBar_10:

					EngOil1_Blue.setVisible(true);
					EngOil1_Blue.invalidate();
					EngineOil_2Blue.setVisible(true);
					EngineOil_2Blue.invalidate();
					engine_oil_bar_one.setVisible(false);
					engine_oil_bar_one.invalidate();
					engine_oil_bar_two.setVisible(false);
					engine_oil_bar_two.invalidate();
					engine_oil_bar_three.setVisible(true);
					engine_oil_bar_three.invalidate();
					engine_oil_bar_four.setVisible(true);
					engine_oil_bar_four.invalidate();
					engine_oil_bar_five.setVisible(true);
					engine_oil_bar_five.invalidate();
					engine_oil_bar_six.setVisible(true);
					engine_oil_bar_six.invalidate();
					engine_oil_bar_seven.setVisible(true);
					engine_oil_bar_seven.invalidate();
					engine_oil_bar_eight.setVisible(true);
					engine_oil_bar_eight.invalidate();
					engine_oil_bar_nine.setVisible(true);
					engine_oil_bar_nine.invalidate();
					engine_oil_bar_ten.setVisible(true);
					engine_oil_bar_ten.invalidate();
				break;
				default:
					EngOil1_Blue.setVisible(false);
					EngOil1_Blue.invalidate();
					EngineOil_2Blue.setVisible(false);
					EngineOil_2Blue.invalidate();
					engine_oil_bar_one.setVisible(false);
					engine_oil_bar_one.invalidate();
					engine_oil_bar_two.setVisible(false);
					engine_oil_bar_two.invalidate();
					engine_oil_bar_three.setVisible(false);
					engine_oil_bar_three.invalidate();
					engine_oil_bar_four.setVisible(false);
					engine_oil_bar_four.invalidate();
					engine_oil_bar_five.setVisible(false);
					engine_oil_bar_five.invalidate();
					engine_oil_bar_six.setVisible(false);
					engine_oil_bar_six.invalidate();
					engine_oil_bar_seven.setVisible(false);
					engine_oil_bar_seven.invalidate();
					engine_oil_bar_eight.setVisible(false);
					engine_oil_bar_eight.invalidate();
					engine_oil_bar_nine.setVisible(false);
					engine_oil_bar_nine.invalidate();
					engine_oil_bar_ten.setVisible(false);
					engine_oil_bar_ten.invalidate();
				break;
			}
		}
	else
	{
		engine_oil_pressure_bar_unfilled.setVisible(false);
		engine_oil_pressure_bar_unfilled.invalidate();
		EngOil1_Blue.setVisible(false);
		EngOil1_Blue.invalidate();
		EngineOil_2Blue.setVisible(false);
		EngineOil_2Blue.invalidate();
		engine_oil_bar_one.setVisible(false);
		engine_oil_bar_one.invalidate();
		engine_oil_bar_two.setVisible(false);
		engine_oil_bar_two.invalidate();
		engine_oil_bar_three.setVisible(false);
		engine_oil_bar_three.invalidate();
		engine_oil_bar_four.setVisible(false);
		engine_oil_bar_four.invalidate();
		engine_oil_bar_five.setVisible(false);
		engine_oil_bar_five.invalidate();
		engine_oil_bar_six.setVisible(false);
		engine_oil_bar_six.invalidate();
		engine_oil_bar_seven.setVisible(false);
		engine_oil_bar_seven.invalidate();
		engine_oil_bar_eight.setVisible(false);
		engine_oil_bar_eight.invalidate();
		engine_oil_bar_nine.setVisible(false);
		engine_oil_bar_nine.invalidate();
		engine_oil_bar_ten.setVisible(false);
		engine_oil_bar_ten.invalidate();

		engine_oil_pressure_img.setVisible(false);
		engine_oil_pressure_img.invalidate();
	}

}

void screenView::PcodeDTC_SPN_FMI_label()
{
	static uint32_t PcodeTimeStamp = 0;
		static uint8_t PcodeCounter = 0;

		uint8_t NoOfFault = getFaultNumber();  // Get number of faults

		if ((NoOfFault > 0u) || (GetSinglePacketPcodeMatchingStatus()))
		{
		    if (FirstEntry == TRUE)
		    {
		   		if( ((PcodeStartingAddrMainScreen + (*(FaultArrayIndexSpnFmiAddrMainScreen + PcodeCounter)))->SPN) == 0u
		    				&& ((PcodeStartingAddrMainScreen + (*(FaultArrayIndexSpnFmiAddrMainScreen + PcodeCounter)))->FMI) == 0u)
				{
					SpnValue.setVisible(false);
					SpnValue.invalidate();
					FmiValue.setVisible(false);
					FmiValue.invalidate();
					PcodeVal.setVisible(false);
					PcodeVal.invalidate();
				}
				else
				{
					SpnValue.setVisible(true);
					SpnValue.invalidate();
					FmiValue.setVisible(true);
					FmiValue.invalidate();
					PcodeVal.setVisible(true);
					PcodeVal.invalidate();
					// Update UI with the current PCODE info
					Unicode::snprintf(SpnValueBuffer, SPNVALUE_SIZE, "%u",((PcodeStartingAddrMainScreen + (*(FaultArrayIndexSpnFmiAddrMainScreen + PcodeCounter)))->SPN));
					SpnValue.invalidate();
					Unicode::snprintf(FmiValueBuffer, FMIVALUE_SIZE, "%u",((PcodeStartingAddrMainScreen + (*(FaultArrayIndexSpnFmiAddrMainScreen + PcodeCounter)))->FMI));
					FmiValue.invalidate();
					Unicode::strncpy(PcodeValBuffer,(const char *)((PcodeStartingAddrMainScreen + (*(FaultArrayIndexSpnFmiAddrMainScreen + PcodeCounter)))->DTCTxts),PCODEVAL_SIZE);
					PcodeVal.invalidate();
				}
		        FirstEntry = FALSE;
		        PcodeTimeStamp = u32GetClockTicks();  // Initialize timestamp on first entry
		        PcodeTimeStamp = 1500;
		    }

		    if (NoOfFault <= 1u)
		    {
		        PcodeCounter = 0u;
	    		if( ((PcodeStartingAddrMainScreen + (*(FaultArrayIndexSpnFmiAddrMainScreen + PcodeCounter)))->SPN) == 0u
	    				&& ((PcodeStartingAddrMainScreen + (*(FaultArrayIndexSpnFmiAddrMainScreen + PcodeCounter)))->FMI) == 0u)
	    		{
	    		    SpnValue.setVisible(false);
	    		    SpnValue.invalidate();
	    		    FmiValue.setVisible(false);
	    		    FmiValue.invalidate();
	    		    PcodeVal.setVisible(false);
	    		    PcodeVal.invalidate();
	    		}
	    		else
	    		{
	    	        SpnValue.setVisible(true);
	    	        SpnValue.invalidate();
	    	        FmiValue.setVisible(true);
	    	        FmiValue.invalidate();
	    	        PcodeVal.setVisible(true);
	    	        PcodeVal.invalidate();
	    	 	    // Update UI with the current PCODE info
	    			Unicode::snprintf(SpnValueBuffer, SPNVALUE_SIZE, "%u",((PcodeStartingAddrMainScreen + (*(FaultArrayIndexSpnFmiAddrMainScreen + PcodeCounter)))->SPN));
	    			SpnValue.invalidate();
	    			Unicode::snprintf(FmiValueBuffer, FMIVALUE_SIZE, "%u",((PcodeStartingAddrMainScreen + (*(FaultArrayIndexSpnFmiAddrMainScreen + PcodeCounter)))->FMI));
	    			FmiValue.invalidate();
	    			Unicode::strncpy(PcodeValBuffer,(const char *)((PcodeStartingAddrMainScreen + (*(FaultArrayIndexSpnFmiAddrMainScreen + PcodeCounter)))->DTCTxts),PCODEVAL_SIZE);
	    			PcodeVal.invalidate();
	    		}
				PcodeTimeStamp = u32GetClockTicks();  // Update timestamp only on rotation
		    }
		    else
		    {
		        // Rotate every 1500 ticks (e.g., ~3 seconds if 1 tick = 2 ms)
		        if ((u32GetClockTicks() - PcodeTimeStamp) >= 1500u)
		        {
		            PcodeCounter++;
		            if (PcodeCounter >= NoOfFault)
		            {
		                PcodeCounter = 0u;
		            }

		    		if( ((PcodeStartingAddrMainScreen + (*(FaultArrayIndexSpnFmiAddrMainScreen + PcodeCounter)))->SPN) == 0u
		    				&& ((PcodeStartingAddrMainScreen + (*(FaultArrayIndexSpnFmiAddrMainScreen + PcodeCounter)))->FMI) == 0u)
		    		{
		    		    SpnValue.setVisible(false);
		    		    SpnValue.invalidate();
		    		    FmiValue.setVisible(false);
		    		    FmiValue.invalidate();
		    		    PcodeVal.setVisible(false);
		    		    PcodeVal.invalidate();
		    		}
		    		else
		    		{
		    	        SpnValue.setVisible(true);
		    	        SpnValue.invalidate();
		    	        FmiValue.setVisible(true);
		    	        FmiValue.invalidate();
		    	        PcodeVal.setVisible(true);
		    	        PcodeVal.invalidate();
		    	 	    // Update UI with the current PCODE info
		    			Unicode::snprintf(SpnValueBuffer, SPNVALUE_SIZE, "%u",((PcodeStartingAddrMainScreen + (*(FaultArrayIndexSpnFmiAddrMainScreen + PcodeCounter)))->SPN));
		    			SpnValue.invalidate();
		    			Unicode::snprintf(FmiValueBuffer, FMIVALUE_SIZE, "%u",((PcodeStartingAddrMainScreen + (*(FaultArrayIndexSpnFmiAddrMainScreen + PcodeCounter)))->FMI));
		    			FmiValue.invalidate();
		    			Unicode::strncpy(PcodeValBuffer,(const char *)((PcodeStartingAddrMainScreen + (*(FaultArrayIndexSpnFmiAddrMainScreen + PcodeCounter)))->DTCTxts),PCODEVAL_SIZE);
		    			PcodeVal.invalidate();
		    		}
		            PcodeTimeStamp = u32GetClockTicks();  // Update timestamp only on rotation
		        }
		        else
		        {

		        }
		    }
		}
		else
		{
		    // No faults present, hide the fields
		    SpnValue.setVisible(false);
		    SpnValue.invalidate();
		    FmiValue.setVisible(false);
		    FmiValue.invalidate();
		    PcodeVal.setVisible(false);
		    PcodeVal.invalidate();
		    FirstEntry = TRUE;
		    PcodeCounter = 0u;
		    PcodeTimeStamp = u32GetClockTicks();  // Reset timestamp
		}
}

void screenView::PopUpMessageDisplay_label()
{
	uint8_t PopUpIndex = 0u;
	// storing multiple strings using pointer
	PopUpMsg.setVisible(true);
	PopUpMsg.invalidate();

	PopUpIndex = GetSetTopLineMsgNumber();
	Unicode::strncpy(PopUpMsgBuffer,(const char *)PopUpMessageDisplay[PopUpIndex], POPUPMSG_SIZE);
	PopUpMsg.invalidate();

}

void screenView::incrementText_rpm()
{
	uint16_t Data = 0;

	Data = u16GetEngineSpeed();
	//Data = GetAdcValueForDisplay();	// This is only for ADC value display on LCD

	Unicode::snprintf(rpm_valueBuffer, RPM_VALUE_SIZE,"%d",Data);
	rpm_value.invalidate();

}


void screenView::incrementText_odo()
{


}


void screenView::OdoSpeedoTextHourDisplay()
{
	uint32_t OdoValueBuffer1 = 0;
	unsigned char OdoValueBuffer2 = 0;
	unsigned int Value = 0;
	uint8_t SpeedoVal = 0;
	uint8_t BlinkStatus = 0;
	unsigned int OdoTripValueBuffer1 = 0;
	unsigned char OdoTripValueBuffer2 = 0;
	static uint32_t ValueTrip = 0;
	static uint32_t ValueTripHour = 0;

	fuel_range.setVisible(false);	// this is upper line hour meter using but as per customer ,hide this
	fuel_range.invalidate();

	textArea1.setVisible(false);
	textArea1.invalidate();

	km_h_label.setVisible(false);
	km_h_label.invalidate();

	km_label.setVisible(false);
	km_label.invalidate();

	trip_km_label.setVisible(false);
	trip_km_label.invalidate();

	speedo_value.setVisible(false);
	speedo_value.invalidate();

	TripHrLbl.setVisible(true);
	TripHrLbl.invalidate();

	TotHrLble.setVisible(true);
	TotHrLble.invalidate();

	BlinkStatus = GetHourSymbolBlinkStatus();

	fuel_tt.setVisible(BlinkStatus);
	fuel_tt.invalidate();

	Value = GetHourDataValue();
	//Value = GetAdcValueForDisplay();
	// Last digit of value
	OdoValueBuffer2 = Value % 10;

	// Avoid Last digit
	OdoValueBuffer1 = Value / 10;

	// Last digit of value
	OdoTripValueBuffer2 = ValueTripHour % 10;

	// Avoid Last digit
	OdoTripValueBuffer1 = ValueTripHour / 10;

	if(GetHourDataComingOrNotStatus() == FALSE)
	{
		odo_value.setVisible(true);
		odo_value.invalidate();
		trip_value.setVisible(true);
		trip_value.invalidate();

		Unicode::snprintf(odo_valueBuffer1, ODO_VALUEBUFFER1_SIZE,"%u",OdoValueBuffer1);
		odo_value.invalidate();
		Unicode::snprintf(odo_valueBuffer2, ODO_VALUEBUFFER2_SIZE,"%u",OdoValueBuffer2);
		odo_value.invalidate();
		Unicode::snprintf(trip_valueBuffer2, TRIP_VALUEBUFFER2_SIZE, "%u",OdoTripValueBuffer2);
		trip_value.invalidate();
		Unicode::snprintf(trip_valueBuffer1, TRIP_VALUEBUFFER1_SIZE,"%u",OdoTripValueBuffer1);
		trip_value.invalidate();
	}
	else
	{
		odo_value.setVisible(false);
		odo_value.invalidate();
		trip_value.setVisible(false);
		trip_value.invalidate();
	}


	if(GetvShowTrip_A_B_DataOnTFT() == eScreenTripA)
	{
		ValueTrip = GetDisplayTripAFinalMergedValue();
		ValueTripHour = GetTripAHourValue();
		trip_a_label.setVisible(true);
		trip_a_label.invalidate();
		trip_b_label.setVisible(false);
		trip_b_label.invalidate();
	}
	else
	{
		ValueTrip =	GetDisplayTripBFinalMergedValue();
		ValueTripHour = GetTripBHourValue();
		trip_a_label.setVisible(false);
		trip_a_label.invalidate();
		trip_b_label.setVisible(true);
		trip_b_label.invalidate();
	}

}


void screenView::incrementText_trip()
{



}

void screenView::incrementText_AP()
{
	unsigned int ApValueBuffer1 = 0;
	unsigned char ApValueBuffer2 = 0;
	unsigned int Value = 0;
	unsigned int ValueCoolTemp = 0;


	// Last digit of value

	if(GetEngineSelectionType() == eEnable)
	{
		Value = getAirPressure();

		AP_LABEL.setVisible(true);
		AP_LABEL.invalidate();
		Ap_value.setVisible(true);
		Ap_value.invalidate();
		Ap_value_label.setVisible(true);
		Ap_value_label.invalidate();

		degree_icon_1.setVisible(false);
		degree_icon_1.invalidate();
		CoolTempVal.setVisible(false);
		CoolTempVal.invalidate();
		EngCoolTempWhiteLed.setVisible(false);
		EngCoolTempWhiteLed.invalidate();
		EngCoolRedLed.setVisible(false);
		EngCoolRedLed.invalidate();

		ApValueBuffer2 = Value % 10;
		// Avoid Last digit
		ApValueBuffer1 = Value / 10;

		Unicode::snprintf(Ap_valueBuffer2, AP_VALUEBUFFER2_SIZE, "%u",ApValueBuffer2);
		Ap_value.invalidate();

		Unicode::snprintf(Ap_valueBuffer1, AP_VALUEBUFFER1_SIZE,"%u",ApValueBuffer1);
		Ap_value.invalidate();
	}
	else
	{
		ValueCoolTemp = GetEngineCoolTempValue();
		AP_LABEL.setVisible(false);
		AP_LABEL.invalidate();
		Ap_value.setVisible(false);
		Ap_value.invalidate();
		Ap_value_label.setVisible(false);
		Ap_value_label.invalidate();

		degree_icon_1.setVisible(true);
		degree_icon_1.invalidate();
		CoolTempVal.setVisible(true);
		CoolTempVal.invalidate();

		if(GetEngineCoolTempWarningLampStatus() == TRUE)
		{
			EngCoolRedLed.setVisible(true);
			EngCoolRedLed.invalidate();
			EngCoolTempWhiteLed.setVisible(false);
			EngCoolTempWhiteLed.invalidate();
		}
		else
		{
			EngCoolTempWhiteLed.setVisible(true);
			EngCoolTempWhiteLed.invalidate();
			EngCoolRedLed.setVisible(false);
			EngCoolRedLed.invalidate();
		}

		Unicode::snprintf(CoolTempValBuffer, COOLTEMPVAL_SIZE,"%u",ValueCoolTemp);
		CoolTempVal.invalidate();
	}

}

void screenView::incrementText_HourMeter()
{


}

void screenView::incrementText_speedo_value()
{


}

void screenView::incrementText_soot_load()
{
	uint16_t data = 0;
	uint32_t ValueDpf = 0;
	uint8_t DPFValueBuff2 = 0;
	uint16_t DPFValueBuff1 = 0;
	uint8_t engineSelection =0;
	// Avoid Last digit

	if(GetEngineSelectionType() == eBackhoeLoaderKoelEngine74Hp)
	{
		data = u16GetSootLoadValue();

		soot_load.setVisible(true);
		soot_load.invalidate();
		SOOT_LOAD_LABEL.setVisible(true);
		SOOT_LOAD_LABEL.invalidate();
		percent_label.setVisible(false);
		percent_label.invalidate();
		grams_label.setVisible(true);
		grams_label.invalidate();

		KpaValue.setVisible(false);
		KpaValue.invalidate();

		DpfText.setVisible(false);
		DpfText.invalidate();

		KpaText.setVisible(false);
		KpaText.invalidate();

		if(u8GetSootLoadBadDataTimeoutStatus() == FALSE)
		{
			Unicode::snprintf(soot_loadBuffer, SOOT_LOAD_SIZE, "%u",data);
			soot_load.invalidate();
		}
		else
		{
			Unicode::snprintf(soot_loadBuffer, SOOT_LOAD_SIZE, "0");
			soot_load.invalidate();
		}
	}
	else
	{
		data = u16GetSootLoadValue();

		soot_load.setVisible(true);
		soot_load.invalidate();
		SOOT_LOAD_LABEL.setVisible(true);
		SOOT_LOAD_LABEL.invalidate();
		percent_label.setVisible(true);
		percent_label.invalidate();
		grams_label.setVisible(false);
		grams_label.invalidate();

		KpaValue.setVisible(false);
		KpaValue.invalidate();

		DpfText.setVisible(false);
		DpfText.invalidate();

		KpaText.setVisible(false);
		KpaText.invalidate();

		if(u8GetSootLoadBadDataTimeoutStatus() == FALSE)
		{
			Unicode::snprintf(soot_loadBuffer, SOOT_LOAD_SIZE, "%u",data);
			soot_load.invalidate();
		}
		else
		{
			Unicode::snprintf(soot_loadBuffer, SOOT_LOAD_SIZE, "0");
			soot_load.invalidate();
		}
	}

}


void screenView::ShowTransOilTemp()
{
	uint8_t	oil_temp = GetTransOilTempVaLue();

	Unicode::snprintf(degree_valueBuffer, DEGREE_VALUE_SIZE,"%u",oil_temp);
	degree_value.invalidate();

}

void screenView::showBattvolt(void)
{
	unsigned int BattValueBuffer1 = 0;
	unsigned char BattValueBuffer2 = 0;
	unsigned int Value = 0;

	Value = get_battery_volt();
	// Last digit of value
	BattValueBuffer2 = Value % 10;
	// Avoid Last digit
	BattValueBuffer1 = Value / 10;

	Unicode::snprintf(volt_valueBuffer1, VOLT_VALUEBUFFER1_SIZE,"%u",BattValueBuffer1);
	volt_value.invalidate();

	Unicode::snprintf(volt_valueBuffer2, VOLT_VALUEBUFFER2_SIZE,"%u",BattValueBuffer2);
	volt_value.invalidate();


}

