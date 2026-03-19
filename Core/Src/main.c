/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_touchgfx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MuxDemux.h"
#include "Odo.h"
#include <stdio.h>
#include <stdlib.h>
#include "ledDriver5716.h"
#include "TellTales.h"
#include "TransOilPressreure.h"
#include "FuelGauze.h"
#include "BattVolt.h"
#include "TransOilTemp.h"
#include "AirPressureGauge.h"
#include "J1939.h"
#include "DefGauge.h"
#include "EngineOilPress.h"
#include "EngineCoolTemp.h"
#include "HourMeter.h"
#include "RpmMeter.h"
#include "SootLoad.h"
#include "extEEPROM.h"
#include "Screen.h"
#include "DiscreteSwitch.h"
#include "P15765H.h"
#include "TML_14229_UDSH.h"
#include "POPUPMessages.h"
#include "DieselParticulateFilter.h"
#include "Brightness.h"
#include "adc.h"
#include "Brightness.h"


/*---------------------------------------*/
#define IGN_SENSE_TIME           		50U     			// Ignition sense time 300ms
#define CAN_RECEIVE_NODE				0u
#define REGULATOR_CUTOFF_TIME			5000u				// Regulator disable time 30sec
#define CAN_ID_HOUR_REQUEST 			0x18EA0021
#define CAN_ID_SOOT_LOAD				0x18EA0017

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
FDCAN_TxHeaderTypeDef TxHeader;
FDCAN_RxHeaderTypeDef RxHeader;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

CRC_HandleTypeDef hcrc;

FDCAN_HandleTypeDef hfdcan1;

IWDG_HandleTypeDef hiwdg;

LTDC_HandleTypeDef hltdc;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim15;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_FDCAN1_Init(void);
static void MX_ICACHE_Init(void);
static void MX_IWDG_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM7_Init(void);
static void MX_TIM15_Init(void);
static void MX_CRC_Init(void);
static void MX_SPI1_Init(void);
static void MX_LTDC_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */
//static void PWMSetValueForTFTBrightness(uint16_t Value);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void FDCAN_Configure(void);
static void FD_CAN_Init(void);
void J1939Data();
static void _vClusterWakeUp();
static void _vClusterSleep();
static uint8_t _isIgnitionwhile(void);
static void _vRegulatorPowerControlling(void);
static uint8_t _bCheckIgnitionOn();
static uint8_t _bCheckIgnitionOff();
void PWMSetValueForTFTBrightness(uint16_t Value);
static void MY_Bootloader_Run(void);
static void EcuReset(void);


/*****************************************************************/


