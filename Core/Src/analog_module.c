/**
  ******************************************************************************
  * @file    analog_module.c
  * @brief   This file implement sampling analog data from ADC1
  ******************************************************************************
  * @attention
  *
  * This software component is created base on STM32F1 HAL library.
  *
  ******************************************************************************
  */

#include "analog_module.h"
#include "main.h"

extern ADC_HandleTypeDef hadc1; /* main.c */

/**
  * @brief  Blocking call that sampling the battery sense pin and convert to %
  * @retval Battery percentage from 0% to 100%
  */
uint32_t getBatteryPercentage(void) {
  uint32_t batterySense = 0;
  float batteryVoltage = 0.0f;

  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1, 1000);
  batterySense = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);

  batterySense &= 0x0FFF; /* Mask with 12-bit*/
  batteryVoltage = (3300.0f/4096.0f) * (float)(batterySense * 2);

  /* Convert to percentage */
  int32_t battInt = (int32_t)batteryVoltage;
  battInt -= 3200;
  battInt = (battInt < 0) ? (0) : (battInt / 10);
  battInt = (battInt > 40) ? (battInt) : (battInt >> 1);
  return battInt;
}

/***************************************************************END OF FILE****/
