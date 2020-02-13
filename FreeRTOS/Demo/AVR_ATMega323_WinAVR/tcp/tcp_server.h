#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define tcpMAC             0x44, 0xFF, 0xFF, 0x00, 0x00, 0x00
#define tcpIP              192, 168, 1, 110 
#define tcpSUBNET          255, 255, 255, 0 
#define tcpGATEWAY         192, 168, 1, 1 
#define tcpDNS             8, 8, 8, 8
#define tcpPORT            8080

typedef enum
{
    tcpSOCKET0,
    tcpSOCKET1,
    tcpSOCKET2,
    tcpSOCKET3,
    tcpSOCKET4,
    tcpSOCKET5,
    tcpSOCKET6,
    tcpSOCKET7
} eSocketNum;

#define vInitSPI()                                          \
{                                                           \
    DDRB |= (1 << PB5) | (1 << PB3) | (1 << PB2);           \
    SPCR |= (1 << SPE) | (1 << MSTR);                       \
}                                                           

#ifndef DATA_BUF_SIZE
    #define DATA_BUF_SIZE       ( 1024 )
#endif

portTASK_FUNCTION_PROTO( vTcpRxTask, pvParameters );

/* Function prototypes */
void vTcpServerInit( void );

#ifdef __cplusplus
}
#endif

#endif /* TCP_SERVER_H */