static uint32_t u32RegulatorCutoffTimeStamp = 0;		// Regulator disable timer
uint8_t RxData[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint32_t gu32Clock1msTicker = 0;
uint8_t SelfcheckCompleteFlag = TRUE;
static uint8_t gbIgnOff = TRUE;							// flag set true on wakeup to force MCU reset and false on sleep mode to move 															// stepper pointer back to home position
static uint8_t bWakeupIgnition = TRUE;					// set flag in sleep mode
static uint16_t TempAdcValue = 0;
static uint8_t bProgDependenciesFlag = 0;
static uint8_t exit_status_flag = FALSE;


typedef struct
{
	uint32_t   u32IgnTimeStamp;   		//Time stamp for sense Ignition
	uint8_t    bIsIgnON; 		// flag hold the status of ignition set true if it present continious for 300ms otherwise false
} sIgnition;
sIgnition sIgn;

eBAUD_RATE eBaudRateIndex = e250;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */


  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_FDCAN1_Init();
  MX_ICACHE_Init();
  MX_IWDG_Init();
  MX_TIM4_Init();
  MX_TIM7_Init();
  MX_TIM15_Init();
  MX_CRC_Init();
  MX_SPI1_Init();
  MX_LTDC_Init();
  MX_TIM1_Init();
  MX_TIM6_Init();
  MX_TouchGFX_Init();
  /* USER CODE BEGIN 2 */
  __enable_irq();
  HAL_TIM_Base_Start_IT(&htim7);

//  HAL_TIM_Base_Start_IT(&htim7);
//  HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_1);
  sIgn.bIsIgnON = _isIgnitionwhile();
  /* Disable instruction cache prior to internal cacheable memory update */


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

  //MX_TouchGFX_Process();
    /* USER CODE BEGIN 3 */
	  if(sIgn.bIsIgnON)
	  {
	  		// check ignition status for ignition off
	  		u32RegulatorCutoffTimeStamp = u32GetClockTicks();      // Delay clear at every ign cycle
	  		sIgn.bIsIgnON = _bCheckIgnitionOff();
	  		// cluster wakeup if it comes from sleep mode
	  		if(bWakeupIgnition)
	  		{
	  			_vClusterWakeUp();
	  		}
	  		HAL_IWDG_Refresh(&hiwdg); // 1.5 sec WDT time
	  		vControlTellTales();
	  		MX_TouchGFX_Process();
	  		fuel_gauge_Process();
	  		vTransOIL_TempAnlaogProcess();
	  		vJ1939_ServiceManager();
	  		vEngineOilLevelGauge();
	  		vEngineCoolTempLevelGauge();
	  		vHourMeterCAN();
	  		vRpmDataProcessing();
	  		vSootLoadDataProcessing();
	  		vBattVoltDataProcessing();
	  		CAN_Tx_SendMsg();
	  		vBuzzerControl();
	  		ScreensProcessed();
	  		EcuReset();
	  		TripA_B_Reset();
	  		_vProgDependenciesInit();
	  	    processDiagnosticsMessages();
	  	    sendDiagnosticsResponse();
	  	    sessionTimeout();
	  	    vSetEMSFaultText();
	  	    vDpfDataProcessing();
	  	    PopUpMessageDisplayTFT();
	  	    TftBrightNessProcess();
	  	    TripHourAndTftBrightNessSave();
	  	}
	  	else
	  	{
	  		 /* ignition off put cluster in sleep mode */
	  		_vClusterSleep();
	  		_vRegulatorPowerControlling();
	  	}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_MSIK;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIDiv = RCC_LSI_DIV1;
  RCC_OscInitStruct.MSIKClockRange = RCC_MSIKRANGE_4;
  RCC_OscInitStruct.MSIKState = RCC_MSIK_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV1;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 1;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLLVCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_14B;
  hadc1.Init.GainCompensation = 0;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 2;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_16;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_5CYCLE;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_15;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  sConfig.SamplingTime = ADC_SAMPLETIME_391CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief FDCAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = ENABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 8;
  hfdcan1.Init.NominalSyncJumpWidth = 1;
  hfdcan1.Init.NominalTimeSeg1 = 4;
  hfdcan1.Init.NominalTimeSeg2 = 3;
  hfdcan1.Init.DataPrescaler = 8;
  hfdcan1.Init.DataSyncJumpWidth = 1;
  hfdcan1.Init.DataTimeSeg1 = 2;
  hfdcan1.Init.DataTimeSeg2 = 1;
  hfdcan1.Init.StdFiltersNbr = 0;
  hfdcan1.Init.ExtFiltersNbr = 8;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */

  /* USER CODE END FDCAN1_Init 2 */

}

/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */

  /** Enable instruction cache in 1-way (direct mapped cache)
  */
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_16;
  hiwdg.Init.Window = 4095;
  hiwdg.Init.Reload = 3999;
  hiwdg.Init.EWI = 0;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */
