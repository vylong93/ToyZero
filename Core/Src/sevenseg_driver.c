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

SPI_HandleTypeDef hspi1; /* main.c */

/**
  * @brief  Turn off all 7-SEGs
  * @retval None
  */
void turnOffDisplay(void) {
  displayDigit(0x00, DIGIT_NONE);
}

/**
  * @brief  Set a specific RGB color to a target LED
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

/***************************************************************END OF FILE****/
