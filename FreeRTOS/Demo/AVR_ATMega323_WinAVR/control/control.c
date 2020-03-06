/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "control.h"
#include "../usart.h"

QueueHandle_t xControlCmdQueue = NULL;

BaseType_t xStartControlTask( void )
{
    xControlCmdQueue = xQueueCreate( 10, sizeof( BaseType_t ) ); 
    return xTaskCreate( vControlTask, "Ctrl", 512, NULL, ctrlCONTROL_TASK_PRIORITY, NULL ); 
}

portTASK_FUNCTION( vControlTask, pvParameters )
{
    /* Remove compiler warning. */
    ( void ) pvParameters;

BaseType_t *command;

    for( ;; )
    {
        if( xControlCmdQueue != 0 )
        {
            if( xQueueReceive( xControlCmdQueue, &command, 0 ) )
            {
                /* Toggle LED. */
                for (int i = 0; i < command; i++){
                    PORTB ^= (1 << PB0);
                }
            } 
        }
    }
}
