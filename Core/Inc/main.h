/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
extern FDCAN_TxHeaderTypeDef TxHeader;
extern FDCAN_RxHeaderTypeDef RxHeader;

extern FDCAN_HandleTypeDef hfdcan1;
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MOD_BUTTON_SW2_Pin GPIO_PIN_0
#define MOD_BUTTON_SW2_GPIO_Port GPIOA
#define SET_BUTTON_SW1_Pin GPIO_PIN_1
#define SET_BUTTON_SW1_GPIO_Port GPIOA
#define LED_DRIVER_G1_Pin GPIO_PIN_4
#define LED_DRIVER_G1_GPIO_Port GPIOA
#define LED_DRIVER_LATCH_Pin GPIO_PIN_4
#define LED_DRIVER_LATCH_GPIO_Port GPIOC
#define LED_DRIVER_CLR_Pin GPIO_PIN_5
#define LED_DRIVER_CLR_GPIO_Port GPIOC
#define I2C_SCL_PIN_Pin GPIO_PIN_10
#define I2C_SCL_PIN_GPIO_Port GPIOB
#define Z0_AnalogMuxWritePIN_Pin GPIO_PIN_12
#define Z0_AnalogMuxWritePIN_GPIO_Port GPIOB
#define CAN_5V_ENABLE_Pin GPIO_PIN_15
#define CAN_5V_ENABLE_GPIO_Port GPIOB
#define I2C_SDA_PIN_Pin GPIO_PIN_13
#define I2C_SDA_PIN_GPIO_Port GPIOD
#define LED_DRIVER_G2_Pin GPIO_PIN_6
#define LED_DRIVER_G2_GPIO_Port GPIOC
#define S2_Pin GPIO_PIN_7
#define S2_GPIO_Port GPIOC
#define S1_Pin GPIO_PIN_8
#define S1_GPIO_Port GPIOC
#define S0_Pin GPIO_PIN_9
#define S0_GPIO_Port GPIOC
#define HIGH_OUTPUT__IN_Pin GPIO_PIN_8
#define HIGH_OUTPUT__IN_GPIO_Port GPIOA
#define HIGH_OUTPUT_FAULT_Pin GPIO_PIN_9
#define HIGH_OUTPUT_FAULT_GPIO_Port GPIOA
#define LOW_OUT_PUT_ENABLE_Pin GPIO_PIN_10
#define LOW_OUT_PUT_ENABLE_GPIO_Port GPIOA
#define MUX_Z3_Write_Pin GPIO_PIN_10
#define MUX_Z3_Write_GPIO_Port GPIOC
#define Z2_MUX_INPUT_READ_Pin GPIO_PIN_11
#define Z2_MUX_INPUT_READ_GPIO_Port GPIOC
#define Z4_MUX_INPUT_Pin GPIO_PIN_12
#define Z4_MUX_INPUT_GPIO_Port GPIOC
#define S3_MUX_Pin GPIO_PIN_2
#define S3_MUX_GPIO_Port GPIOD
#define LCD_RESET_Pin_Pin GPIO_PIN_4
#define LCD_RESET_Pin_GPIO_Port GPIOD
#define IGN_SENSE_Pin GPIO_PIN_5
#define IGN_SENSE_GPIO_Port GPIOD
#define CAN_STB_Pin GPIO_PIN_7
#define CAN_STB_GPIO_Port GPIOD
#define TRANS_OIL_TEMP_ENABLE_Pin GPIO_PIN_3
#define TRANS_OIL_TEMP_ENABLE_GPIO_Port GPIOB
#define LDO_ENABLE_PWR_Pin GPIO_PIN_4
#define LDO_ENABLE_PWR_GPIO_Port GPIOB
#define S2_MUX_Pin GPIO_PIN_5
#define S2_MUX_GPIO_Port GPIOB
#define S1_MUX_Pin GPIO_PIN_6
#define S1_MUX_GPIO_Port GPIOB
#define S0_MUX_Pin GPIO_PIN_7
#define S0_MUX_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define TRUE 						1u
#define FALSE 						0u
#define HIGH						1u
#define LOW							0u

#define ACTIVE_HIGH					1u
#define ACTIVE_LOW					0u

