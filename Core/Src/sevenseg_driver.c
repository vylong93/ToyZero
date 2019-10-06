/**
  ******************************************************************************
  * @file    sevenseg_driver.c
  * @brief   This file implement SPI to driving 7-SEG CA tube of 4 digits.
  ******************************************************************************
  * @attention
  *
  * This software component is created base on STM32F1 HAL library.
  *
  ******************************************************************************
  */

#include "sevenseg_driver.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1; /* main.c */
extern TIM_HandleTypeDef htim3; /* main.c */

static uint8_t g_numberDigits[10] = {DIGIT_0, DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4,
                                     DIGIT_5, DIGIT_6, DIGIT_7, DIGIT_8, DIGIT_9};

static uint32_t g_displayValue = 4321;

/**
  * @brief  Display a digit in a specific position
  * @param  position to display range from [0 to 1]
  * @param  value digit to display
  * @retval None
  */
void displayDigit(uint8_t position, uint8_t value)
{
  HAL_GPIO_WritePin(LATCH_7SEG_GPIO_Port, LATCH_7SEG_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, &value, 1, 1);
  HAL_SPI_Transmit(&hspi1, &position, 1, 1);
  HAL_GPIO_WritePin(LATCH_7SEG_GPIO_Port, LATCH_7SEG_Pin, GPIO_PIN_SET);
}

/**
  * @brief  Turn on scanner
  * @retval None
  */
void turnOnDisplay(void) {
  HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_3);
}

/**
  * @brief  Turn off all 7-SEGs
  * @retval None
  */
void turnOffDisplay(void) {
  HAL_TIM_OC_Stop_IT(&htim3, TIM_CHANNEL_3);
  displayDigit(0x00, DIGIT_NONE);
}

/**
  * @brief  Set a specific RGB color to a target LED
  * @param  position to display range from [0 to 1]
  * @param  number to display from 0 to 9
  * @retval None
  */
void displayNumber(uint8_t position, uint8_t number)
{
  position = 1 << position; /* Convert to SEGLED_X */
  number = g_numberDigits[number % 10];
  displayDigit(position, number);
}

/**
  * @brief  Test 7-SEG tube
  * @retval None
  */
void test_sevenseg_tube(void)
{
  uint8_t patern[] = {DIGIT_DOT, DIGIT_NONE, DIGIT_0, DIGIT_1, DIGIT_2, DIGIT_3,
                      DIGIT_4, DIGIT_5, DIGIT_6, DIGIT_7, DIGIT_8, DIGIT_9,
                      DIGIT_A, DIGIT_B, DIGIT_C, DIGIT_D, DIGIT_E, DIGIT_F };
  displayDigit(0x0F, 0x00);
  HAL_Delay(500);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 18; j++) {
      displayDigit(1 << i, patern[j]);
      HAL_Delay(100);
    }
  }
  turnOffDisplay();
}

/**
  * @brief  Set the display value of seven segment tube
  * @param  value in decimal format, maxium 4 digits
  * @retval None
  */
void setDisplayNumber(uint32_t value) {
  /* TODO: protection */
  g_displayValue = value;
}

/**
  * @brief  Display one digit on Seven Segment Tube in order.
  *         This function need to be put into an 4ms loop to render 4 digits nicely
  * @retval None
  */
void performSevenSegOneStepScan(void) {
  static int i = 0;
  /* TODO: use power(10, i) instead of if else */
  switch (i) {
    case 1:
      displayNumber(0, g_displayValue % 10);
    break;

    case 2:
      if (g_displayValue >= 10) {
        displayNumber(1, (g_displayValue / 10) % 10);
      } else {
        displayDigit(1, DIGIT_NONE);
      }
    break;

    case 3:
      if (g_displayValue >= 100) {
        displayNumber(2, (g_displayValue / 100) % 10);
      } else {
        displayDigit(2, DIGIT_NONE);
      }
    break;

    case 4:
      if (g_displayValue >= 1000) {
        displayNumber(3, (g_displayValue / 1000) % 10);
      } else {
        displayDigit(3, DIGIT_NONE);
      }
    break;

    default:
      i = 0;
    break;
  }
  ++i;
}

/***************************************************************END OF FILE****/
