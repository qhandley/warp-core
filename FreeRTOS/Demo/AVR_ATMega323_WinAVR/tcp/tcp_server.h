#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define tcpTCP_SERVER_TASK_PRIORITY         ( tskIDLE_PRIORITY + 2 )

/* TCP server configuration. */
#define tcpDATA_BUF_SIZE    ( 64 )

#define tcpMAC              0x44, 0xFF, 0xFF, 0x00, 0x00, 0x00
#define tcpIP               192, 168, 1, 100 
#define tcpSUBNET           255, 255, 255, 0 
#define tcpGATEWAY          192, 168, 1, 1 
#define tcpDNS              8, 8, 8, 8
#define tcpPORT             8080

/* JSON command parsing configuration. */
#define tcpJSON_CMD_ID      "cmd"
#define tcpNUM_JSON_TOKENS  32 

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
