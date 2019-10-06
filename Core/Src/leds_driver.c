/**
  ******************************************************************************
  * @file    leds_driver.c
  * @brief   This file implement Timer PWN channel to driving RGB LEDs.
  *          Each RGB LED is multiplexed to share 3 channels of TIM1.
  ******************************************************************************
  * @attention
  *
  * This software component is reference from Ontaelio' How to Make Proper Rainbow
  * and Random Colors With the RGB Color Model
  * https://www.instructables.com/id/How-to-Make-Proper-Rainbow-and-Random-Colors-With-/
  *
  ******************************************************************************
  */

#include "leds_driver.h"
#include "main.h"

#define ENABLE_SINE
//#define ENABLE_HSV
//#define ENABLE_HSV_POWER

extern TIM_HandleTypeDef htim1; /* main.c */

static GPIO_TypeDef* g_portLeds[NUMBER_OF_LEDS] = { RGB_LED1_GPIO_Port,
                                                    RGB_LED2_GPIO_Port,
                                                    RGB_LED3_GPIO_Port,
                                                    RGB_LED4_GPIO_Port,
                                                    RGB_LED5_GPIO_Port,
                                                    RGB_LED6_GPIO_Port,
                                                    RGB_LED7_GPIO_Port
};
static uint16_t g_pinLeds[NUMBER_OF_LEDS] = { RGB_LED1_Pin,
                                              RGB_LED2_Pin,
                                              RGB_LED3_Pin,
                                              RGB_LED4_Pin,
                                              RGB_LED5_Pin,
                                              RGB_LED6_Pin,
                                              RGB_LED7_Pin
};

#ifdef ENABLE_SINE
static const unsigned char lights[360]={
  0,   0,   0,   0,   0,   1,   1,   2,
  2,   3,   4,   5,   6,   7,   8,   9,
  11,  12,  13,  15,  17,  18,  20,  22,
  24,  26,  28,  30,  32,  35,  37,  39,
  42,  44,  47,  49,  52,  55,  58,  60,
  63,  66,  69,  72,  75,  78,  81,  85,
  88,  91,  94,  97, 101, 104, 107, 111,
  114, 117, 121, 124, 127, 131, 134, 137,
  141, 144, 147, 150, 154, 157, 160, 163,
  167, 170, 173, 176, 179, 182, 185, 188,
  191, 194, 197, 200, 202, 205, 208, 210,
  213, 215, 217, 220, 222, 224, 226, 229,
  231, 232, 234, 236, 238, 239, 241, 242,
  244, 245, 246, 248, 249, 250, 251, 251,
  252, 253, 253, 254, 254, 255, 255, 255,
  255, 255, 255, 255, 254, 254, 253, 253,
  252, 251, 251, 250, 249, 248, 246, 245,
  244, 242, 241, 239, 238, 236, 234, 232,
  231, 229, 226, 224, 222, 220, 217, 215,
  213, 210, 208, 205, 202, 200, 197, 194,
  191, 188, 185, 182, 179, 176, 173, 170,
  167, 163, 160, 157, 154, 150, 147, 144,
  141, 137, 134, 131, 127, 124, 121, 117,
  114, 111, 107, 104, 101,  97,  94,  91,
   88,  85,  81,  78,  75,  72,  69,  66,
   63,  60,  58,  55,  52,  49,  47,  44,
   42,  39,  37,  35,  32,  30,  28,  26,
   24,  22,  20,  18,  17,  15,  13,  12,
   11,   9,   8,   7,   6,   5,   4,   3,
    2,   2,   1,   1,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0};

/**
  * @brief  Set the sine wave rainbow
  * @param  angle value in degree 0 to 360
  * @retval None
  */
void set_rgb_sineLED(int angle)
{
  set_rgb(lights[(angle+120)%360] >> 2, lights[angle] >> 2,  lights[(angle+240)%360] >> 2);
}
#endif /* ENABLE_SINE */

#ifdef ENABLE_HSV
static const unsigned char HSVlights[61] =
  {0, 4, 8, 13, 17, 21, 25, 30, 34, 38, 42, 47, 51, 55, 59, 64, 68, 72, 76,
  81, 85, 89, 93, 98, 102, 106, 110, 115, 119, 123, 127, 132, 136, 140, 144,
  149, 153, 157, 161, 166, 170, 174, 178, 183, 187, 191, 195, 200, 204, 208,
  212, 217, 221, 225, 229, 234, 238, 242, 246, 251, 255};

/**
  * @brief  Set the real HSV rainbow
  * @param  angle value in degree 0 to 360
  * @retval None
  */
void set_rgb_trueHSV(int angle)
{
  unsigned char red, green, blue;

  if (angle<60) {red = 255; green = HSVlights[angle]; blue = 0;} else
  if (angle<120) {red = HSVlights[120-angle]; green = 255; blue = 0;} else
  if (angle<180) {red = 0, green = 255; blue = HSVlights[angle-120];} else
  if (angle<240) {red = 0, green = HSVlights[240-angle]; blue = 255;} else
  if (angle<300) {red = HSVlights[angle-240], green = 0; blue = 255;} else
                 {red = 255, green = 0; blue = HSVlights[360-angle];}
  set_rgb(red, green, blue);
}
#endif /* ENABLE_HSV */

