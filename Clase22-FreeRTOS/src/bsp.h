/*
 * bsp.h
 *
 *  Created on: 11 de ago. de 2016
 *      Author: LCSR-AF
 */

#ifndef BSP_BSP_H_
#define BSP_BSP_H_

#include "stdint.h"

#define LEDS_PORT GPIOD

enum{
	LED_VERDE = 0,
	LED_ROJO,
	LED_NARANJA,
	LED_AZUL
}leds;


#define EXP_BOARD_POT_PIN				   GPIO_PIN_2
#define EXP_BOARD_POT_PORT                   GPIOC
#define EXP_BOARD_POT_PIN_CLK_ENABLE()           __GPIOC_CLK_ENABLE()
#define EXP_BOARD_POT_ADC_CLK_ENABLE()		__ADC1_CLK_ENABLE()
#define EXP_BOARD_POT_CLK_DISABLE()          __GPIOD_CLK_DISABLE()
#define EXP_BOARD_POT_CHANNEL				   ADC_CHANNEL_12


void BSP_Init(void);

uint32_t Get_SW_State(void);

void led_setBright(uint8_t led, uint8_t value);

void BSP_ADC_Init(void);
uint8_t BSP_GetBrightness(void);

#endif /* BSP_BSP_H_ */