static void MX_LTDC_Init(void)
{

  /* USER CODE BEGIN LTDC_Init 0 */

  /* USER CODE END LTDC_Init 0 */

  LTDC_LayerCfgTypeDef pLayerCfg = {0};

  /* USER CODE BEGIN LTDC_Init 1 */

  /* USER CODE END LTDC_Init 1 */
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 7;
  hltdc.Init.VerticalSync = 3;
  hltdc.Init.AccumulatedHBP = 14;
  hltdc.Init.AccumulatedVBP = 5;
  hltdc.Init.AccumulatedActiveW = 334;
  hltdc.Init.AccumulatedActiveH = 245;
  hltdc.Init.TotalWidth = 340;
  hltdc.Init.TotalHeigh = 247;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 320;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 240;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = 0;
  pLayerCfg.ImageWidth = 320;
  pLayerCfg.ImageHeight = 240;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LTDC_Init 2 */

  /* USER CODE END LTDC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  SPI_AutonomousModeConfTypeDef HAL_SPI_AutonomousMode_Cfg_Struct = {0};

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_1LINE;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x7;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  hspi1.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
  hspi1.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerState = SPI_AUTO_MODE_DISABLE;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerSelection = SPI_GRP1_GPDMA_CH0_TCF_TRG;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerPolarity = SPI_TRIG_POLARITY_RISING;
  if (HAL_SPIEx_SetConfigAutonomousMode(&hspi1, &HAL_SPI_AutonomousMode_Cfg_Struct) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 53;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 250;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 4294967295;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 64000;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 64000;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 1;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief TIM15 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM15_Init(void)
{

  /* USER CODE BEGIN TIM15_Init 0 */

  /* USER CODE END TIM15_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM15_Init 1 */

  /* USER CODE END TIM15_Init 1 */
  htim15.Instance = TIM15;
  htim15.Init.Prescaler = 40;
  htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim15.Init.Period = 625;
  htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim15.Init.RepetitionCounter = 0;
  htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim15, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim15, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim15, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM15_Init 2 */

  /* USER CODE END TIM15_Init 2 */
  HAL_TIM_MspPostInit(&htim15);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_DRIVER_G1_Pin|HIGH_OUTPUT__IN_Pin|HIGH_OUTPUT_FAULT_Pin|LOW_OUT_PUT_ENABLE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LED_DRIVER_LATCH_Pin|LED_DRIVER_CLR_Pin|LED_DRIVER_G2_Pin|S2_Pin
                          |S1_Pin|S0_Pin|MUX_Z3_Write_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, I2C_SCL_PIN_Pin|Z0_AnalogMuxWritePIN_Pin|CAN_5V_ENABLE_Pin|TRANS_OIL_TEMP_ENABLE_Pin
                          |LDO_ENABLE_PWR_Pin|S2_MUX_Pin|S1_MUX_Pin|S0_MUX_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, I2C_SDA_PIN_Pin|S3_MUX_Pin|LCD_RESET_Pin_Pin|CAN_STB_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : MOD_BUTTON_SW2_Pin SET_BUTTON_SW1_Pin */
  GPIO_InitStruct.Pin = MOD_BUTTON_SW2_Pin|SET_BUTTON_SW1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_DRIVER_G1_Pin HIGH_OUTPUT__IN_Pin HIGH_OUTPUT_FAULT_Pin LOW_OUT_PUT_ENABLE_Pin */
  GPIO_InitStruct.Pin = LED_DRIVER_G1_Pin|HIGH_OUTPUT__IN_Pin|HIGH_OUTPUT_FAULT_Pin|LOW_OUT_PUT_ENABLE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_DRIVER_LATCH_Pin LED_DRIVER_CLR_Pin LED_DRIVER_G2_Pin S2_Pin
                           S1_Pin S0_Pin */
  GPIO_InitStruct.Pin = LED_DRIVER_LATCH_Pin|LED_DRIVER_CLR_Pin|LED_DRIVER_G2_Pin|S2_Pin
                          |S1_Pin|S0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : I2C_SCL_PIN_Pin Z0_AnalogMuxWritePIN_Pin CAN_5V_ENABLE_Pin TRANS_OIL_TEMP_ENABLE_Pin
                           LDO_ENABLE_PWR_Pin S2_MUX_Pin S1_MUX_Pin S0_MUX_Pin */
  GPIO_InitStruct.Pin = I2C_SCL_PIN_Pin|Z0_AnalogMuxWritePIN_Pin|CAN_5V_ENABLE_Pin|TRANS_OIL_TEMP_ENABLE_Pin
                          |LDO_ENABLE_PWR_Pin|S2_MUX_Pin|S1_MUX_Pin|S0_MUX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : I2C_SDA_PIN_Pin S3_MUX_Pin LCD_RESET_Pin_Pin CAN_STB_Pin */
  GPIO_InitStruct.Pin = I2C_SDA_PIN_Pin|S3_MUX_Pin|LCD_RESET_Pin_Pin|CAN_STB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : MUX_Z3_Write_Pin */
  GPIO_InitStruct.Pin = MUX_Z3_Write_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MUX_Z3_Write_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Z2_MUX_INPUT_READ_Pin Z4_MUX_INPUT_Pin */
  GPIO_InitStruct.Pin = Z2_MUX_INPUT_READ_Pin|Z4_MUX_INPUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : IGN_SENSE_Pin */
  GPIO_InitStruct.Pin = IGN_SENSE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IGN_SENSE_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

static void FD_CAN_Init(void)
{
	hfdcan1.Instance = FDCAN1;
	hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
	hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
	hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
	hfdcan1.Init.AutoRetransmission = ENABLE;
	hfdcan1.Init.TransmitPause = DISABLE;
	hfdcan1.Init.ProtocolException = DISABLE;

	if(GetBaudRateType() == e250)
	{
		hfdcan1.Init.NominalPrescaler = CAN_PRESCALER_250Kbps;
	}
	else if(GetBaudRateType() == e500)
	{
		hfdcan1.Init.NominalPrescaler = CAN_PRESCALER_500Kbps;
	}
	else if(GetBaudRateType() == e1000)
	{
		hfdcan1.Init.NominalPrescaler = CAN_PRESCALER_1Mbps;
	}
	else
	{
		hfdcan1.Init.NominalPrescaler = CAN_PRESCALER_250Kbps;
	}


	hfdcan1.Init.NominalSyncJumpWidth = 1;
	hfdcan1.Init.NominalTimeSeg1 = 4;
	hfdcan1.Init.NominalTimeSeg2 = 3;
	hfdcan1.Init.DataPrescaler = 8;
	hfdcan1.Init.DataSyncJumpWidth = 1;
	hfdcan1.Init.DataTimeSeg1 = 2;
	hfdcan1.Init.DataTimeSeg2 = 1;
	hfdcan1.Init.StdFiltersNbr = 0;
	hfdcan1.Init.ExtFiltersNbr = 8;
	hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
	{
		Error_Handler();
	}
}

/*********************************************************************
 *
 * Checking programming depedencies return all pointer to home ,Stop buzzer and lamps
 * then after 1 sec jump in bootloader code
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/

uint8_t bCheckDepenDencies(void)
{
    return (bProgDependenciesFlag);
}

void _vProgDependenciesInit(void)
{
    static uint8_t firstEntryFlag = TRUE;
    static uint16_t timestamp = 0U;
    uint16_t temp = FALSE;

    temp = u16GetRoutineControlServiceStatus();

    if(temp == CHECK_PROG_DEP)
    {
		sendDiagnosticsResponse();
		bProgDependenciesFlag = TRUE;
		RoutineService();
		vSaveBootloaderState(eBootloaderMode);
		SaveBootIdentifireParameter();
		MY_Bootloader_Run();
    }

}

/***********************************************************************
 *
 * Flash Erase Status (Dummy function for keeping UDS service code same in Application & Bootloader)
 *
 * @param      None
 *
 * @return     None
 *************************************************************************/
uint8_t bFlashEarseStatus(void)
{
    return(TRUE);
}
uint8_t GetControllerMode(void)
{
    return(eApplicationMode);
}

static void MY_Bootloader_Run(void)
{
    while(1);
}

void FDCAN_Configure(void)
{
    if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK) {
        Error_Handler();
    }


	FDCAN_FilterTypeDef sFilterConfig;
	// ----------------- Filter 0 -------------------
	// Group IDs: 0x18FEE500, 0x18FEEE00, 18FEEF00,0x18FEF100
	sFilterConfig.IdType = FDCAN_EXTENDED_ID;
	sFilterConfig.FilterIndex = 0u;
	sFilterConfig.FilterType = FDCAN_FILTER_RANGE;
	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	sFilterConfig.FilterID1 = 0x18FEE500;
	sFilterConfig.FilterID2 = 0x18FEF100; // Mask: ignore lower 12 bits (covers CA00, E500, F700, etc.)
	if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK) {
		Error_Handler();
	}

	// ----------------- Filter 1 -------------------
	// Exact match for 0x18FEF700 , 0x18FEFF00
	sFilterConfig.IdType = FDCAN_EXTENDED_ID;
	sFilterConfig.FilterIndex = 1u;
	sFilterConfig.FilterType = FDCAN_FILTER_DUAL;
	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;
	sFilterConfig.FilterID1 = 0x18FEF700;
	sFilterConfig.FilterID2 = 0x18FEFF00; // Exact match
	if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK) {
		Error_Handler();
	}

	// ----------------- Filter 2 -------------------
	// Group IDs: 0x18FD7B00 , 0x18FD7C00
	sFilterConfig.IdType = FDCAN_EXTENDED_ID;
	sFilterConfig.FilterIndex = 2u;
	sFilterConfig.FilterType = FDCAN_FILTER_DUAL;
	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;
	sFilterConfig.FilterID1 = 0x18FD7B00;
	sFilterConfig.FilterID2 = 0x18FD7C00; // Mask lower 12 bits
	if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK) {
		Error_Handler();
	}

	// ----------------- Filter 3 -------------------
	// Exact match for 0x18FDC600
	sFilterConfig.IdType = FDCAN_EXTENDED_ID;
	sFilterConfig.FilterIndex = 3u;
	sFilterConfig.FilterType = FDCAN_FILTER_DUAL;
	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;
	sFilterConfig.FilterID1 = 0x18FD0700;
	sFilterConfig.FilterID2 = 0x18FF6700; // Exact match
	if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK) {
		Error_Handler();
	}

	//  0x0CF00400,18FD1C00
	sFilterConfig.IdType = FDCAN_EXTENDED_ID;
	sFilterConfig.FilterIndex = 4u;
	sFilterConfig.FilterType = FDCAN_FILTER_DUAL;
	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;
	sFilterConfig.FilterID1 = 0x18FD1C00;
	sFilterConfig.FilterID2 = 0x1BDA17F9;
	if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK) { Error_Handler(); }

	// 0x18EBFF00 , 0x18ECFF00
	sFilterConfig.IdType = FDCAN_EXTENDED_ID;
	sFilterConfig.FilterIndex = 5u;
	sFilterConfig.FilterType = FDCAN_FILTER_DUAL;
	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	sFilterConfig.FilterID1 = 0x18EBFF00;
	sFilterConfig.FilterID2 = 0x18ECFF00;
	if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK) { Error_Handler(); }

	// 0x18FE5600 , 0x18FECA00 , 0x18FDC600
	sFilterConfig.IdType = FDCAN_EXTENDED_ID;
	sFilterConfig.FilterIndex = 6u;
	sFilterConfig.FilterType = FDCAN_FILTER_DUAL;
	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	sFilterConfig.FilterID1 = 0x18FDC600;
	sFilterConfig.FilterID2 = 0x18FECA00;
	if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK) { Error_Handler(); }

	// 0x18FE5600 , 0x0CF00400
	sFilterConfig.IdType = FDCAN_EXTENDED_ID;
	sFilterConfig.FilterIndex = 7u;
	sFilterConfig.FilterType = FDCAN_FILTER_DUAL;
	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	sFilterConfig.FilterID1 = 0x0CF00400;
	sFilterConfig.FilterID2 = 0x18FE5600;
	if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK) { Error_Handler(); }

    // ----------------- Global Filter -------------------
    HAL_FDCAN_ConfigGlobalFilter(&hfdcan1,
							  FDCAN_REJECT, //Reject non matching Std ID
							  FDCAN_REJECT, //Reject non matching Ext ID
							  FDCAN_REJECT_REMOTE, //Reject Remote STD
							  FDCAN_REJECT_REMOTE); //Reject Remote EXT

    // ----------------- Tx Header -------------------
    TxHeader.IdType = FDCAN_EXTENDED_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = FDCAN_DLC_BYTES_8;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    // ----------------- Start FDCAN -------------------
    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK) {	Error_Handler();}

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(htim);
	if (htim->Instance == TIM7)
	{
		gu32Clock1msTicker++;
	}
	else if(htim->Instance == TIM6)
	{
		MuxDmuxProcessing();
		AdcDataProcessing();
	}

}



