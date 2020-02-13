/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* AVR include files. */
#define F_CPU       16000000
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* Application include files. */
#include "tcp_server.h"
#include "socket.h"
#include "uart.h"
#include "jsmn.h"

#define tcpDELAY_TIME           ( ( const TickType_t ) 100 )


/* Static prototypes for methods in this file. */
static int8_t serverStatus( eSocketNum sn, TickType_t *xLastWaitTime );

void vTcpServerInit( void )
{
    portENTER_CRITICAL();
    {
        vInitSPI();

        /* Wiznet chip setup time. */
        _delay_ms(2000);
        
        struct wiz_NetInfo_t network_config = 
        {
            { tcpMAC },
            { tcpIP },
            { tcpSUBNET },
            { tcpGATEWAY },
            { tcpDNS },
            2
        };

        /* Allocate 1KB for tx and rx buffer for socket 0 */
        uint8_t txsize[8] = { 1, 0, 0, 0, 0, 0, 0, 0 };
        uint8_t rxsize[8] = { 1, 0, 0, 0, 0, 0, 0, 0 };
        
        /* Initialize network configuration and buffer size. */
        wizchip_setnetinfo( &network_config );
        wizchip_init( txsize, rxsize );
    }
    portEXIT_CRITICAL();
}

static int8_t serverStatus( eSocketNum sn, TickType_t *xLastWaitTime )
{
int8_t ret;

    switch( getSn_SR(sn) )
    {
        /* Socket connection established with peer - SYN packet received */
        case SOCK_ESTABLISHED:
            if( getSn_IR( sn ) & Sn_IR_CON )
            {
                /* Clear CON interrupt bit issued from successful connection */
                setSn_IR( sn, Sn_IR_CON );
            }
            writeString("Socket established on port 8080\n");
            return 2;
            break;

        /* Socket n received disconnect-request (FIN packet) from connected peer */
        case SOCK_CLOSE_WAIT:
            if( ( ret = disconnect( sn ) ) != SOCK_OK ) return ret;
            writeString("Closing socket...\n");
            break;

        /* Socket n is opened with TCP mode, start listening for peer */
        case SOCK_INIT:
            if( ( ret = listen( sn ) ) != SOCK_OK ) return ret;
            writeString("Socket initialized... listening for connection\n");
            vTaskDelayUntil( xLastWaitTime, tcpDELAY_TIME );
            break;

         /* Socket n is closed, configure TCP server for socket n on port 8080 */
        case SOCK_CLOSED:
            if( ( ret = socket( sn, Sn_MR_TCP, 8080, 0x00 ) ) != 0 ) return ret;
            writeString("Socket closed... opening\n");
            break;

        default:
            break;
    }
    return 1;
} 

portTASK_FUNCTION( vTcpRxTask, pvParameters )
{
int32_t ret;
uint16_t size = 0;
uint8_t buf[100];

int8_t r;
jsmn_parser p;
jsmntok_t t;

TickType_t xLastWaitTime;

    jsmn_init(&p);
    xLastWaitTime = xTaskGetTickCount();

    for( ;; )
    {
        if( serverStatus( 0, &xLastWaitTime ) == 2 )
        {
            /* Check if a recv has occured otherwise block */
            if( ( getSn_IR(0) & Sn_IR_RECV ) ) 
            {
                /* Read in wiz rx buffer and process commands */
                writeString("Received some data!\n");

                if((size = getSn_RX_RSR(0)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
                {
                    if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE; // clips size if larger that data buffer
                    ret = recv(0, buf, size);

                    if(ret <= 0) continue;  // check SOCKERR_BUSY & SOCKERR_XXX. For showing the occurrence of SOCKERR_BUSY.

                    r = jsmn_parse(&p, (const char *)buf, sizeof(buf), &t, sizeof(t));
                }
            }
            else
            {
                writeString("Delaying 20 ticks -> no receive\n");
                vTaskDelayUntil( &xLastWaitTime, tcpDELAY_TIME );
            }
        }
    }
}
