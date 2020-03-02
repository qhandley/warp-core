#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <stdint.h>

#define MAC             0x44, 0xFF, 0xFF, 0x00, 0x00, 0x00
#define IP              192, 168, 1, 50 
#define SUBNET          255, 255, 255, 0 
#define GATEWAY         192, 168, 1, 1 
#define DNS             8, 8, 8, 8

#define PORT            8080
#define SOCK_TCP        0

#ifndef DATA_BUF_SIZE
    #define DATA_BUF_SIZE          ( 256 )
#endif

int32_t loopback_tcps(uint8_t sn, uint8_t* buf, uint16_t port);

#endif /* _TCP_SERVER_H_ */
