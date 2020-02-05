/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* AVR include files. */
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU           16000000
#include <util/delay.h>

/* TCP include files. */
#include "tcp_server.h"
#include "socket.h"

/* UART include file. */
#include "uart.h"

#define tcpDELAY_TIME           ( ( const TickType_t ) 100 )

/* Static prototypes for methods in this file. */
void vTcpServerInitialise( void );
static int8_t serverStatus( eSocketNum sn, TickType_t *xLastWaitTime );

void vTcpServerInitialise( void )
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

        uint8_t txsize[8] = { 1, 0, 0, 0, 0, 0, 0, 0 };
        uint8_t rxsize[8] = { 1, 0, 0, 0, 0, 0, 0, 0 };
        
        /* Initialize network configuration and buffer size. */
        wizchip_init( txsize, rxsize );
        wizchip_setnetinfo( &network_config );
    }
    portEXIT_CRITICAL();
}

static int8_t serverStatus( eSocketNum sn, TickType_t *xLastWaitTime )
{
int8_t ret;

    switch( getSn_SR(sn) )
    {
        case SOCK_ESTABLISHED:
            if( getSn_IR( 0 ) & Sn_IR_CON )
            {
                setSn_IR( 0, Sn_IR_CON );
            }
            writeString("Socket established\n");
            return 2;
            break;
        case SOCK_CLOSE_WAIT:
            if( ( ret = disconnect( 0 ) ) != SOCK_OK ) return ret;
            writeString("Socket close wait\n");
            break;
        case SOCK_INIT:
            if( ( ret = listen( 0 ) ) != SOCK_OK ) return ret;
            writeString("Socket init... waiting for connection\n");
            vTaskDelayUntil( xLastWaitTime, tcpDELAY_TIME );
            break;
        case SOCK_CLOSED:
            if( ( ret = socket( 0, Sn_MR_TCP, 8080, 0x00 ) ) != 0 ) return ret;
            writeString("Socket closed\n");
            break;
        default:
            break;
    }
    return 1;
} 

portTASK_FUNCTION( vTcpRxTask, pvParameters )
{
TickType_t xLastWaitTime;

    xLastWaitTime = xTaskGetTickCount();

    for( ;; )
    {
		//writeString("looping\n");
        if( serverStatus( 0, &xLastWaitTime ) == 2 )
        {
            /* Check if a recv has occured otherwise block */
            if( !( getSn_IR(0) & Sn_IR_RECV ) ) 
            {
                writeString("Delaying 20 ticks -> no receive\n");
                vTaskDelayUntil( &xLastWaitTime, tcpDELAY_TIME );
                continue;
            }

            /* Read in wiz rx buffer and process commands */
            writeString("Received some data!\n");
        }
    }
}

int32_t loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port)
{
    int32_t ret;
    uint16_t size = 0, sentsize = 0;

    switch(getSn_SR(sn))
    {
        case SOCK_ESTABLISHED:
            if(getSn_IR(sn) & Sn_IR_CON)
            {
                setSn_IR(sn,Sn_IR_CON);
            }
            if((size = getSn_RX_RSR(sn)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
            {
                if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE; // clips size if larger that data buffer
                ret = recv(sn, buf, size);

                if(ret <= 0) return ret;      // check SOCKERR_BUSY & SOCKERR_XXX. For showing the occurrence of SOCKERR_BUSY.
                size = (uint16_t) ret;
                sentsize = 0;

                while(size != sentsize)
                {
                    writeNumChar("First char received: ", *buf, 10);
                    ret = send(sn, buf+sentsize, size-sentsize);
                    if(ret < 0)
                    {
                        close(sn);
                        return ret;
                    }
                    sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
                }
            }
            break;
        case SOCK_CLOSE_WAIT :
            if( (ret = disconnect(sn)) != SOCK_OK) return ret;
            break;
        case SOCK_INIT :
            if( (ret = listen(sn)) != SOCK_OK) return ret;
            break;
        case SOCK_CLOSED:
            if( (ret = socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;
        default:
            break;
    }
    return 1;
}

int32_t tcps(uint8_t sn, uint8_t* buf, uint16_t port)
{
    int32_t ret;
    uint16_t size = 0, sentsize = 0;

    switch(getSn_SR(sn))
    {
        case SOCK_ESTABLISHED:
            if(getSn_IR(sn) & Sn_IR_CON)
            {
                setSn_IR(sn,Sn_IR_CON);
            }
            if((size = getSn_RX_RSR(sn)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
            {
                if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE; // clips size if larger that data buffer
                ret = recv(sn, buf, size);

                if(ret <= 0) return ret;      // check SOCKERR_BUSY & SOCKERR_XXX. For showing the occurrence of SOCKERR_BUSY.
            }
            break;
        case SOCK_CLOSE_WAIT :
            if( (ret = disconnect(sn)) != SOCK_OK) return ret;
            break;
        case SOCK_INIT :
            if( (ret = listen(sn)) != SOCK_OK) return ret;
            break;
        case SOCK_CLOSED:
            if( (ret = socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;
        default:
            break;
    }
    return 1;
}