void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{

}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
	{
		if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &RxHeader, (uint8_t *)&RxData) != HAL_OK)
		{
			Error_Handler();
		}
		vJ1939OnFullRx();
	}
}

void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
	if((RxFifo1ITs & FDCAN_IT_RX_FIFO1_NEW_MESSAGE) != RESET)
	{
		if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO1, &RxHeader, (uint8_t *)&RxData) != HAL_OK)
		{
			Error_Handler();
		}

		vJ1939OnFullRx();
	}
}



/* This is use for bus recovery --------*/
void  HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs)
{
  if((ErrorStatusITs & FDCAN_IE_BOE) != 0)  /* If Bus-Off error occured */
  {
     hfdcan->Instance->CCCR &= ~FDCAN_CCCR_INIT; /* Recover from Bus-Off */
  }
}


/*****************************************************************************
*Checks the status of ignition for 100ms .
* @param	  None
*
* @return	  None


*****************************************************************************/

static uint8_t _isIgnitionwhile(void)
{
	uint8_t status;
	uint32_t clk;

	status = FALSE;

    clk = u32GetClockTicks();
    if(HAL_GPIO_ReadPin(GPIOD, IGN_SENSE_Pin) == HIGH)
	while(u32GetClockTicks() - clk < 100)
		HAL_IWDG_Refresh(&hiwdg);


	if( HAL_GPIO_ReadPin(GPIOD, IGN_SENSE_Pin) == HIGH )
	{
		status = TRUE;
	}
	else
	{
		status = FALSE;
	}
	return (status);
}

