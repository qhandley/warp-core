/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* AVR include files. */
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* Application include files. */
#include "tcp_server.h"
#include "socket.h"
#include "../usart.h"
#include "../jsmn.h"

#define _TCP_DEBUG_

#ifdef _TCP_DEBUG_
    char debug_buf[20];
#endif

#define tcpDELAY_TIME           ( ( const TickType_t ) 100 )

portBASE_TYPE xServerConnEstablished = pdFALSE;

/* TCP server tasks prototype. */
portTASK_FUNCTION_PROTO( vTCPServerTask, pvParameters );

/* Static prototypes for methods in this file. */
static void vTCPServerInit( void );
static portBASE_TYPE xServerStatus( eSocketNum sn );

BaseType_t xStartTCPServerTask( void )
{
    vTCPServerInit();
    return xTaskCreate( vTCPServerTask, "TCP", 2048, NULL, tcpTCP_SERVER_TASK_PRIORITY, NULL );
}

static void vTCPServerInit( void )
{
    portENTER_CRITICAL();
    {
        INIT_SPI_MASTER();

        /* Wiznet chip setup time. */
        _delay_ms(1000);

        /* Set RST bit in mode register */
        setMR(0x80);

#ifdef _TCP_DEBUG_
        usart1_tx_str( "SPI initialized\n" );
        uint8_t version = getVERSIONR();
        itoa( version, debug_buf, 10 );
        usart1_tx_str( "Version: " );
        usart1_tx_str( debug_buf );
        usart1_tx( '\n' );
#endif
                
        struct wiz_NetInfo_t network_config = 
        {
            { tcpMAC },
            { tcpIP },
            { tcpSUBNET },
            { tcpGATEWAY },
            { tcpDNS },
            1 // static
        };

        /* Allocate 1KB for tx and rx buffer for socket 0. */
        uint8_t txsize[8] = { 1, 0, 0, 0, 0, 0, 0, 0 };
        uint8_t rxsize[8] = { 1, 0, 0, 0, 0, 0, 0, 0 };
        
        /* Initialize network configuration and buffer size. */
        wizchip_init( txsize, rxsize );
        _delay_ms(10);
        wizchip_setnetinfo( &network_config );
        _delay_ms(10);

#ifdef _TCP_DEBUG_
        struct wiz_NetInfo_t temp;
        wizchip_getnetinfo( &temp );

        usart1_tx_str( "Wizchip configured with:\n" );
        usart1_tx_str( "IP: " );
        for(uint8_t i=0; i<4; i++)
        {
            itoa( temp.ip[i], debug_buf, 10 );
            usart1_tx_str( debug_buf );
            usart1_tx( ' ' );
        }
        usart1_tx( '\n' );
#endif
    }
    portEXIT_CRITICAL();
}

static portBASE_TYPE xServerStatus( eSocketNum sn )
{
int8_t ret;

    switch( getSn_SR( sn ) )
    {
        /* Socket connection established with peer - SYN packet received */
        case SOCK_ESTABLISHED:
            if( getSn_IR( sn ) & Sn_IR_CON )
            {
                usart1_tx_str("New connection established!\n");
                /* Clear CON interrupt bit issued from successful connection */
                setSn_IR( sn, Sn_IR_CON );
            }
            xServerConnEstablished = pdTRUE;
            break;

        /* Socket n received disconnect-request (FIN packet) from connected peer */
        case SOCK_CLOSE_WAIT:
            if( ( ret = disconnect( sn ) ) != SOCK_OK ) return ret;
            xServerConnEstablished = pdFALSE;
            usart1_tx_str("Closing socket...\n");
            break;

        /* Socket n is opened with TCP mode, start listening for peer */
        case SOCK_INIT:
            if( ( ret = listen( sn ) ) != SOCK_OK ) return ret;
            usart1_tx_str("Socket initialized... listening for connection\n");
            break;

         /* Socket n is closed, configure TCP server for socket n on port 8080 */
        case SOCK_CLOSED:
            if( ( ret = socket( sn, Sn_MR_TCP, tcpPORT, 0x00 ) ) != sn ) return ret;
            usart1_tx_str("Socket closed... opening\n");
            break;

        default:
            break;
    }
    return 1;
} 

portTASK_FUNCTION( vTCPServerTask, pvParameters )
{
    /* Remove compiler warning */
    ( void ) pvParameters;

int32_t ret;
uint16_t size = 0;
uint8_t buf[ DATA_BUF_SIZE ];
portBASE_TYPE status;

int8_t r;
jsmn_parser p;
jsmntok_t t[16];
TickType_t xLastWakeTime;

    jsmn_init(&p);
    xLastWakeTime = xTaskGetTickCount();

    for( ;; )
    {
        status = xServerStatus( 0 );

        if( xServerConnEstablished == pdTRUE && status == 1 )
        {
            /* Check if a recv has occured otherwise block */
            if( ( getSn_IR(0) & Sn_IR_RECV ) ) 
            {
                if( (size = getSn_RX_RSR(0)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
                {
                    if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE; // clips size if larger that data buffer
                    ret = recv( 0, buf, size);
                    usart1_tx_str(buf);

                    //r = jsmn_parse(&p, (const char *)buf, sizeof(buf), &t, sizeof(t));
                    //json_extract((char *) buf, t, r); 
                }
                setSn_IR( 0, Sn_IR_RECV );
            }
            else
            {
                //usart1_tx_str("No rx... delaying\n");
                //vTaskDelayUntil( &xLastWakeTime, tcpDELAY_TIME );
            }
        }
    }
}
