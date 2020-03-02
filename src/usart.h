#ifndef _USART_H_
#define _USART_H_

#include <avr/io.h>

#define FOSC 20000000 // Clock Speed
#define BAUD 9600
#define MYUBRR (((FOSC / (BAUD * 16UL))) - 1)

/* usart initialization */
void usart_init( unsigned int ubrr);
void usart0_init( unsigned int ubrr);
void usart1_init( unsigned int ubrr);

/* usart transmit */
void usart0_tx( unsigned char data );
void usart1_tx_str( char *data );
void usart1_tx( unsigned char data );

/* usart receive */
unsigned char usart0_rx( void );
unsigned char usart1_rx( void );

#endif /* _USART_H_ */
