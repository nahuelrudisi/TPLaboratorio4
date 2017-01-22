/*
 * bsp.h
 *
 *  Created on: 11 de ago. de 2016
 *      Author: LCSR-AF
 */

#ifndef BSP_BSP_H_
#define BSP_BSP_H_

#include "stdint.h"
#include "stm32f4xx.h"

#define LED_A	GPIO_PIN_0
#define LED_B	GPIO_PIN_1
#define LED_C	GPIO_PIN_2
#define LED_D	GPIO_PIN_3
#define LED_E	GPIO_PIN_6
#define LED_F	GPIO_PIN_7
#define LED_G	GPIO_PIN_10
#define LED_H	GPIO_PIN_11

#define LEDS_PORT GPIOD

#define SW_UP_PIN          GPIO_PIN_4
#define SW_LEFT_PIN        GPIO_PIN_5
#define SW_DOWN_PIN        GPIO_PIN_6
#define SW_RIGHT_PIN       GPIO_PIN_2

void BSP_Init(void);
void led_setBright(uint8_t led, uint8_t value);
void BSP_ADC_Init(void);
void LedOn(uint8_t led);
void LedOff(uint8_t led);
uint8_t BSP_GetBrightness(void);
uint32_t BSP_SW_GetState(uint8_t sw);

#endif /* BSP_BSP_H_ */
