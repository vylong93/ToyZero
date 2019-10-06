/**
  ******************************************************************************
  * @file           : sevenseg_driver.h
  * @brief          : Header for sevenseg_driver.c file.
  *                   This file contains the common defines for driving 7-SEG tube
  ******************************************************************************
  * @attention
  *
  * This software component is created base on STM32F1 HAL library.
  *
  ******************************************************************************
  */

#ifndef __SEVENSEG_DRIVER_H
#define __SEVENSEG_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#define TUBE_LENGTH (4)

/* Common Anode 7-segment LEDS tube */
#define SEGLED_0 0x01
#define SEGLED_1 0x02
#define SEGLED_2 0x04
#define SEGLED_3 0x08

#define DIGIT_DASH 0xBF
#define DIGIT_DOT 0x7F
#define DIGIT_NONE 0xFF
#define DIGIT_0 0xC0
#define DIGIT_1 0xF9
#define DIGIT_2 0xA4
#define DIGIT_3 0xB0
#define DIGIT_4 0x99
#define DIGIT_5 0x92
#define DIGIT_6 0x82
#define DIGIT_7 0xF8
#define DIGIT_8 0x80
#define DIGIT_9 0x98
#define DIGIT_A 0x88
#define DIGIT_B 0x83
#define DIGIT_C 0xC6
#define DIGIT_D 0xA1
#define DIGIT_E 0x86
#define DIGIT_F 0x8E

void turnOnDisplay(void);
void turnOffDisplay(void);
void displayNumber(uint8_t position, uint8_t number);
void test_sevenseg_tube(void);

void setDisplayIdle(void);
void setDisplayNumber(uint32_t value);
void performSevenSegOneStepScan(void);

#ifdef __cplusplus
}
#endif

#endif /* __SEVENSEG_DRIVER_H */

/***************************************************************END OF FILE****/
