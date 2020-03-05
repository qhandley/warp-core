#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define tcpTCP_SERVER_TASK_PRIORITY         ( tskIDLE_PRIORITY + 2 )

#define tcpDATA_BUF_SIZE    ( 128 )

#define tcpMAC              0x44, 0xFF, 0xFF, 0x00, 0x00, 0x00
#define tcpIP               10, 0, 0, 10 
#define tcpSUBNET           255, 255, 255, 0 
#define tcpGATEWAY          10, 0, 0, 1 
#define tcpDNS              8, 8, 8, 8
#define tcpPORT             8080

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


BaseType_t xStartTCPServerTask( void );
portTASK_FUNCTION( vTCPServerTask, pvParameters );

#ifdef __cplusplus
}
#endif

#endif /* TCP_SERVER_H */
