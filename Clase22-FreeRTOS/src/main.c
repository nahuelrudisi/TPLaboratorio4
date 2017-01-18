/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */

#include "bsp.h"

/* Standard includes */
#include <stdio.h>                          // prototype declarations for I/O functions
#include <string.h>

/* Scheduler includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

TaskHandle_t tareaB;
SemaphoreHandle_t xSemaphore;
QueueHandle_t xQueue1;

void prvTaskA(void* pvParameters) {
	(void) pvParameters;                    // Just to stop compiler warnings.
	char msg = 'A';
	for (;;) {
		xQueueSend(xQueue1, (void * ) &msg, 0);
		xQueueSend(xQueue1, (void * ) &msg, 0);
		xQueueSend(xQueue1, (void * ) &msg, 0);
		vTaskDelay(1000);
	}
}

void prvTaskB(void* pvParameters) {
	(void) pvParameters;                    // Just to stop compiler warnings.
	char msg = 'B';
	for (;;) {
		xQueueSend(xQueue1, (void * ) &msg, 0);
		xQueueSend(xQueue1, (void * ) &msg, 0);
		xQueueSend(xQueue1, (void * ) &msg, 0);
		vTaskDelay(100);
	}
}

void prvTaskC(void* pvParameters) {
	(void) pvParameters;                    // Just to stop compiler warnings.
	char msg;
	char buffer[100];
	int size;
	for (;;) {
		xQueueReceive( xQueue1,(void * ) &( msg ), portMAX_DELAY );
		size = sprintf(buffer, "Recibi mensaje %c\n",msg);
		TransmitData(buffer, size);
	}
}
typedef struct {
	uint8_t led;
	uint32_t int_inicial;
} parametro;

void tareaLed(void* pvParameters) {
	parametro *data = (parametro *) pvParameters;

	int i = data->int_inicial;
	int incre = 1;

	for (;;) {
		vTaskDelay(20);
		led_setBright(data->led, i);

		i += incre;
		if (i >= 75)
			incre = -1;
		else if (i <= 0)
			incre = 1;
	}
}

parametro dataA = { LED_VERDE, 0 };
parametro dataB = { LED_NARANJA, 25 };
parametro dataC = { LED_ROJO, 50 };
parametro dataD = { LED_AZUL, 75 };

int main(void) {
	//vUartInit();

	BSP_Init();

	xTaskCreate(tareaLed, (signed char * ) "TaskA", configMINIMAL_STACK_SIZE,
			(void* ) &dataA, tskIDLE_PRIORITY, ( xTaskHandle * ) NULL);
	xTaskCreate(tareaLed, (signed char * ) "TaskB", configMINIMAL_STACK_SIZE,
			(void* ) &dataB, tskIDLE_PRIORITY, ( xTaskHandle * ) NULL);
	xTaskCreate(tareaLed, (signed char * ) "TaskC", configMINIMAL_STACK_SIZE,
			(void* ) &dataC, tskIDLE_PRIORITY, ( xTaskHandle * ) NULL);
	xTaskCreate(tareaLed, (signed char * ) "TaskD", configMINIMAL_STACK_SIZE,
			(void* ) &dataD, tskIDLE_PRIORITY, ( xTaskHandle * ) NULL);

	vTaskStartScheduler();

	//should never get here
	printf("ERORR: vTaskStartScheduler returned!");
	for (;;)
		;
}

void APP_1ms() {

}
