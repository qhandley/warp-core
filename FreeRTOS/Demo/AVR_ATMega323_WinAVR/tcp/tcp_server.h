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

typedef struct
{
    uint8_t cmd;
} xTcpCmdType_t;

typedef struct
{
    uint8_t data;
} xTcpDataType_t;


#define vInitSPI()                                          \
{                                                           \
    DDRB |= (1 << PB5) | (1 << PB3) | (1 << PB2);           \
    SPCR |= (1 << SPE) | (1 << MSTR);                       \
}                                                           

// #pragma message("Wizchip ID: " _WIZCHIP_ID_)

/* Loopback test debug message printout enable */
#define	_LOOPBACK_DEBUG_

/* DATA_BUF_SIZE define for Loopback example */
#ifndef DATA_BUF_SIZE
    #define DATA_BUF_SIZE          ( 1024 )
#endif

/************************/
/* Select LOOPBACK_MODE */
/************************/
#define LOOPBACK_MAIN_NOBLOCK    0
#define LOOPBACK_MODE   LOOPBACK_MAIN_NOBLOCK

void vTcpServerInitialise( unsigned portBASE_TYPE uxQueueLength );

// TCP server loopback test example
int32_t loopback_tcps( uint8_t sn, uint8_t* buf, uint16_t port );

// TCP server
int32_t tcps( uint8_t sn, uint8_t* buf, uint16_t port );

#ifdef __cplusplus
}
#endif

#endif /* TCP_SERVER_H */
