#ifndef UART_32u4_H_
#define UART_32u4_H_

#include <avr/io.h>
#include <stdint.h>

/* Prototypes */
void initUART(void);
uint8_t getByte(void);
void putByte(unsigned char data);
void writeString(char *str);

#endif /* UART_32u4_H_ */
