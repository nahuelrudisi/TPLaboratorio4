#include "bsp/bsp.h"
#include "cmsis_os.h"

/**Declaración de las tareas del sistema**/
void vSecuenciaUno (void *Parameters);
void vSecuenciaDos (void *Parameters);
void vSecuenciaTres(void *Parameters);
void vSecuenciaCuatro (void *Parameters);
void vBoton(void *Parameters);
void vSerie(void* pvParameters);

QueueHandle_t botonPresionado = NULL;
QueueHandle_t uartQueue;
TaskHandle_t secuencia[5], boton, UART;

TickType_t ms = 100;
static TickType_t intervalo = 100;

int main(void)
{
	BSP_Init();

	botonPresionado = xQueueCreate(1, sizeof(int));
	uartQueue = xQueueCreate(1, sizeof(int));

	xTaskCreate( vSecuenciaUno, "Rutina 1",	configMINIMAL_STACK_SIZE, NULL, 2, &secuencia[1]);
 	xTaskCreate( vSecuenciaDos, "Rutina 2",  configMINIMAL_STACK_SIZE, NULL, 2, &secuencia[2]);
	xTaskCreate( vSecuenciaTres, "Rutina 3", configMINIMAL_STACK_SIZE, NULL, 2, &secuencia[3]);
	xTaskCreate( vSecuenciaCuatro, "Rutina 4", configMINIMAL_STACK_SIZE, NULL, 2, &secuencia[4]);
 	xTaskCreate( vBoton, "Boton", configMINIMAL_STACK_SIZE, NULL, 3, &boton);
 	xTaskCreate( vSerie, "Serie", configMINIMAL_STACK_SIZE, NULL, 4, &UART);

	vTaskSuspend(secuencia[2]);
	vTaskSuspend(secuencia[3]);
	vTaskSuspend(secuencia[4]);
	vTaskSuspend(boton);

	osKernelStart();

	for(;;);
}

void vSerie(void* pvParameters)
{
	(void) pvParameters;                    // Just to stop compiler warnings.
	char patron;
	char pantalla[50];
	uint16_t size;

	size = sprintf(pantalla, "Patrón Inicial: 1\n");
	TransmitData(pantalla, size);
	size = sprintf(pantalla, "Velocidad Inicial: %dmseg.\n", ms);
	TransmitData(pantalla, size);

	for (;;)
	{
		if(xQueueReceive( uartQueue, &patron, portMAX_DELAY))
		{
			size = sprintf(pantalla, "\nPatrón actual: %c\n",patron);
			TransmitData(pantalla, size);
			size = sprintf(pantalla, "Velocidad actual: %dmseg.\n",ms);
			TransmitData(pantalla, size);
		}
		vTaskSuspend(UART);
	}
}

void vBoton(void *Parameters)
{
	uint8_t secuencia_led = 1, bandera_tiempo = 0;
	uint16_t boton_presionado=-1;
	char msg;

	while (1)
	{
		vTaskDelay(250);	// Antirebote

		xQueueReceive(botonPresionado,&boton_presionado,portMAX_DELAY);
		switch (boton_presionado)
		{
			case 0:
					if(secuencia_led == 4)
						secuencia_led = 1;
					else
						secuencia_led++;

					for (uint16_t x= 1 ; x <= 4 ; x++)
						vTaskSuspend(secuencia[x]);

					vTaskResume(secuencia[secuencia_led]);
					break;

			case 1:
					if(secuencia_led == 1)
						secuencia_led = 4;
					else
						secuencia_led--;

					for (uint16_t x= 1 ; x < 5 ; x++)
						vTaskSuspend(secuencia[x]);

					vTaskResume(secuencia[secuencia_led]);
					break;

			case 2:
					if (ms >= intervalo)
						ms -= intervalo;
					else
						ms = 0;
					break;

			case 3:
					ms += intervalo;
					break;
		}
		msg = secuencia_led + '0';

		vTaskResume(UART);
		xQueueSend(uartQueue,&msg, portMAX_DELAY);
		vTaskSuspend(boton);
	}
}

void vSecuenciaUno (void *Parameters)
{
	uint8_t led = 1, bandera = 0;

	for (;;)
	{
		vTaskDelay(ms);
		if(led == 1)
			bandera = 0;

		if(led == 9)
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
	}
}

void vSecuenciaDos (void *Parameters)
{
	uint8_t led = 1, bandera = 0;

	for (;;)
	{
		if(led == 1)
		{
			led++;
			bandera = 0;
		}

		if(led == 9)
		{
			led--;
			bandera = 1;
		}

		if(bandera == 0)
		{
			LedOn(led);
			LedOff(led-1);
			led++;
		}
		else
		{
			led--;
			LedOn(led);
			LedOff(led+1);
		}
		vTaskDelay(ms);
	}
}

void vSecuenciaTres (void *Parameters)
{
	uint8_t led = 1;

	for (;;)
	{
		for(led=1;led<9;led++)
			LedOn(led);

		vTaskDelay(ms);

		for(led=1;led<9;led++)
			LedOff(led);

		vTaskDelay(ms);
	}
}

void vSecuenciaCuatro (void *Parameters)
{
	uint8_t led = 1;

	for (;;)
	{
		for(led=2;led<9;led+=2)
			LedOff(led);
		for(led=1;led<9;led+=2)
			LedOn(led);

		vTaskDelay(ms);

		for(led=1;led<9;led+=2)
			LedOff(led);
		for(led=2;led<9;led+=2)
			LedOn(led);

		vTaskDelay(ms);
	}
}
