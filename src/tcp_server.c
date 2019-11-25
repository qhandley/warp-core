#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "socket.h"
#include "tcp_server.h"
#include "uart.h"

#define MAC         0x44, 0xFF, 0xFF, 0x00, 0x00, 0x00
#define IP          192, 168, 1, 110 
#define SUBNET      255, 255, 255, 0 
#define GATEWAY     192, 168, 1, 1 
#define DNS         8, 8, 8, 8

#pragma message("Wizchip ID: " _WIZCHIP_ID_)

void initSPI()
{
    DDRB |= (1 << PB5) | (1 << PB3) | (1 << PB2); //sck, mosi, ss outputs
    SPCR |= (1 << SPE) | (1 << MSTR);
}

void initTCNT0()
{
    TIMSK0 |= (1 << TOIE0); //enable overflow interrupt 
    TCCR0B |= (1 << CS02) | (1 << CS00); //1024 prescaling
}

void disableTCNT0()
{
    TCCR0B = 0;
}

void enableTCNT0()
{
    TCCR0B |= (1 << CS02) | (1 << CS00); //1024 prescaling
}

ISR(TIMER0_OVF_vect)
{
    uint8_t s0_ir = getSn_IR(0);

    if(!bit_is_clear(s0_ir, 2))
    {
        writeString("recv interrupt received!\r\n");
        setSn_IR(0, (s0_ir & (1 << 2))); //clear interrupt
        PORTB ^= (1 << 0);
    }
}

int32_t loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port)
{
   int32_t ret;
   uint16_t size = 0, sentsize=0;

   switch(getSn_SR(sn))
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(sn) & Sn_IR_CON)
         {
			setSn_IR(sn,Sn_IR_CON);
         }
		 if((size = getSn_RX_RSR(sn)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
         {
			if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
			ret = recv(sn, buf, size);

			if(ret <= 0) return ret;      // check SOCKERR_BUSY & SOCKERR_XXX. For showing the occurrence of SOCKERR_BUSY.
			size = (uint16_t) ret;
			sentsize = 0;

			while(size != sentsize)
			{
                writeNumChar("First: ", *buf, 10);
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
         if((ret = disconnect(sn)) != SOCK_OK) return ret;
         break;
      case SOCK_INIT :
         if( (ret = listen(sn)) != SOCK_OK) return ret;
         break;
      case SOCK_CLOSED:
         if((ret = socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;
      default:
         break;
   }
   return 1;
}

int main()
{
    initUART();
    initSPI();
    initTCNT0();
    DDRB |= (1 << 0);

    uint8_t buf[1000];

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

    writeNumChar("Init return: ", wizchip_init(txsize, rxsize), 10);
    wizchip_setnetinfo(&network_config);
    wizchip_getnetinfo(&temp);

    writeNumChar("ip[0]: ", temp.ip[0], 10);
    writeNumChar("ip[1]: ", temp.ip[1], 10);
    writeNumChar("ip[2]: ", temp.ip[2], 10);
    writeNumChar("ip[3]: ", temp.ip[3], 10);

    writeNumChar("version: ", getVERSIONR(), 16);
    writeNumShort("retry count: ", getRTR(), 10);

    sei(); //enable global interrupts

    while(1)
    {
        disableTCNT0();
        loopback_tcps(0, buf, 8080);
        writeNumChar("Socket 0 SR: ", getSn_SR(0), 16);
        enableTCNT0();
        _delay_ms(250);
    }

    return 0;
}

