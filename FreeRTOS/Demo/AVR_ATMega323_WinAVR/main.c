/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * Creates TCP server task that monitors socket connection for any received data.
 * The task blocks until data has arrived and responds accordingly. 
 */

/* Standard C includes. */
#include <stdlib.h>
#include <string.h>

#define F_CPU           16000000
#include <util/delay.h>

#ifdef GCC_MEGA_AVR
	/* EEPROM routines used only with the WinAVR compiler. */
	#include <avr/eeprom.h>
#endif

#include <avr/io.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* TCP functionality */
#include "tcp/tcp_server.h"
#include "socket.h"

/* UART functionality */
#include "uart.h"

/* Priority definitions for most of the tasks in the demo application.  Some
tasks just use the idle priority. */
#define mainTCP_RX_TASK_PRIORITY			( tskIDLE_PRIORITY + 3 )

/* Prototypes for tasks defined within this file. */ 
static void vBlinkyFunction( void *pvParameters );
static void vTcpServerLoopback( void *pvParameters );

/* The idle hook is used to just blink LED. */
void vApplicationIdleHook( void );

/*-----------------------------------------------------------*/
int main( void )
{
    initUART();
    vInitSPI();
    DDRB |= 0x20;
    
    _delay_ms(1000);

    PORTB |= 0x20;
    _delay_ms(1000);
    PORTB &= ~0x20;
    _delay_ms(1000);

    writeString("inside main!");

    /* Setup TCP server for communication */
    //vTcpServerInitialise( 2 );
    
    /*
    struct wiz_NetInfo_t network_config = 
    {
        { tcpMAC },
        { tcpIP },
        { tcpSUBNET },
        { tcpGATEWAY },
        { tcpDNS },
        2
    };

    struct wiz_NetInfo_t temp;

    uint8_t txsize[8] = { 1, 0, 0, 0, 0, 0, 0, 0 };
    uint8_t rxsize[8] = { 1, 0, 0, 0, 0, 0, 0, 0 };

    wizchip_setnetinfo( &network_config );
    wizchip_getnetinfo( &temp );

    wizchip_init(txsize, rxsize);

    BaseType_t status = 0;

    _delay_ms(2000);
    writeNumChar("Version: ", (uint8_t) getVERSIONR(), 10);
    _delay_ms(1000);
    writeNumShort("Retry Count: ", getRTR(), 10);
    _delay_ms(1000);
    writeNumChar("ip[0]: ", temp.ip[0], 10);
    _delay_ms(1000);
    writeNumChar("rx 0 buf size: ", getSn_RXBUF_SIZE(0), 10);
    */

	/* Create the tasks */
	//xTaskCreate( vBlinkyFunction, "Blink", 128, NULL, 3, NULL );
	//status = xTaskCreate( vBlinkyFunction, "TCP", 128, NULL, mainTCP_RX_TASK_PRIORITY, NULL );

    //writeNumChar("Status: ", (uint8_t) status, 10);

	/* In this port, to use preemptive scheduler define configUSE_PREEMPTION
	as 1 in portmacro.h.  To use the cooperative scheduler define
	configUSE_PREEMPTION as 0. */
	//vTaskStartScheduler();

	return 0;
}
/*-----------------------------------------------------------*/

static void vBlinkyFunction( void *pvParameters )
{
	/* The parameters are not used. */
	( void ) pvParameters;

    //const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;
    const TickType_t xDelay = 100;

    for( ;; )
    {
        for(uint8_t r=0; r<2; r++)
        {
            PORTB ^= 0x20;
            _delay_ms(1000);
        }

        vTaskDelay( xDelay );
    }
}

static void vTcpServerLoopback( void *pvParameters )
{
	/* The parameters are not used. */
	( void ) pvParameters;

    uint8_t buf[64];

    for( ;; )
    {
        loopback_tcps( 0, buf, 8080 );
        _delay_ms(500);
        writeNumChar("Socket 0 SR: ", getSn_SR( 0 ), 16);
    }
}

void vApplicationIdleHook( void )
{
    PORTB ^= 0x20;
    _delay_ms(200);
}

