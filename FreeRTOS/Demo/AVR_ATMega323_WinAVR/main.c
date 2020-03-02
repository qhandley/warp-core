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
#include <util/delay.h>

#ifdef GCC_MEGA_AVR
	/* EEPROM routines used only with the WinAVR compiler. */
	#include <avr/eeprom.h>
#endif

#include <avr/io.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* TCP functionality. */
#include "tcp/tcp_server.h"
#include "socket.h"

/* Control task. */
#include "control/control.h"

/* USART functionality. */
#include "usart.h"

//#define _MAIN_DEBUG_

/* Prototypes for tasks defined within this file. */ 
static void vBlinkyTask( void *pvParameters );
BaseType_t xReturned;

/*-----------------------------------------------------------*/
int main( void )
{
    /* Initialize usart for debugging. */
    usart1_init( MYUBRR );

    /* Set built-in LED as output. */
    DDRB |= (1 << PB0);

    char debug_buf[20];

#ifdef _MAIN_DEBUG_
    usart1_tx_str( "Before blinky task\n" );
    _delay_ms(100);
    xTaskCreate( vBlinkyTask, "Blink", 128, NULL, tskIDLE_PRIORITY + 1, NULL );
#else
    /* Setup SPI and TCP server for communication. */
    xReturned = xStartTCPServerTask();
    itoa( (uint8_t) xReturned, debug_buf, 10 ); 
    usart1_tx_str( "Start TCP task: " );
    usart1_tx_str( debug_buf );
    usart1_tx( '\n' );

    /* Setup control task. */
    xReturned = xStartControlTask();
    usart1_tx_str( "Start Control task: " );
    itoa( (uint8_t) xReturned, debug_buf, 10 ); 
    usart1_tx_str( debug_buf );
    usart1_tx( '\n' );
#endif
     
	/* In this port, to use preemptive scheduler define configUSE_PREEMPTION
	as 1 in portmacro.h.  To use the cooperative scheduler define
	configUSE_PREEMPTION as 0. */
	vTaskStartScheduler();

	return 0;
}
/*-----------------------------------------------------------*/

static void vBlinkyTask( void *pvParameters )
{
	/* The parameters are not used. */
	( void ) pvParameters;

    for( ;; )
    {
        /* Toggle LED. */
        PORTB ^= (1<<PB0);
        _delay_ms(1000);
    }
}
