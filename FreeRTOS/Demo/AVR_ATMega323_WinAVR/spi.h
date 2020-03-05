#ifndef SPI_H
#define SPI_H

#define  __SPI_PORT PORTB
#define  __SPI_DDR  DDRB 

void spi_master_init( void );
void spi_write_byte( uint8_t data );

#endif /* SPI_H */
