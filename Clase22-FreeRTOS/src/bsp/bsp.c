/*
 * bsp.c
 *
 *  Created on: 8 de ene. de 2017
 *      Author: facun
 */
#include "bsp.h"
#include "cmsis_os.h"

#include "stm32f4xx.h"
#include "stm32f4_discovery.h"

extern xQueueHandle botonPresionado;
extern TaskHandle_t boton;

UART_HandleTypeDef UART3_Handle;
TIM_HandleTypeDef TIM2_Handle;


const uint16_t SW_ARRAY[4] = {GPIO_PIN_2 , GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_4};
void RCC_system_clock(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	  // Enable Power Control clock
	__PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	// Enable HSE Oscillator and activate PLL with HSE as source

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

void HAL_Conf_Init(void)
{
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/
  /* MemoryManagement_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
  /* BusFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
  /* UsageFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
  /* SVCall_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
  /* DebugMonitor_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

void LED_Init(void) {

		__GPIOD_CLK_ENABLE();

		GPIO_InitTypeDef LED_Init;

		LED_Init.Mode = GPIO_MODE_OUTPUT_PP;
		LED_Init.Pull = GPIO_NOPULL;
		LED_Init.Speed = GPIO_SPEED_FAST;
		LED_Init.Pin = ALL_LEDS;
		HAL_GPIO_Init(LEDS_PORT, &LED_Init);
}

const uint16_t LED_PIN[LED_EXPn] = {NULL ,LED1_EXP_PIN, LED2_EXP_PIN, LED3_EXP_PIN, LED4_EXP_PIN, LED5_EXP_PIN, LED6_EXP_PIN, LED7_EXP_PIN, LED8_EXP_PIN};


void LedOn(leds_Typedef led) {
	HAL_GPIO_WritePin(LEDS_PORT, LED_PIN[led], SET);
}

void LedOff(leds_Typedef led) {
	HAL_GPIO_WritePin(LEDS_PORT, LED_PIN[led], RESET);
}

void LedClear(void) {
	for (leds_exp=LED1_EXP; leds_exp<= LED8_EXP; leds_exp++)
		HAL_GPIO_WritePin(LEDS_PORT, LED_PIN[leds_exp], RESET);
}

void SW_Init(void) {

	__GPIOE_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_4| GPIO_PIN_6 |GPIO_PIN_5;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);

	HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);

	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}


void EXTI4_IRQHandler(void)
{
	static uint16_t identifier_SW1 = 3;
	Manager_Resume(boton);
	ISR(identifier_SW1);

}

void EXTI2_IRQHandler(void)
{
	static uint16_t identifier_SW4 = 0;
	function_IRQHANDLER(identifier_SW4);

}

void EXTI9_5_IRQHandler(void)
{
	static uint16_t identifier_SW2 = 1;
	static uint16_t identifier_SW3 = 2;

	function_IRQHANDLER(identifier_SW2);

	Manager_Resume(boton);
	ISR(identifier_SW3);

}

void ISR(uint16_t identifier) {

	static portBASE_TYPE xHigherPrioritytaskWoken = pdFALSE;

	if(__HAL_GPIO_EXTI_GET_IT(SW_ARRAY[identifier]))
	{
		xQueueOverwriteFromISR(botonPresionado,&identifier,&xHigherPrioritytaskWoken);

		  if( xHigherPrioritytaskWoken == pdTRUE )
		  {
			  portEND_SWITCHING_ISR(xHigherPrioritytaskWoken);
		  }
		  __HAL_GPIO_EXTI_CLEAR_FLAG(SW_ARRAY[identifier]);

	}
}

void Manager_Resume (TaskHandle_t xHandle) {

	 BaseType_t xYieldRequired;

	     // Resume the suspended task.
	     xYieldRequired = xTaskResumeFromISR( xHandle );

	     if( xYieldRequired == pdTRUE )
	     {
	         // We should switch context so the ISR returns to a different task.
	         // NOTE:  How this is done depends on the port you are using.  Check
	         // the documentation and examples for your port.
	         portYIELD_FROM_ISR(xYieldRequired);
	     }
}

void function_IRQHANDLER(uint16_t indentifier)
{
	LedClear();
	Manager_Resume(boton);
	ISR(indentifier);

}
void BSP_UART_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct;

	__GPIOD_CLK_ENABLE()
	;

	__HAL_RCC_USART3_CLK_ENABLE()
	;

	GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	UART3_Handle.Instance = USART3;
	UART3_Handle.Init.BaudRate = 115200;
	UART3_Handle.Init.WordLength = UART_WORDLENGTH_8B;
	UART3_Handle.Init.StopBits = UART_STOPBITS_1;
	UART3_Handle.Init.Parity = UART_PARITY_NONE;
	UART3_Handle.Init.Mode = UART_MODE_TX_RX;
	UART3_Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UART3_Handle.Init.OverSampling = UART_OVERSAMPLING_8;
	HAL_UART_Init(&UART3_Handle);

	HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(USART3_IRQn);

}


void TransmitData(uint16_t *buffer, uint16_t size) {
	HAL_UART_Transmit(&UART3_Handle, buffer, size, 100);
}



void BSP_Init(void) {
	RCC_system_clock();
	HAL_Conf_Init();
	LED_Init();
	SW_Init();
	BSP_UART_Init();
}


