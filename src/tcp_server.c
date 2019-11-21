#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include "socket.h"
#include "tcp_server.h"
#include "uart.h"

#define MAC         0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00
#define IP          192, 168, 1, 10 
#define SUBNET      255, 255, 255, 0 
#define GATEWAY     192, 168, 1, 1 
#define DNS         8, 8, 8, 8

#if LOOPBACK_MODE == LOOPBACK_MAIN_NOBLOCK
void initSPI()
{
    DDRB |= (1 << PB5) | (1 << PB3) | (1 << PB2); //sck, mosi, ss outputs
    SPCR |= (1 << SPE) | (1 << MSTR); //
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

#endif

int main()
{
    initUART();
    initSPI();

    //uint8_t rw_buffer[2000];
    char buffer1[10];
    char buffer2[10];

    uint8_t ip[] = {192, 168, 1, 10};
    char okay[] = "OKAY";
    char err[] = "ERROR";

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

    uint8_t txsize[8] = {16, 0, 0, 0, 0, 0, 0, 0};
    uint8_t rxsize[8] = {16, 0, 0, 0, 0, 0, 0, 0};

    _delay_ms(5000);

    int8_t init_ret = wizchip_init(txsize, rxsize); //software reset and configure tx/rx buffers

    _delay_ms(2000);

    wizchip_setnetinfo(&network_config);
    wizchip_getnetinfo(&temp);

    //setRTR(0xFFFF); //maximum value

    //int8_t ret = socket(0, Sn_MR_TCP, 8080, 0); //establish socket 0 in TCP mode port 8080 no flags
    setSn_MR(0, 0x01); //TCP
    setSn_PORT(0, 8080); //port
    setSn_CR(0, Sn_CR_OPEN);
    _delay_ms(100);

    //while(getSn_CR(0));

    while(1)
    {
        itoa(init_ret, buffer1, 10);
        writeString(buffer1);
        putByte('\r');
        putByte('\n');

        /*
        itoa(ret, buffer1, 10);
        writeString(buffer1);
        putByte('\r');
        putByte('\n');
        */

        itoa(getSn_SR(0), buffer1, 10);
        writeString(buffer1);
        putByte('\r');
        putByte('\n');

        itoa(getSn_MR(0), buffer1, 16);
        writeString(buffer1);
        putByte('\r');
        putByte('\n');

        itoa(getSn_PORT(0), buffer1, 10);
        writeString(buffer1);
        putByte('\r');
        putByte('\n');

        uint8_t rcr = 8; // getRCR();

        itoa(rcr, buffer1, 10);
        writeString(buffer1);
        putByte('\r');
        putByte('\n');

        itoa(getSn_IR(0), buffer1, 16);
        writeString(buffer1);
        putByte('\r');
        putByte('\n');

        itoa(getIR(), buffer1, 10);
        writeString(buffer1);
        putByte('\r');
        putByte('\n');

        /*
        itoa(temp.mac[0], buffer1, 10);
        writeString(buffer1);
        putByte('\r');
        putByte('\n');

        itoa(temp.ip[0], buffer1, 10);
        writeString(buffer1);
        putByte('\r');
        putByte('\n');

        itoa(temp.sn[0], buffer1, 10);
        writeString(buffer1);
        putByte('\r');
        putByte('\n');

        itoa(temp.gw[0], buffer1, 10);
        writeString(buffer1);
        putByte('\r');
        putByte('\n');

        itoa(temp.dns[0], buffer1, 10);
        writeString(buffer1);
        putByte('\r');
        putByte('\n');
        */

        _delay_ms(1000);
    }

    return 0;
}

