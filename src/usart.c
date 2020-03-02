#include "usart.h"

void usart_init( unsigned int ubrr )
{
    usart0_init( ubrr );
    usart1_init( ubrr );
}

void usart0_init( unsigned int ubrr )
{
	/*Set baud rate */
	UBRR0H = (unsigned char) (ubrr>>8);
	UBRR0L = (unsigned char) ubrr;

	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);

	/* Set frame format: 8-bit data, 2 stop bits */
	UCSR0C = (1<<USBS0) | (1<<UCSZ01) | (1<<UCSZ00);

    /* Double speed mode */
    //UCSR1A = (1<<U2X0);
}

void usart1_init( unsigned int ubrr )
{
	/*Set baud rate */
	UBRR1H = (unsigned char) (ubrr>>8);
	UBRR1L = (unsigned char) ubrr;

	/* Enable receiver and transmitter */
	UCSR1B = (1<<RXEN1) | (1<<TXEN1);

	/* Set frame format: 8-bit data, 2 stop bits */
	UCSR1C = (1<<USBS1) | (1<<UCSZ11) | (1<<UCSZ10);

    /* Double speed mode */
    //UCSR1A = (1<<U2X1);
}

void usart0_tx( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0) ) );

	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void usart1_tx( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR1A & (1<<UDRE1) ) );

	/* Put data into buffer, sends the data */
	UDR1 = data;
}

void usart1_tx_str( char *data )
{
    while( *data != '\0' )
    {
        usart1_tx( *data++ );
    }
}

unsigned char usart0_rx( void )
{
	/* Wait for data to be received */
	while ( !( UCSR0A & (1<<RXC0) ) );

	/* Get and return received data from buffer */
	return UDR0;
}

unsigned char usart1_rx( void )
{
	/* Wait for data to be received */
	while ( !( UCSR1A & (1<<RXC1) ) );

	/* Get and return received data from buffer */
	return UDR1;
}
