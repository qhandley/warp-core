#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

#include "tcp_server.h"
#include "socket.h"
#include "usart.h"

void spi_master_init( void )
{
    DDRB |= (1 << PB4) | (1 << PB5) | (1 << PB7); // ss, mosi, sck
    DDRB |= (1 << PB3); // wiznet ss 
    SPCR |= (1 << SPE) | (1 << MSTR); // spi enable, master mode
    SPSR |= (1 << SPI2X); // double-speed
}

int32_t loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port)
{
    int32_t ret;
    uint16_t size = 0, sentsize = 0;

    switch(getSn_SR(sn))
    {
        case SOCK_ESTABLISHED:
            usart1_tx_str( "Connection established!\n" );
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

                usart1_tx_str((char *)buf);

                sentsize = 0;

                while(size != sentsize)
                {
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
            usart1_tx_str( "Socket closed.\n" );
            break;
        case SOCK_INIT :
            if( (ret = listen(sn)) != SOCK_OK) return ret;
            usart1_tx_str( "Socket listening...\n" );
            break;
        case SOCK_CLOSED:
            if( (ret = socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;
            usart1_tx_str( "Socket opened.\n" );
        default:
            break;
    }
    return 1;
}

int main()
{
    /* Init usart1 for debugging */
    usart1_init( MYUBRR );

    /* Init spi for wizchip */
    spi_master_init();

    /* Built-in LED */
    DDRB |= (1 << PB0);

    char buf[20];
    uint8_t tcp_buf[ DATA_BUF_SIZE ];

    struct wiz_NetInfo_t network_config = 
    {
        {MAC},
        {IP},
        {SUBNET},
        {GATEWAY},
        {DNS},
        1
    };

    struct wiz_NetInfo_t temp;

    uint8_t txsize[8] = {1, 0, 0, 0, 0, 0, 0, 0};
    uint8_t rxsize[8] = {1, 0, 0, 0, 0, 0, 0, 0};

    //setup delay
    _delay_ms(500);

    wizchip_init(txsize, rxsize);
    _delay_ms(10);

    wizchip_setnetinfo(&network_config);
    _delay_ms(10);

    wizchip_getnetinfo(&temp);
    _delay_ms(10);

    usart1_tx_str( "IP: " );
    for(uint8_t i=0; i<4; i++)
    {
        itoa( temp.ip[i], buf, 10 );
        usart1_tx_str( buf );
        usart1_tx(' ');
    }
    usart1_tx('\n');

    uint8_t version = 0;
    uint16_t rtr = 0;

    while( 1 )
    {
        /* Toggle LED */
        PORTB ^= (1<<PB0);

        /*
        version = getVERSIONR();    
        itoa( version, buf, 16 );
        usart1_tx_str( "Version: " );
        usart1_tx_str( buf );
        usart1_tx('\n');

        rtr = getRTR();    
        itoa( rtr, buf, 10 );
        usart1_tx_str( "RTR: " );
        usart1_tx_str( buf );
        usart1_tx('\n');
        */

        int8_t ret = loopback_tcps(SOCK_TCP, tcp_buf, PORT);

        _delay_ms(100);
    }

    return 0;
}