/*****************************************************************************
*Check Ignition status with debounce time if ignition singal have glitch then debounce time reset
Call it in main loop for checking ignition off.

* @param	  None
*
* @return	  None

*****************************************************************************/

uint8_t _bCheckIgnitionOff()
{
	uint8_t status = TRUE;

	if(HAL_GPIO_ReadPin(GPIOD, IGN_SENSE_Pin) == LOW)
	{
		if( u32GetClockTicks() - sIgn.u32IgnTimeStamp  >= IGN_SENSE_TIME)
		status = FALSE;
	}
	else
	{
		sIgn.u32IgnTimeStamp = u32GetClockTicks();
	}
	return(status);
}


/*****************************************************************************
*Check Ignition status with debounce time if ignition singal have glitch then debounce time reset
Call it in sleep mode for checking ignition on.

* @param	  None
*
* @return	  None

*****************************************************************************/

uint8_t _bCheckIgnitionOn()
{
	uint8_t status = FALSE;

	if(HAL_GPIO_ReadPin(GPIOD, IGN_SENSE_Pin) == HIGH)
	{
		if( u32GetClockTicks() - sIgn.u32IgnTimeStamp  >= IGN_SENSE_TIME)
		{
			status = TRUE;
		}
	}
	else
	{
		sIgn.u32IgnTimeStamp = u32GetClockTicks();
	}
	return(status);
}

