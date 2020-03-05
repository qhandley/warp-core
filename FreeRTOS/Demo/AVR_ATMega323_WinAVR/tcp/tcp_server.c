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

#define tcpDELAY_TIME           ( ( const TickType_t ) 10 )

/* 
 * TCP server initialization on W5500 chip for IP, MAC, etc. including
 * setup for transmit/receive buffer sizes. 
 */
static void prvTCPServerInit( void );

/*
 * Monitor the status of a TCP server on specified socket (0-7). Returns 1
 * on successful operation and error code otherwise, check W5500 Ethernet
 * library for specific errors.
 */
static BaseType_t prvServerStatus( eSocketNum sn );

/* Holds the state of the TCP server connection. True if connection has been
 established and false if not. */
BaseType_t xServerConnEstablished = pdFALSE;

#ifdef _TCP_DEBUG_
    char debug_buf[20];
#endif

/*------------------------------------------------*/

BaseType_t xStartTCPServerTask( void )
{
    prvTCPServerInit();
    return xTaskCreate( vTCPServerTask, "TCP", 2048, NULL, tcpTCP_SERVER_TASK_PRIORITY, NULL );
}
/*------------------------------------------------*/

static void prvTCPServerInit( void )
{
    taskENTER_CRITICAL();
    {
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

        /* Allocate 16KB for tx and rx buffer for socket 0. */
        uint8_t tx_size[8] = { 16, 0, 0, 0, 0, 0, 0, 0 };
        uint8_t rx_size[8] = { 16, 0, 0, 0, 0, 0, 0, 0 };
        
        /* Initialize network configuration and buffer size. */
        wizchip_init( tx_size, rx_size );
        _delay_ms(10);

        wizchip_setnetinfo( &network_config );
        _delay_ms(10);

#ifdef _TCP_DEBUG_
        struct wiz_NetInfo_t wiz_config;
        wizchip_getnetinfo( &wiz_config );

        usart1_tx_str( "Wizchip configured with:\n" );
        usart1_tx_str( "IP: " );
        for( uint8_t i=0; i<4; i++ )
        {
            itoa( wiz_config.ip[i], debug_buf, 10 );
            usart1_tx_str( debug_buf );
            usart1_tx( ' ' );
        }
        usart1_tx( '\n' );
#endif
    }
    taskEXIT_CRITICAL();
}
/*------------------------------------------------*/

static BaseType_t prvServerStatus( eSocketNum sn )
{
int8_t ret;

    switch( getSn_SR( sn ) )
    {
        /* Socket n connection established with peer - SYN packet received. */
        case SOCK_ESTABLISHED:
            if( getSn_IR( sn ) & Sn_IR_CON )
            {
                usart1_tx_str( "New connection established!\n" );
                /* Clear CON interrupt bit issued from successful connection. */
                setSn_IR( sn, Sn_IR_CON );
            }
            xServerConnEstablished = pdTRUE;
            break;

        /* Socket n received disconnect-request (FIN packet) from connected peer. */
        case SOCK_CLOSE_WAIT:
            if( ( ret = disconnect( sn ) ) != SOCK_OK ) return ret;
            xServerConnEstablished = pdFALSE;
            usart1_tx_str( "Closing socket...\n" );
            break;

        /* Socket n is opened with TCP mode, start listening for peer. */
        case SOCK_INIT:
            if( ( ret = listen( sn ) ) != SOCK_OK ) return ret;
            usart1_tx_str( "Socket initialized... listening for connection\n" );
            break;

         /* Socket n is closed, configure TCP server for socket n. */
        case SOCK_CLOSED:
            if( ( ret = socket( sn, Sn_MR_TCP, tcpPORT, 0x00 ) ) != sn ) return ret;
            usart1_tx_str( "Socket opened.\n" );
            break;

        default:
            break;
    }
    return 1;
} 
/*------------------------------------------------*/

portTASK_FUNCTION( vTCPServerTask, pvParameters )
{
    /* Remove compiler warning. */
    ( void ) pvParameters;

int32_t ret;
uint16_t size = 0, sentsize = 0;
uint8_t tcp_buf[ tcpDATA_BUF_SIZE ];
BaseType_t status;

//int8_t r;
//jsmn_parser p;
//jsmntok_t t[16];

TickType_t xLastWakeTime;

    //jsmn_init(&p);
    xLastWakeTime = xTaskGetTickCount();

    for( ;; )
    {
        status = prvServerStatus( 0 );

        if( xServerConnEstablished == pdTRUE && status == 1 )
        {
            if( ( size = getSn_RX_RSR(0) ) > 0 )
            {
                if(size > tcpDATA_BUF_SIZE) 
                {
                    size = tcpDATA_BUF_SIZE;
                }

                taskENTER_CRITICAL();
                {
                    ret = recv( 0, tcp_buf, size );
                }
                taskEXIT_CRITICAL();

                if( ret <= 0 )
                {
                    /* Handle receive error. */
                }

            #ifdef _TCP_DEBUG_
                usart1_tx_str( "Received: " );
                usart1_tx_str( tcp_buf );
                usart1_tx( '\n' );
            #endif

                /* Parse message received. */
                //r = jsmn_parse(&p, (const char *)buf, sizeof(buf), &t, sizeof(t));
                //json_extract((char *) buf, t, r); 
                
                /* Clear receive bit in socket interrupt register. */ 
                setSn_IR( 0, Sn_IR_RECV );
            }
        }

        vTaskDelayUntil( &xLastWakeTime, tcpDELAY_TIME );
    }
}
