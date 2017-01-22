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

/**Declaración de las tareas del sistema**/
static void vSecuenciaUno(void *pvParameters);
static void vSecuenciaDos(void *pvParameters);
static void vSecuenciaTres(void *pvParameters);
static void vBoton(void *pvParameters);
static void vSerie(void *pvParameters);

SemaphoreHandle_t Secuencia_Uno;	//Semáforo para habilitar o bloquear la tarea vSecuenciaUno
SemaphoreHandle_t Secuencia_Dos;	//Semáforo para habilitar o bloquear la tarea vSecuenciaDos
SemaphoreHandle_t Secuencia_Tres;	//Semáforo para habilitar o bloquear la tarea vSecuenciaTres
SemaphoreHandle_t Boton;			//Semáforo para habilitar o bloquear la tarea vBoton
SemaphoreHandle_t Serie;			//Semáforo para habilitar o bloquear la tarea vSerie
QueueHandle_t Cola = 0;

static void vSecuenciaUno(void *pvParameters)
{
	uint8_t led = 0, bandera = 0;

	/* Tomar el semaforo por segunda vez para bloquear la tarea*/
	xSemaphoreTake(Secuencia_Uno, portMAX_DELAY);

	for (;;)
	{
		//Delay del sistema operativo
		vTaskDelay(100);

		if(led == 0)
			bandera = 0;

		if(led == 8)
			bandera = 1;

		if(bandera == 0)
		{
			LedOn(led);
			led++;
		}
		else
		{
			led--;
			LedOff(led);
		}

		/* Entrega el semáforo de la secuencia dos para que esta se desbloquee*/
		xSemaphoreGive(Boton);
		/*Toma el semáforo de la secuencia uno para que esta se bloquee*/
		xSemaphoreTake(Secuencia_Uno, portMAX_DELAY);
	}
}

static void vSecuenciaDos(void *pvParameters)
{
	uint8_t led = 0, bandera = 0;

	for (;;)
	{
		//Delay del sistema operativo
		vTaskDelay(100);

		if(led == 0)
		{
			led++;
			bandera = 0;
		}

		if(led == 8)
		{
			led--;
			bandera = 1;
		}

		if(bandera == 0)
		{
			LedOn(led);
			vTaskDelay(100);
			LedOff(led-1);
			led++;
		}
		else
		{
			led--;
			LedOn(led);
			vTaskDelay(100);
			LedOff(led+1);
		}

		/* Entrega el semáforo de la secuencia tres para que esta se desbloquee*/
		xSemaphoreGive(Boton);
		/*Toma el semáforo de la secuencia dos para que esta se bloquee*/
		xSemaphoreTake(Secuencia_Dos, portMAX_DELAY);
	}
}

static void vSecuenciaTres(void *pvParameters)
{
	uint8_t led = 0;

	for (;;)
	{
		//Delay del sistema operativo
		vTaskDelay(100);

		for(led=0;led<8;led++)
			LedOn(led);

		vTaskDelay(100);

		for(led=0;led<8;led++)
			LedOff(led);

		/* Entrega el semáforo de Boton para que esta se desbloquee*/
		xSemaphoreGive(Boton);
		/*Toma el semáforo de la secuencia tres para que esta se bloquee*/
		xSemaphoreTake(Secuencia_Tres, portMAX_DELAY);
	}
}

static void vBoton(void *pvParameters)
{
	uint8_t secuencia = 0;

	for (;;)
	{
		if(BSP_SW_GetState(1) == 0)
		{
			if(secuencia == 0)
				secuencia = 2;
			else
				secuencia--;
		}

		if(BSP_SW_GetState(3) == 0)
		{
			if(secuencia == 2)
				secuencia = 0;
			else
				secuencia++;
		}

		switch(secuencia)
		{
			case 0:
					/* Entrega el semáforo de Secuencia Uno para que esta se desbloquee*/
					xSemaphoreGive(Secuencia_Uno);
					/*Toma el semáforo de Boton para que esta se bloquee*/
					xSemaphoreTake(Boton, portMAX_DELAY);
					break;
			case 1:
					/* Entrega el semáforo de Secuencia Dos para que esta se desbloquee*/
					xSemaphoreGive(Secuencia_Dos);
					/*Toma el semáforo de Boton para que esta se bloquee*/
					xSemaphoreTake(Boton, portMAX_DELAY);
					break;
			case 2:
					/* Entrega el semáforo de Secuencia Tres para que esta se desbloquee*/
					xSemaphoreGive(Secuencia_Tres);
					/*Toma el semáforo de Boton para que esta se bloquee*/
					xSemaphoreTake(Boton, portMAX_DELAY);
					break;
			default:
					/* Entrega el semáforo de Secuencia Uno para que esta se desbloquee*/
					xSemaphoreGive(Secuencia_Uno);
					/*Toma el semáforo de Boton para que esta se bloquee*/
					xSemaphoreTake(Boton, portMAX_DELAY);
					break;
		}
	}
}

static void vSerie(void *pvParameters)
{
	for (;;)
	{

		/* Entrega el semáforo de la secuencia uno para que esta se desbloquee*/
		xSemaphoreGive(Secuencia_Uno);
		/*Toma el semáforo de la secuencia tres para que esta se bloquee*/
		xSemaphoreTake(Serie, portMAX_DELAY);
	}
}

int main(void) {
	//vUartInit();

	BSP_Init();

	Cola = xQueueCreate(1, sizeof(int));

	/*Creación de los semáforos*/
	Secuencia_Uno = xSemaphoreCreateBinary();
	Secuencia_Dos = xSemaphoreCreateBinary();
	Secuencia_Tres = xSemaphoreCreateBinary();
	Boton = xSemaphoreCreateBinary();
	Serie = xSemaphoreCreateBinary();

	/*Crear las tareas*/
	xTaskCreate(vSecuenciaUno, "Rutina 1", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	xTaskCreate(vSecuenciaDos, "Rutina 2", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	xTaskCreate(vSecuenciaTres, "Rutina 3", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	xTaskCreate(vBoton, "Boton", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(vSerie, "Serie", configMINIMAL_STACK_SIZE, NULL, 3, NULL);

	/* Tomar semáforos por primera vez */
	xSemaphoreTake(Secuencia_Uno, 0);
	xSemaphoreTake(Secuencia_Dos, 0);
	xSemaphoreTake(Secuencia_Tres, 0);
	xSemaphoreTake(Boton, 0);
	xSemaphoreTake(Serie, 0);

	/*Después de ejecutar las configuraciones iniciales se inicia el scheduler*/
	vTaskStartScheduler();

	//should never get here
	printf("ERORR: vTaskStartScheduler returned!");
	for (;;)
		;
}

