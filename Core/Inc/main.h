/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
#define LED_ONBOARD_Pin GPIO_PIN_13
#define LED_ONBOARD_GPIO_Port GPIOC
#define BATT_SENSE_Pin GPIO_PIN_1
#define BATT_SENSE_GPIO_Port GPIOA
#define RGB_LED1_Pin GPIO_PIN_2
#define RGB_LED1_GPIO_Port GPIOA
#define RGB_LED2_Pin GPIO_PIN_3
#define RGB_LED2_GPIO_Port GPIOA
#define LATCH_7SEG_Pin GPIO_PIN_4
#define LATCH_7SEG_GPIO_Port GPIOA
#define CLOCK_7SEG_Pin GPIO_PIN_5
#define CLOCK_7SEG_GPIO_Port GPIOA
#define DATA_7SEG_Pin GPIO_PIN_7
#define DATA_7SEG_GPIO_Port GPIOA
#define RGB_LED3_Pin GPIO_PIN_0
#define RGB_LED3_GPIO_Port GPIOB
#define RGB_LED4_Pin GPIO_PIN_1
#define RGB_LED4_GPIO_Port GPIOB
#define RGB_LED5_Pin GPIO_PIN_10
#define RGB_LED5_GPIO_Port GPIOB
#define RGB_LED6_Pin GPIO_PIN_11
#define RGB_LED6_GPIO_Port GPIOB
#define RGB_LED7_Pin GPIO_PIN_12
#define RGB_LED7_GPIO_Port GPIOB
#define BUTTON7_Pin GPIO_PIN_13
#define BUTTON7_GPIO_Port GPIOB
#define BUTTON7_EXTI_IRQn EXTI15_10_IRQn
#define BUTTON6_Pin GPIO_PIN_14
#define BUTTON6_GPIO_Port GPIOB
#define BUTTON6_EXTI_IRQn EXTI15_10_IRQn
#define BUTTON5_Pin GPIO_PIN_15
#define BUTTON5_GPIO_Port GPIOB
#define BUTTON5_EXTI_IRQn EXTI15_10_IRQn
#define RED_CHANNEL_Pin GPIO_PIN_8
#define RED_CHANNEL_GPIO_Port GPIOA
#define GREEN_CHANNEL_Pin GPIO_PIN_9
#define GREEN_CHANNEL_GPIO_Port GPIOA
#define BLUE_CHANNEL_Pin GPIO_PIN_10
#define BLUE_CHANNEL_GPIO_Port GPIOA
#define BUTTON4_Pin GPIO_PIN_5
#define BUTTON4_GPIO_Port GPIOB
#define BUTTON4_EXTI_IRQn EXTI9_5_IRQn
#define BUTTON3_Pin GPIO_PIN_6
#define BUTTON3_GPIO_Port GPIOB
#define BUTTON3_EXTI_IRQn EXTI9_5_IRQn
#define BUTTON2_Pin GPIO_PIN_7
#define BUTTON2_GPIO_Port GPIOB
#define BUTTON2_EXTI_IRQn EXTI9_5_IRQn
#define BUTTON1_Pin GPIO_PIN_8
#define BUTTON1_GPIO_Port GPIOB
#define BUTTON1_EXTI_IRQn EXTI9_5_IRQn
#define BUZZER_Pin GPIO_PIN_9
#define BUZZER_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
