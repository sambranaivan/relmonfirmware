/* Copyright 2014, ChaN
 * Copyright 2016, Matias Marando
 * Copyright 2016, Eric Pernia
 * All rights reserved.
 *
 * This file is part of Workspace.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "board.h"
#include "sapi.h"
#include "ff.h"

/*==================[macros and definitions]=================================*/

#define FILENAME "hola.txt"

/*==================[internal data declaration]==============================*/

static FATFS fs;           /**< FatFs work area needed for each volume */
static FIL fp;             /**< File object needed for each open file */

/*==================[internal functions declaration]=========================*/


/*==================[internal functions definition]==========================*/
char* itoa(int value, char* result, int base) {
   // check that the base if valid
   if (base < 2 || base > 36) { *result = '\0'; return result; }

   char* ptr = result, *ptr1 = result, tmp_char;
   int tmp_value;

   do {
      tmp_value = value;
      value /= base;
      *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
   } while ( value );

   // Apply negative sign
   if (tmp_value < 0) *ptr++ = '-';
   *ptr-- = '\0';
   while(ptr1 < ptr) {
      tmp_char = *ptr;
      *ptr--= *ptr1;
      *ptr1++ = tmp_char;
   }
   return result;
}


char* obtenerRegistros(char* result)
{
	   char buff[10];
	uint16_t muestra = 0;
	char * registro = result;
  int j;
   for(j = 0; j<=70; j++){registro[j] = 0;}
	muestra = adcRead( AI0 );


	int i;
	for(i=0;i<=12;i++)
	{
		itoa(muestra + i*5,buff,10);
	if(i == 0){
		strcpy(registro,"\r\n");
		strcat(registro,buff);}
	else
	{
		strcat(registro,buff);
	}


	if(i != 12)
		{strcat(registro,",");}

	}
	return result;
	}

void escribirSD(char* result)
{
	UINT nbytes;
	  /* ------ PROGRAMA QUE ESCRIBE EN LA SD ------- */
	char * registro = result;
	   /* Give a work area to the default drive */
	   if( f_mount( &fs, "", 0 ) != FR_OK ){
	      /* If this fails, it means that the function could
	       * not register a file system object.
	       * Check whether the SD card is correctly connected */
	   }

	   /* Create/open a file, then write a string and close it */
	   if( f_open( &fp, FILENAME, FA_WRITE | FA_OPEN_APPEND ) == FR_OK ){
	      //f_write( &fp, "Hola mundo\r\n", 12, &nbytes );
		   f_write( &fp, registro, 70, &nbytes );

	      f_close(&fp);

	      if( nbytes == 70 ){
	         /* Turn ON LEDG if the write operation was successful */
	         gpioWrite( LEDG, ON );

	      }
	   } else{
	      /* Turn ON LEDR if the write operation was fail */
	      gpioWrite( LEDR, ON );
	   }
	}

void enviarUSB(char* result)
{
		char * registro = result;

        uartWriteString(UART_USB, registro);


}
/*==================[external functions definition]==========================*/


/* FUNCION que se ejecuta cada vezque ocurre un Tick. */
bool_t diskTickHook( void *ptr ){
   disk_timerproc();   /* Disk timer process */
   return 1;
}


/* FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE RESET. */
int main(void){

   /* ------------- INICIALIZACIONES ------------- */

   //UINT nbytes;
   char registro[70];
   

   /* Inicializar la placa */
   boardConfig();

   /* Inicializar GPIOs */
     gpioConfig( 0, GPIO_ENABLE );

     /* Configuración de pines de entrada para Teclas de la EDU-CIAA-NXP */
     gpioConfig( TEC1, GPIO_INPUT );
     gpioConfig( TEC2, GPIO_INPUT );
     gpioConfig( TEC3, GPIO_INPUT );
     gpioConfig( TEC4, GPIO_INPUT );

     /* Configuración de pines de salida para Leds de la EDU-CIAA-NXP */
     gpioConfig( LEDR, GPIO_OUTPUT );
     gpioConfig( LEDG, GPIO_OUTPUT );
     gpioConfig( LEDB, GPIO_OUTPUT );
     gpioConfig( LED1, GPIO_OUTPUT );
     gpioConfig( LED2, GPIO_OUTPUT );
     gpioConfig( LED3, GPIO_OUTPUT );


     /* Configuración de pines de entrada de la CIAA-NXP */
     gpioConfig( DI0, GPIO_INPUT );
     gpioConfig( DI1, GPIO_INPUT );
     gpioConfig( DI2, GPIO_INPUT );
     gpioConfig( DI3, GPIO_INPUT );
     gpioConfig( DI4, GPIO_INPUT );
     gpioConfig( DI5, GPIO_INPUT );
     gpioConfig( DI6, GPIO_INPUT );
     gpioConfig( DI7, GPIO_INPUT );

     /* Configuración de pines de salida de la CIAA-NXP */
     gpioConfig( DO0, GPIO_OUTPUT );
     gpioConfig( DO1, GPIO_OUTPUT );
     gpioConfig( DO2, GPIO_OUTPUT );
     gpioConfig( DO3, GPIO_OUTPUT );
     gpioConfig( DO4, GPIO_OUTPUT );
     gpioConfig( DO5, GPIO_OUTPUT );
     gpioConfig( DO6, GPIO_OUTPUT );
     gpioConfig( DO7, GPIO_OUTPUT );

   /* Inicializar el conteo de Ticks con resolución de 10ms, 
    * con tickHook diskTickHook */
   tickConfig( 10, diskTickHook );

   /* SPI configuration */
   Board_SSP_Init(LPC_SSP1);
   Chip_SSP_Init(LPC_SSP1);
   Chip_SSP_Enable(LPC_SSP1);

   /* Inicializar AnalogIO */
     /* Posibles configuraciones:
      *    ADC_ENABLE,  ADC_DISABLE,
      *    ADC_ENABLE,  ADC_DISABLE,
      */
     adcConfig( ADC_ENABLE ); /* ADC */
     dacConfig( DAC_ENABLE ); /* DAC */
     uartConfig( UART_USB, 115200 );
     /* Variable para almacenar el valor leido del ADC CH1 */


             


   /* ------------- REPETIR POR SIEMPRE ------------- */
   while(1) {

	   obtenerRegistros(registro);
	   escribirSD(registro);
	   enviarUSB(registro);
	   delay(2000);
	   gpioWrite( LEDG, OFF );
	   delay(1000);
      //sleepUntilNextInterrupt();
   }

   /* NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa no es llamado
      por ningun S.O. */
   return 0 ;
}

/*==================[end of file]============================================*/
