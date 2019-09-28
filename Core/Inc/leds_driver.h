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

/* Port: B; Pin: { 13, 14, 15 }
 * TODO add port param
 */
typedef enum led_rgb {
  led0 = GPIO_PIN_13,
  led1 = GPIO_PIN_14,
  led2 = GPIO_PIN_15,
} led_rgb_t;

void init_rgb_driver(void);
void set_rgb(led_rgb_t led, unsigned char r, unsigned char g, unsigned char b);
void test_all_rgb_leds(void);
void set_rgb_trueHSV(led_rgb_t led, int angle);
void set_rgb_powerHSV(led_rgb_t led, int angle);
void set_rgb_sineLED(led_rgb_t led, int angle);

#ifdef __cplusplus
}
#endif

#endif /* __LEDS_DRIVER_H */

/***************************************************************END OF FILE****/
