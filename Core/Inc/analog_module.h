/**
  ******************************************************************************
  * @file           : analog_module.h
  * @brief          : Header for analog_module.c file.
  *                   This file contains the common defines for analog stuff
  ******************************************************************************
  * @attention
  *
  * This software component is created base on STM32F1 HAL library.
  *
  ******************************************************************************
  */

#ifndef __ANALOG_MODULE_H
#define __ANALOG_MODULE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

uint32_t getBatteryPercentage(void);

#ifdef __cplusplus
}
#endif

#endif /* __ANALOG_MODULE_H */

/***************************************************************END OF FILE****/
