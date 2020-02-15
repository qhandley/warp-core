#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>

#include "uart_32u4.h"

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE ((( F_CPU / ( USART_BAUDRATE * 16UL))) - 1)

/*
void initUART( void )
{
    DDRD |= (1<<PD3);

    unsigned int baud = BAUD_PRESCALE;

    UBRR1H = (unsigned char) (baud>>8);
    UBRR1L = (unsigned char) baud;

    UCSR1B = (1<<RXEN1) | (1<<TXEN1);
    UCSR1C = (1<<USBS1) | (3<<UCSZ10);
}
*/

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
