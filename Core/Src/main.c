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
  GAME_OVER,
  MUSIC
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
volatile int g_buttonRecord[] = { 0, 0, 0, 0, 0, 0, 0 };
volatile int g_newRecordUpdate = 0;
volatile int g_expectedIndexBuffer[32] = { 0 };
volatile note_t g_playbackNodeBuffer[32] = { 0 };
volatile uint32_t g_currentScore = 0;
volatile uint32_t g_lastHighestScore = 0;
char g_force_stop = 0;
song_t g_target_music = SUPER_MARIO;
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
void setGameState(game_state_t state) {
  game_state_t old_state = g_state;
  g_state = state;
  g_newStateUpdate = (old_state != g_state) ? (1) : (0);
}
void blinkLedOnBoard()
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_Delay(999);
}
void _disableAllInputButtons(void) {
  HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
  HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
}
void _enableAllInputButtons(void) {
  __HAL_GPIO_EXTI_CLEAR_IT(BUTTON1_Pin);
  __HAL_GPIO_EXTI_CLEAR_IT(BUTTON2_Pin);
  __HAL_GPIO_EXTI_CLEAR_IT(BUTTON3_Pin);
  __HAL_GPIO_EXTI_CLEAR_IT(BUTTON4_Pin);
  __HAL_GPIO_EXTI_CLEAR_IT(BUTTON5_Pin);
  __HAL_GPIO_EXTI_CLEAR_IT(BUTTON6_Pin);
  __HAL_GPIO_EXTI_CLEAR_IT(BUTTON7_Pin);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}