/*****************************************************************************
* Cluster wakeup on Ignition Enable periphrals.
* @param	  None
*
* @return	  None


*****************************************************************************/

static void _vClusterWakeUp()
{
	static uint32_t temp_var = 0u;
	static uint32_t BuzzerTemp_var = 0u;

 	//if it comes from sleep mode force MCU reset //
	if(!gbIgnOff)
	{
		gbIgnOff = TRUE;							// set flag true to bypass force MCU reset
		NVIC_SystemReset();
	}

	if (HAL_ICACHE_Disable() != HAL_OK)
	{
		Error_Handler();
	}

	HAL_GPIO_WritePin(GPIOA,HIGH_OUTPUT__IN_Pin, GPIO_PIN_SET); // High out put PIn8
	HAL_GPIO_WritePin(GPIOA, HIGH_OUTPUT_FAULT_Pin, GPIO_PIN_SET);  // High Always
	HAL_GPIO_WritePin(GPIOD, CAN_STB_Pin, GPIO_PIN_RESET);	//CAN STB PIN Enable

	ReadOdoDataAndFractionValue();	/* Read odo and trip data read */
	ReadHourDataSystemParameter();	/* Read hour Parameter read */
	GetEOl_ParemetersRead();		/* EOL Parameter read */

	FD_CAN_Init();			/*  */

	HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_1);
	PWMSetValueForTFTBrightness(100);

	HAL_GPIO_WritePin(GPIOB,TRANS_OIL_TEMP_ENABLE_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB,LDO_ENABLE_PWR_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(CAN_5V_ENABLE_GPIO_Port, CAN_5V_ENABLE_Pin, GPIO_PIN_RESET);	//We are using this pin trans oil temp enable/disable gain
	HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1);
	HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0u);
	HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
	HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_BUS_OFF, 0u);						// Bus off recovery interrupt initilize
	FDCAN_Configure();
	vJ1939Init();
	diagnosticDataInit();
	vBuzzerEnable();
	/*----------------------------------------------------------------------------------------*/
	HAL_GPIO_WritePin(LCD_RESET_Pin_GPIO_Port,LCD_RESET_Pin_Pin, GPIO_PIN_SET);			//Enabling LCD Backlight
	vCheckLamps();
	vSetBuzzerSoundType(CONTINUE_ALARM);
	vBuzzerControl();
	vLedDriverInit();
	vSetLedDriverBlankHigh();
	temp_var = u32GetClockTicks();
	HourRequestMessageOneTimeSend();

	while(u32GetClockTicks() - temp_var < 2020u)	/* Delay < 4 seconds for self check*/
	{
		HAL_IWDG_Refresh(&hiwdg);
		if(u32GetClockTicks() - BuzzerTemp_var > 1200u)
		{
			_vStopBuzzer();
			vSetBuzzerSoundType(BUZZER_STOP);
			BuzzerTemp_var = u32GetClockTicks();
		}
		MuxDmuxProcessing();
		AdcDataProcessing();
		MX_TouchGFX_Process();
		vJ1939_ServiceManager();
		vMuxLedPrevStateInit();
		vHourMeterCAN();
	}
	HAL_TIM_Base_Start_IT(&htim6);
	ReadTftBrightnessValue();		/* Read Tft brightness data read */
	HourRequestMessageOneTimeSend();
	HAL_TIM_IC_Start_IT(&htim4 , TIM_CHANNEL_4); // PD5 pin
	SelfcheckCompleteFlag = FALSE;
	bWakeupIgnition = FALSE;					// set flag to avoid wakeup cluster

}

