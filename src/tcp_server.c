#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>
#include <string.h>
#include <math.h>

#include "tcp_server.h"
#include "socket.h"
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE ((( F_CPU / ( USART_BAUDRATE * 16UL))) - 1)

void initSPI()
{
    //DDRB |= (1 << PB5) | (1 << PB3) | (1 << PB2); //sck, mosi, ss outputs
    DDRB |= (1 << PB1) | (1 << PB2) | (1 << PB6); //sck, mosi, ss outputs
    SPCR |= (1 << SPE) | (1 << MSTR);
}

void USART_Init( void )
{
    DDRD |= (1<<PD3);

    unsigned int baud = BAUD_PRESCALE;

    UBRR1H = (unsigned char) (baud>>8);
    UBRR1L = (unsigned char) baud;

    UCSR1B = (1<<RXEN1) | (1<<TXEN1);
    UCSR1C = (1<<USBS1) | (3<<UCSZ10);
}

uint8_t getByte(void)
{
    // Check to see if something was received
    while (!(UCSR1A & _BV(RXC1)));
    return (uint8_t) UDR1;
}

void putByte(unsigned char data)
{
    // Stay here until data buffer is empty
    while (!(UCSR1A & _BV(UDRE1)));
    UDR1 = (unsigned char) data;
}

void writeString(char *str)
{
    while (*str != '\0')
    {
        putByte(*str);
        ++str;
    }
}

int32_t loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port)
{
    int32_t ret;
    uint16_t size = 0, sentsize = 0;

    switch(getSn_SR(sn))
    {
        case SOCK_ESTABLISHED:
            writeString("Established\n");
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
            writeString("Init\n");
            break;
        case SOCK_CLOSED:
            if( (ret = socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;
            writeString("Closed\n");
        default:
            break;
    }
    return 1;
}

int main()
{
    USART_Init();
    initSPI();

    // Built-in LED
    DDRD |= (1 << PD4);

    uint8_t buf[100];
    char buffer[10];

    struct wiz_NetInfo_t network_config = 
    {
        {MAC},
        {IP},
        {SUBNET},
        {GATEWAY},
        {DNS},
        2
    };

    struct wiz_NetInfo_t temp;

    uint8_t txsize[8] = {1, 0, 0, 0, 0, 0, 0, 0};
    uint8_t rxsize[8] = {1, 0, 0, 0, 0, 0, 0, 0};

    //setup delay
    _delay_ms(2000);

    wizchip_init(txsize, rxsize);
    wizchip_setnetinfo(&network_config);
    wizchip_getnetinfo(&temp);

    uint8_t version = 0;
    uint16_t rcr = 0;
    uint8_t phycfgr = 0;

    PORTD |= (1 << PD4);

    while(1)
    {
        version = getVERSIONR();    
        rcr = getRCR();    

        phycfgr = getPHYCFGR();
        itoa(phycfgr, buffer, 16);
        writeString(buffer);

        _delay_ms(50);
        setPHYCFGR(0xF8);

        phycfgr = getPHYCFGR();
        itoa(phycfgr, buffer, 16);
        writeString(buffer);

        //itoa(temp.ip[0], buffer, 10);
        //writeString(buffer);

        int8_t ret = loopback_tcps(0, buf, 8080);

        //itoa(ret, buffer, 10);
        //writeString(buffer);

        _delay_ms(1000);
    }

    return 0;
}

