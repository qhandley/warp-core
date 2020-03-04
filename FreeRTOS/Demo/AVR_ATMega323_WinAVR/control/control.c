/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "control.h"

#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "../usart.h"
#include "../jmsn.h"

#define ctrlCONTROL_TASK_PRIORITY       ( tskIDLE_PRIORITY + 1 )


QueueHandle_t xControlCmdQueue = NULL;

portTASK_FUNCTION_PROTO( vControlTask, pvParameters );

BaseType_t xStartControlTask( void )
{
    xControlCmdQueue = xQueueCreate( 10, sizeof( portBASE_TYPE ) ); 
    return xTaskCreate( vControlTask, "Ctrl", 256, NULL, ctrlCONTROL_TASK_PRIORITY, NULL ); 
}

portTASK_FUNCTION( vControlTask, pvParameters )
{
    /* Remove compiler warning */
    ( void ) pvParameters;

portBASE_TYPE *command;


    for( ;; )
    {
        if( xControlCmdQueue != 0 )
        {
           if( xQueueReceive( xControlCmdQueue, &command, ( TickType_t ) 10 ) )
           {
                PORTB ^= (1 << PB0);
           } 
        }
    }
}