/*****************************************************************************

*Disable Peripherals on Ignition OFF.
* @param	  None
*
* @return	  None


*****************************************************************************/
static void _vClusterSleep()
{
	static uint8_t FistEntry = TRUE;

	if(!bWakeupIgnition)
	{

	}

	if(FistEntry)
	{
		//SaveFractionDigit();
		FistEntry = FALSE;
	}

	asm("NOP");asm("NOP");
	HAL_GPIO_WritePin(GPIOD, CAN_STB_Pin, GPIO_PIN_SET);	//CAN STB PIN Disable
	PWMSetValueForTFTBrightness(0);	// PWM OFF Immideate
	HAL_GPIO_WritePin(LCD_RESET_Pin_GPIO_Port,LCD_RESET_Pin_Pin, GPIO_PIN_RESET);			//RESET LCD
	vAllLampsOff();
	_vStopBuzzer();
	gbIgnOff = FALSE;
	vSetLedDriverBlankLow();
	HAL_GPIO_WritePin(CAN_5V_ENABLE_GPIO_Port, CAN_5V_ENABLE_Pin, GPIO_PIN_RESET);	//Enabling CAN PWR
   // set flag true to perform cluster on ignition on again
	bWakeupIgnition = TRUE;
	//check ingnition
	sIgn.bIsIgnON = _bCheckIgnitionOn();

}


/*****************************************************************************

 * Regulator out put power enable/disable by MCU pin

 * @param	   None

 * @return	   None

 *****************************************************************************/
static void _vRegulatorPowerControlling(void)
{
	if(sIgn.bIsIgnON == FALSE)			// Ign off check
	{
		if(u32GetClockTicks() - u32RegulatorCutoffTimeStamp >= REGULATOR_CUTOFF_TIME)
		{
			u32RegulatorCutoffTimeStamp = u32GetClockTicks();
			HAL_GPIO_WritePin(GPIOB,LDO_ENABLE_PWR_Pin, GPIO_PIN_RESET); // MCU power cutoff
		}
	}

}

static void EcuReset(void)
{
	if(bIsECUResetByService() == TRUE)
	{
		NVIC_SystemReset();
	}

}

void PWMSetValueForTFTBrightness(uint16_t Value)
{
	  uint32_t pulse = 0;

	  pulse = (htim1.Init.Period * Value) / 100u;
	   __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1, pulse);

}

