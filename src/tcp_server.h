#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define MAC             0x44, 0xFF, 0xFF, 0x00, 0x00, 0x00
#define IP              192, 168, 1, 110 
#define SUBNET          255, 255, 255, 0 
#define GATEWAY         192, 168, 1, 1 
#define DNS             8, 8, 8, 8

#define SOCKET          0
#define PORT            8080

#pragma message("Wizchip ID: " _WIZCHIP_ID_)

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

// TCP server loopback test example
int32_t loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port);

// TCP server
int32_t tcps(uint8_t sn, uint8_t* buf, uint16_t port);

#ifdef __cplusplus
}
#endif

#endif // _TCP_SERVER_H_