void renderBatteryPercentage(void) {
  _disableAllInputButtons();
  uint32_t battPercent = getBatteryPercentage();
  setDisplayNumber(battPercent);
  if (battPercent > 57) {  /* Battery High - GREEN */
    set_rgb(0, 50, 0);
    for (int i = 0; i < 2; i++) {
      turn_on_all_leds();
      HAL_Delay(500);
      turn_off_all_leds();
      HAL_Delay(500);
    }
  } else if (battPercent > 28) {
    set_rgb(50, 50, 0); /* Battery Medium - YELLOW */
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
    set_rgb(50, 0, 0); /* Battery Low - RED */
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
void _test_all_in_one (led_rgb_t led, note_t note, unsigned int duration_ticks,
  unsigned char no_sound, uint32_t display_value) {
  set_rgb_color_base_on_led(led);
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
  uint32_t duration_ticks = 1;
  unsigned char no_sound = 0;
  setDisplayNumber(0);
  turnOnDisplay();
  set_bpm(150);

  /* Up */
  _test_all_in_one(led1, C4, duration_ticks, no_sound, 1);
  _test_all_in_one(led2, D4, duration_ticks, no_sound, 2);
  _test_all_in_one(led3, E4, duration_ticks, no_sound, 3);
  _test_all_in_one(led4, F4, duration_ticks, no_sound, 4);
  _test_all_in_one(led5, G4, duration_ticks, no_sound, 5);
  _test_all_in_one(led6, A4, duration_ticks, no_sound, 6);
  _test_all_in_one(led7, B4, duration_ticks, no_sound, 7);
  /* Down */
  _test_all_in_one(led6, A4, duration_ticks, no_sound, 6);
  _test_all_in_one(led5, G4, duration_ticks, no_sound, 5);
  _test_all_in_one(led4, F4, duration_ticks, no_sound, 4);
  _test_all_in_one(led3, E4, duration_ticks, no_sound, 3);
  _test_all_in_one(led2, D4, duration_ticks, no_sound, 2);
  _test_all_in_one(led1, C4, duration_ticks, no_sound, 1);

  setDisplayIdle();
  turn_on_all_leds();
  _enableAllInputButtons();
  g_newStateUpdate = 0;
  while (!g_newStateUpdate) {
    for (int k = 0; (g_newStateUpdate == 0) && (k < 360); k++)
    {
      set_rgb_sineLED(k);
      HAL_Delay(30);
    }
  }

  /* Clean up */
  _disableAllInputButtons();
  g_newStateUpdate = 0;
  turn_off_all_leds();
  turnOffDisplay();
}
void runningStartingState(void) {
  turnOnDisplay();
  setDisplayNumber(g_lastHighestScore);
  beep();
  _enableAllInputButtons();

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
  _disableAllInputButtons();
  g_newStateUpdate = 0;
  turn_off_all_leds();
  turnOffDisplay();
}
void _clearButtonRecord(void) {
  for (int i = 0; i < 7; i++) {
    g_buttonRecord[i] = 0;
  }
}
void runningPlayState (void) {
  song_t targetSong = SUPER_MARIO;
  signed int remainLive = 0;
  int measure_length = 0;
  measure_t* measures_list = NULL;
  unsigned int current_bpm = 100;

  /* Preparing level */
  _disableAllInputButtons();
  _clearButtonRecord();
  switch (g_level) {
    case EASY:
      setDisplayText(DIGIT_E, DIGIT_DASH, DIGIT_NONE, DIGIT_NONE);
      remainLive = 0;
      targetSong = SUPER_MARIO;
      current_bpm = 60;
      audio_transition_starting_to_easy();
      measures_list = get_mario_measures_list(&measure_length);
    break;

    case NORMAL:
      setDisplayText(DIGIT_N, DIGIT_DASH, DIGIT_DASH, DIGIT_NONE);
      remainLive = 0; // 3
      targetSong = BOBOMB_BATTLEFIELD;
      current_bpm = 100;
      audio_transition_starting_to_normal();
      measures_list = get_mario_measures_list(&measure_length);
    break;

    case HARD:
      setDisplayText(DIGIT_H, DIGIT_DASH, DIGIT_DASH, DIGIT_DASH);
      remainLive = 0;
      targetSong = PRINCESS_SLIDE;
      current_bpm = 120;
      audio_transition_starting_to_hard();
      measures_list = get_mario_measures_list(&measure_length);
    break;

    default:
    break;
  }

  g_currentScore = 0;
  turnOnDisplay();

  while (!g_newStateUpdate) {
    HAL_Delay(500);
    set_bpm(current_bpm);

    uint32_t expectedPointer = 0;
    g_newRecordUpdate = 0;
    for (int i = 0; (i < measure_length) && (remainLive >= 0); i++) {
      expectedPointer = 0;
      for (int j = 0; j < measures_list[i].length; j += 2) { /* maximum lenght is 32 */
        turnOffDisplay();
        if (measures_list[i].data[j] != REST) {
          g_expectedIndexBuffer[expectedPointer] = get_button_index_from_node(measures_list[i].data[j]);
          g_playbackNodeBuffer[expectedPointer] = measures_list[i].data[j];
          ++expectedPointer;
        }
        play_with_led(measures_list[i].data[j], measures_list[i].data[j + 1]);
      }

      for (int j = 0; (j < expectedPointer) && (remainLive >= 0); j++) {
        g_newRecordUpdate = 0;
        _clearButtonRecord();
        setDisplayNumber(g_currentScore);
        _enableAllInputButtons();
        turnOnDisplay();
        while (!g_newRecordUpdate);
        _disableAllInputButtons();

        if (g_buttonRecord[g_expectedIndexBuffer[j]] == 1) {
          setDisplayNumber(++g_currentScore);
          play_with_led(g_playbackNodeBuffer[j], 1);
          _clearButtonRecord();
        } else {
          --remainLive;
          if (remainLive < 0) {
            setGameState(GAME_OVER);
          }
        }
        g_newRecordUpdate = 0;
      }
      HAL_Delay(250);
    }
    /* WINNING */
    current_bpm += 30;
  }

  /* Clean up */
  g_newStateUpdate = 0;
  turnOffDisplay();
}
void runningGameOverState (void) {
  audio_transition_gameover();
  g_lastHighestScore = (g_currentScore > g_lastHighestScore) ? (g_currentScore) : (g_lastHighestScore);
  setDisplayNumber(g_currentScore);
  _enableAllInputButtons();
  /* Blinking current score */
  while (!g_newStateUpdate) {
    turnOnDisplay();
    HAL_Delay(500);
    turnOffDisplay();
    HAL_Delay(500);
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
  blinkLedOnBoard(); /* For battery power stable */
  init_rgb_driver();
  turnOnDisplay();
  setDisplayIdle();
  renderBatteryPercentage();
  g_state = TEST_MODE;

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

      case MUSIC:
        g_force_stop = 0;
        play_song(g_target_music, &g_force_stop);
        g_state = START_GAME;
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
  HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 4, 0);
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
        case PLAYING:
          g_newRecordUpdate = 1; /* Break while loop */
          setGameState(START_GAME);
          break;
        case TEST_MODE:
        case START_GAME:
          setGameState(START_GAME);
          break;
        case GAME_OVER:
          setGameState(START_GAME);
        default:
        case MUSIC:
          g_force_stop = 1;
        break;
      }
    break;

    case BUTTON1_Pin:
      switch (g_state) {
        case PLAYING: g_buttonRecord[0] = 1; g_newRecordUpdate = 1; break;
        case GAME_OVER: break;
        default: setDisplayNumber(1); audio_button_1(); break;
      }
    break;
    case BUTTON2_Pin:
      switch (g_state) {
        case START_GAME: g_level = EASY; setGameState(PLAYING); break;
        case PLAYING: g_buttonRecord[1] = 1; g_newRecordUpdate = 1; break;
        case GAME_OVER: break;
        default: setDisplayNumber(2); audio_button_2(); break;
      }
    break;
    case BUTTON3_Pin:
      switch (g_state) {
        case PLAYING: g_buttonRecord[2] = 1; g_newRecordUpdate = 1; break;
        case GAME_OVER: break;
        default: setDisplayNumber(3); audio_button_3(); break;
      }
    break;
    case BUTTON4_Pin:
      switch (g_state) {
        case START_GAME: g_level = NORMAL; setGameState(PLAYING); break;
        case PLAYING: g_buttonRecord[3] = 1; g_newRecordUpdate = 1; break;
        case GAME_OVER: break;
        default: setDisplayNumber(4); audio_button_4(); break;
      }
    break;
    case BUTTON5_Pin:
      switch (g_state) {
        case PLAYING: g_buttonRecord[4] = 1; g_newRecordUpdate = 1; break;
        case GAME_OVER: g_target_music = SUPER_MARIO; setGameState(MUSIC); break;
        default: setDisplayNumber(5); audio_button_5(); break;
      }
    break;
    case BUTTON6_Pin:
      switch (g_state) {
        case START_GAME: g_level = HARD; setGameState(PLAYING); break;
        case PLAYING: g_buttonRecord[5] = 1; g_newRecordUpdate = 1; break;
        case GAME_OVER: g_target_music = BOBOMB_BATTLEFIELD; setGameState(MUSIC); break;
        default: setDisplayNumber(6); audio_button_6(); break;
      }
    break;
    case BUTTON7_Pin:
      switch (g_state) {
        case PLAYING: g_buttonRecord[6] = 1; g_newRecordUpdate = 1; break;
        case GAME_OVER: g_target_music = PRINCESS_SLIDE; setGameState(MUSIC); break;
        default: setDisplayNumber(7); audio_button_7(); break;
      }
    break;

    default:
    break;
  }

  /* Make sure to clear all button noise before exit */
  __HAL_GPIO_EXTI_CLEAR_IT(ACT_BUTTON_Pin);
  __HAL_GPIO_EXTI_CLEAR_IT(BUTTON1_Pin);
  __HAL_GPIO_EXTI_CLEAR_IT(BUTTON2_Pin);
  __HAL_GPIO_EXTI_CLEAR_IT(BUTTON3_Pin);
  __HAL_GPIO_EXTI_CLEAR_IT(BUTTON4_Pin);
  __HAL_GPIO_EXTI_CLEAR_IT(BUTTON5_Pin);
  __HAL_GPIO_EXTI_CLEAR_IT(BUTTON6_Pin);
  __HAL_GPIO_EXTI_CLEAR_IT(BUTTON7_Pin);
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