void SetExitFlagStatus(uint8_t Status)
{
	exit_status_flag = Status;
}

 uint8_t Get_exitFlagStatus()
 {
 	return exit_status_flag;
 }

 uint32_t GetRx29BitsId()
 {
 	return RxHeader.Identifier;
 }

 uint8_t GetRxDLC()
 {
 	return RxHeader.DataLength;
 }

 uint8_t GetSelfCheckFlagCompleteStatus(void)
 {
 	return SelfcheckCompleteFlag;
 }

 /* *****************************************************************************/
 uint8_t bGetIgnWakeupStatus(void)
 {
 	return bWakeupIgnition;
 }


 /*********************************************************************//**
  *
  * Return flash writing checksum status
  *(Dummy function for keeping UDS service code same in Application & Bootloader)
  * @param      None
  *
  * @return     None
  *************************************************************************/

 uint8_t bCheckSumStatus(void)
 {
  return(TRUE);
 }

 /*********************************************************************//**
  *
  * Set CAN BaudRate
  *
  *
  * @param      None
  *
  * @return     None
  *************************************************************************/

 void vSetCanBaudRate(eBAUD_RATE BaudRate)
 {
 	eBaudRateIndex = BaudRate;
 }


 /*********************************************************************//**
  *
  * Get CAN baud Rate
  *
  *
  * @param      None
  *
  * @return     Pulse Value
  *************************************************************************/

 eBAUD_RATE eGetCanBaudRate()
 {
 	return eBaudRateIndex;
 }


 /*********************************************************************//**
  *
  * 1ms System clock
  *
  * @param      None
  *
  * @return     system clock value in u32
  *************************************************************************/
 uint32_t u32GetClockTicks(void)
 {
 	return gu32Clock1msTicker;
 }


 void SetAdcValueForDisplay(uint16_t Value)
 {
 	TempAdcValue = Value;
 }

 uint16_t GetAdcValueForDisplay()
 {
 	return TempAdcValue;
 }
 /*********************************************************************//**
  *
  * 1ms System clock
  *
  * @param      None
  *
  * @return     system clock value in u16
  *************************************************************************/
 uint16_t u16GetClockTicks(void)
 {
 	return (uint16_t)gu32Clock1msTicker;
 }

 /*********************************************************************//**
  *
  * SDA_PinChangeModeType
  *
  * @param      None
  *
  * @return     state
  *************************************************************************/
 void SDA_PinChangeModeType(eSDA_PinType State)
 {
 	GPIO_InitTypeDef GPIO_InitStruct = {0};
 	/*Configure GPIO pins : I2C_SDA_PIN_Pin S3_MUX_Pin LCD_RESET_Pin_Pin CAN_STB_Pin */
 	GPIO_InitStruct.Pin = I2C_SDA_PIN_Pin;

 	if(State == eSDA_inputMode)
 	{
 		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
 	}
 	else
 	{
 		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 	}

 	GPIO_InitStruct.Pull = GPIO_NOPULL;
 	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
 }

 uint16_t GetAdcConversionRank1_Ch16(void)
 {
 	uint16_t ADC_value = 0u;
 	static uint8_t FirstEntryFlag = TRUE;

 	ADC_ChannelConfTypeDef sConfig = {0};

 	/** Configure Regular Channel
 	*/
 	sConfig.Channel = ADC_CHANNEL_16;
 	sConfig.Rank = ADC_REGULAR_RANK_1;
 	sConfig.SamplingTime = ADC_SAMPLETIME_6CYCLES;
 	sConfig.SingleDiff = ADC_SINGLE_ENDED;
 	sConfig.OffsetNumber = ADC_OFFSET_NONE;
 	sConfig.Offset = 0;
 	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
 	{
 	  Error_Handler();
 	}

 	HAL_ADC_Start(&hadc1);
 	HAL_ADC_PollForConversion(&hadc1, 50);
 	ADC_value = HAL_ADC_GetValue(&hadc1);
 	HAL_ADC_Stop(&hadc1);

 	return ADC_value;
 }

 uint16_t GetAdcConversionRank2_Ch15(void)
 {
 	uint16_t ADC_value = 0u;
 	ADC_ChannelConfTypeDef sConfig = {0};

 	/** Configure Regular Channel
 	*/
 	sConfig.Channel = ADC_CHANNEL_15;
 	sConfig.Rank = ADC_REGULAR_RANK_1;
 	sConfig.SamplingTime = ADC_SAMPLETIME_6CYCLES;
 	sConfig.SingleDiff = ADC_SINGLE_ENDED;
 	sConfig.OffsetNumber = ADC_OFFSET_NONE;
 	sConfig.Offset = 0;
 	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
 	{
 		Error_Handler();
 	}
 	HAL_ADC_Start(&hadc1);
 	HAL_ADC_PollForConversion(&hadc1, 50);
 	ADC_value = HAL_ADC_GetValue(&hadc1);
 	HAL_ADC_Stop(&hadc1);

 	return ADC_value;
 }

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
