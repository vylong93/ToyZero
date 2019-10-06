/**
  ******************************************************************************
  * @file           : leds_driver.h
  * @brief          : Header for leds_driver.c file.
  *                   This file contains the common defines for driving RGB LEDs
  ******************************************************************************
  * @attention
  *
  * This software component is reference from Ontaelio' How to Make Proper Rainbow
  * and Random Colors With the RGB Color Model
  * https://www.instructables.com/id/How-to-Make-Proper-Rainbow-and-Random-Colors-With-/
  *
  ******************************************************************************
  */

#ifndef __LEDS_DRIVER_H
#define __LEDS_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#define NUMBER_OF_LEDS  (7)

typedef enum led_rgb {
  led1 = 0, led2, led3, led4, led5, led6, led7
} led_rgb_t;

void init_rgb_driver(void);
void turn_led_on(led_rgb_t led);
void turn_led_off(led_rgb_t led);
void turn_on_all_leds(void);
void turn_off_all_leds(void);
void set_rgb(unsigned char r, unsigned char g, unsigned char b);
void test_all_rgb_leds(void);
void set_rgb_trueHSV(int angle);
void set_rgb_powerHSV(int angle);
void set_rgb_sineLED(int angle);
void test_rgb_rainbow(void);

#ifdef __cplusplus
}
#endif

#endif /* __LEDS_DRIVER_H */

/***************************************************************END OF FILE****/
