/*
 * bsp.h
 *
 *  Created on: 8 de ene. de 2017
 *      Author: facun
 */

#ifndef BSP_BSP_H_
#define BSP_BSP_H_

#include "stdint.h"

typedef enum {
				LED1_EXP = 1,
				LED2_EXP,
				LED3_EXP,
				LED4_EXP,
				LED5_EXP,
				LED6_EXP,
				LED7_EXP,
				LED8_EXP
			} leds_Typedef;

leds_Typedef leds_exp;

////// LEDS
#define LED1_EXP_PIN	GPIO_PIN_0
#define LED2_EXP_PIN	GPIO_PIN_1
#define LED3_EXP_PIN	GPIO_PIN_2
#define LED4_EXP_PIN	GPIO_PIN_3
#define LED5_EXP_PIN	GPIO_PIN_6
#define LED6_EXP_PIN	GPIO_PIN_7
#define LED7_EXP_PIN	GPIO_PIN_10
#define LED8_EXP_PIN	GPIO_PIN_11
#define ALL_LEDS		LED1_EXP_PIN | LED2_EXP_PIN | LED3_EXP_PIN | LED4_EXP_PIN | LED5_EXP_PIN | LED6_EXP_PIN | LED7_EXP_PIN | LED8_EXP_PIN
#define LEDS_PORT		GPIOD
#define LED_EXPn		9

////// PUSH BUTTON
#define PUSH1_EXP_PIN		GPIO_PIN_4
#define PUSH2_EXP_PIN		GPIO_PIN_5
#define PUSH3_EXP_PIN		GPIO_PIN_6
#define PUSH4_EXP_PIN		GPIO_PIN_2
#define ALL_BUTTONS			PUSH1_EXP_PIN | PUSH2_EXP_PIN | PUSH3_EXP_PIN | PUSH4_EXP_PIN
#define PUSH_BUTTON_PORT	GPIOE

void BSP_Init(void);

void LedToggle(leds_Typedef led);
void LedOn(leds_Typedef led);
void LedOff(leds_Typedef led);

void LedClear(void);
void TransmitData(uint16_t *buffer, uint16_t size);
#endif /* BSP_BSP_H_ */
