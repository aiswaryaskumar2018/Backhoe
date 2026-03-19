#include "main.h"



 	/* Total ODO data save in page no 28 and 29 for location 448 to 479 */
#define TOTAL_HOUR_A_ADD        469U
#define TOTAL_HOUR_B_ADD        477U
#define EOL_VEH_OP_ADD          489U
#define EOL_SPD_PULSE_ADD       495U
#define TFT_BRIGHTNESS_ADD      503U
#define BOOTLOADER_IDEN_ADD     507U
#define ODO_FRAC_TRIAB_F_ADD	322U
#define ODO_FRAME_ADDRESS		485U


#define BLANK_OK 	0xFFFFFFFF

extern IWDG_HandleTypeDef hiwdg;

typedef enum
{
	eInValid,
	ePass,
	eFail,
	eInvalidAddress,

}eFlashStatus;



void ReadSystemParametrs(void);
void SaveSysValues(void);


/*---------------------------------EOL Parameter -----------------------------------------*/
void updateEolSpeedoPulsesInFlash(uint32_t Val);
uint32_t GetEolSpeedoPulsesInFlash();
uint16_t GetEolVeicleOptionContentValueInFlash();
void updateEolVeicleOptionContentValueInFlash(uint16_t Data);

#ifdef __cplusplus
extern "C"
{
#endif

uint8_t GetNutralGearInputType(void);
uint8_t GetParkingBrakeInputType(void);
uint8_t GetEngineSelectionType(void);
uint8_t GetBaudRateType(void);

#ifdef __cplusplus
}
#endif
void GetEOl_ParemetersRead(void);
void SaveDefalutEolparamers(void);
void SaveEolSystemParameter(void);


void SaveBootIdentifireParameter(void);
void ReadBootIdentifireParameter(void);
void vSaveBootloaderState(uint8_t data);
uint8_t u8GetBootloaderState();
uint32_t GetLastSavedHourValue(void);

void SetLastSavedHourValueForTripB(uint32_t Value);
void SetLastSavedHourValueForTripA(uint32_t Value);
void SetEolSpeedoPulsesInFlash(uint32_t Val);
void SetEolVeicleOptionContentValueInFlash(uint16_t Val);

void ReadOdoDataAndFractionValue(void);
void ReadTftBrightnessValue(void);
void SaveFractionDigit();
void AllparameterWriteToZero(void);
void TripHourAndTftBrightNessSave(void);
