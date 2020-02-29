/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "control.h"

#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "../uart_32u4.h"

#define ctrlCONTROL_TASK_PRIORITY       (tskIDLE_PRIORITY + 1)

QueueHandle_t xControlCmdQueue = NULL;

portTASK_FUNCTION_PROTO( vControlTask, pvParameters );

void vStartControlTask( void )
{
   xControlCmdQueue = xQueueCreate( 10, sizeof( portBASE_TYPE ) ); 
   xTaskCreate( vControlTask, "Ctrl", 256, NULL, ctrlCONTROL_TASK_PRIORITY, NULL ); 
}

portTASK_FUNCTION( vControlTask, pvParameters )
{
    /* Remove compiler warning */
    ( void ) pvParameters;

uint8_t i;
portBASE_TYPE *command;
portBASE_TYPE buf[10];

    for( ;; )
    {
        if( xControlCmdQueue != 0 )
        {
           if( xQueueReceive( xControlCmdQueue, &( command ), ( TickType_t ) 20 ) )
           {
                PORTB ^= (1 << PB0);
           } 
        }
    }
}