#define INDICATOR_ON_TIME			750
#define INDICATOR_OFF_TIME			1000

#define ADDR_FLASH_PAGE_248    		((uint32_t)0x083F0000) /* Base @ of Page 16, 8 Kbytes */
#define ADDR_FLASH_PAGE_255   		((uint32_t)0x083FC000) /* Base @ of Page 127, 8 Kbytes */
#define INTLAL_VALID_DATA_END_ADD   ((uint32_t)0x083FFFF0) /* Base @ of Page 127, 8 Kbytes */

#define FLASH_USER_START_ADDR   		 ADDR_FLASH_PAGE_248   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     		(ADDR_FLASH_PAGE_255 + FLASH_PAGE_SIZE - 1)   /* End @ of user Flash area */

#define EOL_ADDR_FLASH_PAGE_245    		((uint32_t)0x083EA000) /* Base @ of Page 245, 8 Kbytes */
#define EOL_ADDR_FLASH_PAGE_END_245    	((EOL_ADDR_FLASH_PAGE_245 + FLASH_PAGE_SIZE - 1u))	/* Base @ of Page 247, 8 Kbytes */

#define HOUR_ADDR_FLASH_PAGE_247    	((uint32_t)0x083EE000) /* Base @ of Page 247, 8 Kbytes */
#define HOUR_ADDR_FLASH_PAGE_END_247    ((EOL_ADDR_FLASH_PAGE_247 + FLASH_PAGE_SIZE - 1u))	/* Base @ of Page 247, 8 Kbytes */

#define BOOT_ADDR_FLASH_PAGE_246    	((uint32_t)0x083EC000) /* Base @ of Page 246, 8 Kbytes */
#define BOOT_ADDR_FLASH_PAGE_END_246    ((BOOT_ADDR_FLASH_PAGE_246 + FLASH_PAGE_SIZE - 1))	/* Base @ of Page 246, 8 Kbytes */



#define MAXIMUM_NUMBER_OF_DTC		38

#define CAN_PRESCALER_250Kbps			8u
#define CAN_PRESCALER_500Kbps			4u
#define CAN_PRESCALER_1Mbps				2u

typedef enum
{
	eStatusOff = 0u,
	eStatusON,
	eStatusBlink,
	eNoofState
};

typedef enum
{
	e250 = 0U,
	e500,
	e1000,
	eNumOfBauds
} eBAUD_RATE;

typedef enum
{
	eActiveHigh = 0u,
	eActiveLow,
	eTotalType,
};

typedef enum
{
	eBackhoeLoaderKoelEngine74Hp = 0u,
	eBackhoeLoaderEngineTata74Hp,
	eBackhoeLoaderEngineM_M74Hp,
	eBachoeLoaderTotalEngine
};

typedef enum
{
	eSDA_inputMode = 0u,
	eSDA_OutPutMode
}eSDA_PinType;


uint8_t GetRxDLC();
uint32_t GetRx29BitsId();
extern uint8_t RxData[8];
uint16_t u16GetClockTicks(void);
uint32_t u32GetClockTicks(void);
uint8_t GetSelfCheckFlagCompleteStatus(void);
uint8_t bGetIgnWakeupStatus(void);
uint8_t bCheckSumStatus(void);
void TftBrightnessHourmeterDataSave(void);
/* for displaying brightness --------------------------------*/
void set_brightnesScreen(void);
uint8_t Get_brightnesScreen(void);
void SetExitFlagStatus(uint8_t Status);
uint8_t Get_exitFlagStatus(void);
/*FOr ADc value displaying */
void SetAdcValueForDisplay(uint16_t Value);
uint16_t GetAdcValueForDisplay();
void _vProgDependenciesInit(void);
uint8_t bFlashEarseStatus(void);
/*----------------------------------------------------------------*/
void SetNutralGearInputType(uint8_t Value);
void SetParkingBrakeInputType(uint8_t Value);
void SetBaudRateType(uint8_t Value);
void SetEngineSelectionType(uint8_t Value);
//----------------------------------------------------------
void SDA_PinChangeModeType(eSDA_PinType State);
uint16_t GetAdcConversionRank1_Ch16(void);
uint16_t GetAdcConversionRank2_Ch15(void);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
