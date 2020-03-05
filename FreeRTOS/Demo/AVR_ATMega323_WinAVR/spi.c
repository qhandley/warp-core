#include <avr/io.h>
#include "spi.h"

#if (__AVR_ATmega1284P__)
const uint8_t __SPI_OUT_SS = 2;
const uint8_t __SPI_WIZ_SS = 3;
const uint8_t __SPI_SD_SS = 4;
const uint8_t __SPI_MOSI = 5;
const uint8_t __SPI_MISO = 6;
const uint8_t __SPI_SCK = 7;
#else
#error Cannot init spi communication (not ATmega1284p)
#endif

void spi_master_init( void )
{
    /* Set MOSI, SCK, and SS as outputs. */
    __SPI_DDR |= (1<<__SPI_MOSI) | (1<<__SPI_SCK) | (1<<__SPI_WIZ_SS);

#if (__AVR_ATmega1284P__)
    __SPI_DDR |= (1<<__SPI_SD_SS) | (1<<__SPI_OUT_SS);
    __SPI_PORT |= (1<<__SPI_OUT_SS);
#endif

    /* Enable SPI and set to master mode. */
    SPCR |= (1<<SPE) | (1<<MSTR);

    /* Enable double speed. */
    SPSR |= (1<<SPI2X); 
}

void spi_write_byte( uint8_t data )
{
    /* Pull SS line low to start transfer. */
    __SPI_PORT &= ~(1<<__SPI_OUT_SS);
    SPDR = data;
    while( !(SPSR & (1<<SPIF)) );
    __SPI_PORT |= (1<<__SPI_OUT_SS);
}