#ifdef ENABLE_HSV_POWER
static const unsigned char HSVpower[121] =
  {0, 2, 4, 6, 8, 11, 13, 15, 17, 19, 21, 23, 25, 28, 30, 32, 34, 36, 38, 40,
  42, 45, 47, 49, 51, 53, 55, 57, 59, 62, 64, 66, 68, 70, 72, 74, 76, 79, 81,
  83, 85, 87, 89, 91, 93, 96, 98, 100, 102, 104, 106, 108, 110, 113, 115, 117,
  119, 121, 123, 125, 127, 130, 132, 134, 136, 138, 140, 142, 144, 147, 149,
  151, 153, 155, 157, 159, 161, 164, 166, 168, 170, 172, 174, 176, 178, 181,
  183, 185, 187, 189, 191, 193, 195, 198, 200, 202, 204, 206, 208, 210, 212,
  215, 217, 219, 221, 223, 225, 227, 229, 232, 234, 236, 238, 240, 242, 244,
  246, 249, 251, 253, 255};

/**
  * @brief  Set the 'power-conscious' HSV rainbow
  * @param  angle value in degree 0 to 360
  * @retval None
  */
void set_rgb_powerHSV(int angle)
{
  unsigned char red, green, blue;
  if (angle<120) {red = HSVpower[120-angle]; green = HSVpower[angle]; blue = 0;} else
  if (angle<240) {red = 0;  green = HSVpower[240-angle]; blue = HSVpower[angle-120];} else
                 {red = HSVpower[angle-240]; green = 0; blue = HSVpower[360-angle];}
    set_rgb(red, green, blue);
}
#endif /* ENABLE_HSV_POWER */

/**
  * @brief  Start three PWM channels to control RGB color
  * @retval None
  */
void init_rgb_driver(void) {
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
}

/**
  * @brief  Turn on an RGB LED
  * @param  led target LED to turn ON
  * @retval None
  */
void turn_led_on(led_rgb_t led) {
  HAL_GPIO_WritePin(g_portLeds[led], g_pinLeds[led], GPIO_PIN_RESET);
}

/**
  * @brief  Turn off an RGB LED
  * @param  led target LED to turn OFF
  * @retval None
  */
void turn_led_off(led_rgb_t led) {
  HAL_GPIO_WritePin(g_portLeds[led], g_pinLeds[led], GPIO_PIN_SET);
}

/**
  * @brief  Turn on all LEDs
  * @retval None
  */
void turn_on_all_leds(void) {
  for (int l = 0; l < NUMBER_OF_LEDS; l++) {
    turn_led_on(l);
  }
}

/**
  * @brief  Turn off all LEDs
  * @retval None
  */
void turn_off_all_leds(void) {
  for (int l = 0; l < NUMBER_OF_LEDS; l++) {
    turn_led_off(l);
  }
}

/**
  * @brief  Set a specific RGB color to a target LED
  * @param  r value for RED color from range 0-255
  * @param  g value for GREEN color from range 0-255
  * @param  b value for BLUE color from range 0-255
  * @retval None
  */
void set_rgb(unsigned char r, unsigned char g, unsigned char b) {
  htim1.Instance->CCR1 = r;
  htim1.Instance->CCR2 = g;
  htim1.Instance->CCR3 = b;
}

/**
  * @brief  Test all RGB LEDs, turn them all ON in sequence and then all OFF at a same time
  * @retval None
  */
void test_all_rgb_leds(void) {
  for (int l = 0; l < NUMBER_OF_LEDS; l++) {
    turn_led_on(l);
    /* Fade in */
    for (int i = 0; i < 256; i++) {
      set_rgb(i, 0, i);
      HAL_Delay(1);
    }
    /* Fade out */
    for (int i = 255; i >= 0; i--) {
      set_rgb(i, 0, i);
      HAL_Delay(1);
    }
  }
  turn_off_all_leds();
}

/**
  * @brief  Test rainbow color
  * @retval None
  */
void test_rgb_rainbow(void)
{
  for (int k = 0; k < 360; k++)
  {
#ifdef ENABLE_SINE
    set_rgb_sineLED(k);
#else
#ifdef ENABLE_HSV_POWER
    set_rgb_powerHSV(k);
#else
#ifdef ENABLE_HSV
    set_rgb_trueHSV(k);
#endif /* ENABLE_HSV */
#endif /* ENABLE_HSV_POWER */
#endif /* ENABLE_SINE */
    HAL_Delay(30);
  }
}

/**
  * @brief  Led position color mapping
  * @param  led target led position convert to color
  * @retval None
  */
void set_rgb_color_base_on_led(led_rgb_t led) {
  switch (led) {
    case led1:
      set_rgb(150, 0, 0);
      break;
    case led2:
      set_rgb(150, 40, 0);
      break;
    case led3:
      set_rgb(150, 150, 0);
      break;
    case led4:
      set_rgb(0, 150, 0);
      break;
    case led5:
      set_rgb(0, 0, 150);
      break;
    case led6:
      set_rgb(20, 0, 150);
      break;
    case led7:
      set_rgb(150, 0, 150);
      break;
    default:
      set_rgb(0, 0, 0);
      break;
  }
}

/***************************************************************END OF FILE****/
