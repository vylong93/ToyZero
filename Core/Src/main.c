/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "music_player.h"
#include "leds_driver.h"
#include "sevenseg_driver.h"
#include "analog_module.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum game_state {
  TEST_MODE = 0,
  START_GAME,
  PLAYING,
  GAME_OVER
} game_state_t;

typedef enum game_level {
  EASY = 0,
  NORMAL,
  HARD
} game_level_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */
volatile game_state_t g_state = TEST_MODE;
volatile int g_newStateUpdate = 0;
volatile game_level_t g_level = EASY;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM4_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void blinkLedOnBoard()
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_Delay(999);
}
void renderBatteryPercentage(void) {
  uint32_t battPercent = getBatteryPercentage();
  setDisplayNumber(battPercent);
  if (battPercent > 57) {  /* Battery High - GREEN */
    set_rgb(0, 255, 0);
    for (int i = 0; i < 2; i++) {
      turn_on_all_leds();
      HAL_Delay(500);
      turn_off_all_leds();
      HAL_Delay(500);
    }
  } else if (battPercent > 28) {
    set_rgb(100, 100, 0); /* Battery Medium - GREEN */
    for (int i = 0; i < 4; i++) {
      turn_led_on(led1);
      turn_led_on(led2);
      turn_led_on(led3);
      turn_led_on(led4);
      HAL_Delay(200);
      turn_led_off(led1);
      turn_led_off(led2);
      turn_led_off(led3);
      turn_led_off(led4);
      HAL_Delay(200);
    }
  } else {
    set_rgb(50, 0, 0); /* Battery Low - GREEN */
    for (int i = 0; i < 10; i++) {
      turn_led_on(led1);
      turn_led_on(led2);
      HAL_Delay(100);
      turn_led_off(led1);
      turn_led_off(led2);
      HAL_Delay(100);
    }
  }
  turnOffDisplay();
}
void _test_all_in_one (unsigned char r, unsigned char g, unsigned char b,
  led_rgb_t led, note_t note, unsigned int duration_ticks,
  unsigned char no_sound, uint32_t display_value) {
  set_rgb(r, g, b);
  setDisplayNumber(display_value);
  turn_led_on(led);
  if (no_sound) {
    HAL_Delay(200); /* BPM 150 */
  } else {
    play(note, duration_ticks);
  }
  turn_led_off(led);
}
void runningTestMode(void) {
  uint32_t duration_ticks = 2;
  unsigned char no_sound = 0;
  setDisplayNumber(0);
  turnOnDisplay();
  set_bpm(150);

  /* Up */
  _test_all_in_one(255, 0, 0, led1, C4, duration_ticks, no_sound, 1);
  _test_all_in_one(255, 100, 0, led2, D4, duration_ticks, no_sound, 2);
  _test_all_in_one(255, 255, 0, led3, E4, duration_ticks, no_sound, 3);
  _test_all_in_one(0, 255, 0, led4, F4, duration_ticks, no_sound, 4);
  _test_all_in_one(0, 0, 255, led5, G4, duration_ticks, no_sound, 5);
  _test_all_in_one(55, 0, 255, led6, A4, duration_ticks, no_sound, 6);
  _test_all_in_one(255, 0, 255, led7, B4, duration_ticks, no_sound, 7);
  /* Down */
  _test_all_in_one(55, 0, 255, led6, A4, duration_ticks, no_sound, 6);
  _test_all_in_one(0, 0, 255, led5, G4, duration_ticks, no_sound, 5);
  _test_all_in_one(0, 255, 0, led4, F4, duration_ticks, no_sound, 4);
  _test_all_in_one(255, 255, 0, led3, E4, duration_ticks, no_sound, 3);
  _test_all_in_one(255, 100, 0, led2, D4, duration_ticks, no_sound, 2);
  _test_all_in_one(255, 0, 0, led1, C4, duration_ticks, no_sound, 1);

  setDisplayIdle();
  turn_on_all_leds();
  while (!g_newStateUpdate) {
    for (int k = 0; (g_newStateUpdate == 0) && (k < 360); k++)
    {
      set_rgb_sineLED(k);
      HAL_Delay(30);
    }
  }

  /* Clean up */
  g_newStateUpdate = 0;
  turn_off_all_leds();
  turnOffDisplay();
}
void runningStartingState(void) {
  turnOnDisplay();
  setDisplayText(DIGIT_O, DIGIT_O, DIGIT_O, DIGIT_O);
  beep();

  while (!g_newStateUpdate) {
    /* EASY */
    set_rgb(255, 50, 0);
    turn_led_on(led2);
    HAL_Delay(5);
    turn_off_all_leds();

    /* NORMAl */
    set_rgb(0, 150, 150);
    turn_led_on(led4);
    HAL_Delay(5);
    turn_off_all_leds();

    /* HARD */
    set_rgb(152, 0, 152);
    turn_led_on(led6);
    HAL_Delay(5);
    turn_off_all_leds();
  }

  /* Clean up */
  g_newStateUpdate = 0;
  turn_off_all_leds();
  turnOffDisplay();
}
void runningPlayState (void) {
  song_t targetSong = SUPER_MARIO;
  int remainLive = 0;

  turnOnDisplay();

  /* Preparing level */
  switch (g_level) {
    case EASY:
      setDisplayText(DIGIT_E, DIGIT_DASH, DIGIT_NONE, DIGIT_NONE);
      remainLive = 5;
      targetSong = SUPER_MARIO;
      audio_transition_starting_to_easy();
    break;

    case NORMAL:
      setDisplayText(DIGIT_N, DIGIT_DASH, DIGIT_DASH, DIGIT_NONE);
      remainLive = 3;
      targetSong = BOBOMB_BATTLEFIELD;
      audio_transition_starting_to_normal();
    break;

    case HARD:
      setDisplayText(DIGIT_H, DIGIT_DASH, DIGIT_DASH, DIGIT_DASH);
      remainLive = 0;
      targetSong = PRINCESS_SLIDE;
      audio_transition_starting_to_hard();
    break;

    default:
    break;
  }

  while (!g_newStateUpdate) {
    HAL_Delay(500);
    play_song(targetSong);
    HAL_Delay(500);

    --remainLive;
    if (remainLive <= 0) {
      g_state = GAME_OVER;
      g_newStateUpdate = 1;
      break;
    }
  }

  /* Clean up */
  g_newStateUpdate = 0;
  turnOffDisplay();
}
void runningGameOverState (void) {
  audio_transition_gameover();

  while (!g_newStateUpdate) {
    turnOnDisplay();
    setDisplayNumber(6969);
  }

  /* Clean up */
  g_newStateUpdate = 0;
  turnOffDisplay();
}

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
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(300); /* For battery power stable */
  init_rgb_driver();
  turnOnDisplay();
  setDisplayIdle();
  renderBatteryPercentage();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    switch (g_state) {
      case TEST_MODE:
        runningTestMode();
      break;

      case START_GAME:
        runningStartingState();
      break;

      case PLAYING:
        runningPlayState();
      break;

      case GAME_OVER:
        runningGameOverState();
      break;

      default:
        blinkLedOnBoard();
      break;
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV16;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV8;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 40-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 256-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1-1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
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
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65536-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 4000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 2-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65536-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
  sConfigOC.Pulse = 1136;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_ONBOARD_GPIO_Port, LED_ONBOARD_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RGB_LED1_Pin|RGB_LED2_Pin|LATCH_7SEG_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, RGB_LED3_Pin|RGB_LED4_Pin|RGB_LED5_Pin|RGB_LED6_Pin
                          |RGB_LED7_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : LED_ONBOARD_Pin */
  GPIO_InitStruct.Pin = LED_ONBOARD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_ONBOARD_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ACT_BUTTON_Pin */
  GPIO_InitStruct.Pin = ACT_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(ACT_BUTTON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RGB_LED1_Pin RGB_LED2_Pin */
  GPIO_InitStruct.Pin = RGB_LED1_Pin|RGB_LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LATCH_7SEG_Pin */
  GPIO_InitStruct.Pin = LATCH_7SEG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(LATCH_7SEG_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RGB_LED3_Pin RGB_LED4_Pin RGB_LED5_Pin RGB_LED6_Pin
                           RGB_LED7_Pin */
  GPIO_InitStruct.Pin = RGB_LED3_Pin|RGB_LED4_Pin|RGB_LED5_Pin|RGB_LED6_Pin
                          |RGB_LED7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTTON7_Pin BUTTON6_Pin BUTTON5_Pin BUTTON4_Pin
                           BUTTON3_Pin BUTTON2_Pin BUTTON1_Pin */
  GPIO_InitStruct.Pin = BUTTON7_Pin|BUTTON6_Pin|BUTTON5_Pin|BUTTON4_Pin
                          |BUTTON3_Pin|BUTTON2_Pin|BUTTON1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
/**
  * @brief  Output Compare callback in non-blocking mode
  * @param  htim TIM OC handle
  * @retval None
  */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (TIM4 == htim->Instance) {
    htim4.Instance->CCR4 += current_note;
  } else if (TIM3 == htim->Instance) {
    htim3.Instance->CCR3 += 8000; /* TODO: 4ms */
    performSevenSegOneStepScan();
  }
}

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch  (GPIO_Pin) {
    case ACT_BUTTON_Pin:
      switch (g_state) {
        case TEST_MODE:
          g_state = START_GAME;
          g_newStateUpdate = 1;
        break;

        case START_GAME:
          g_state = START_GAME;
          g_newStateUpdate = 0;
        break;

        case PLAYING:
          g_state = START_GAME;
          g_newStateUpdate = 1;
        break;

        case GAME_OVER:
          g_state = START_GAME;
          g_newStateUpdate = 1;
        break;

        default:
        break;
      }
    break;

    case BUTTON1_Pin:
      setDisplayNumber(1);
    break;

    case BUTTON2_Pin:
      switch (g_state) {
        case START_GAME:
          g_level = EASY;
          g_state = PLAYING;
          g_newStateUpdate = 1;
        break;

        default:
          setDisplayNumber(2);
        break;
      }
    break;

    case BUTTON3_Pin:
      setDisplayNumber(3);
    break;

    case BUTTON4_Pin:
      switch (g_state) {
        case START_GAME:
          g_level = NORMAL;
          g_state = PLAYING;
          g_newStateUpdate = 1;
        break;

        default:
          setDisplayNumber(4);
        break;
      }
    break;

    case BUTTON5_Pin:
      setDisplayNumber(5);
    break;

    case BUTTON6_Pin:
      switch (g_state) {
        case START_GAME:
          g_level = HARD;
          g_state = PLAYING;
          g_newStateUpdate = 1;
        break;

        default:
          setDisplayNumber(6);
        break;
      }
    break;

    case BUTTON7_Pin:
      setDisplayNumber(7);
    break;

    default:
    break;
  }
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
