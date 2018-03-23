//
// Created by evgenii on 23.03.18.
//

#ifndef ICMP_SOCKET_H
#define ICMP_SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>

int socket_create(int* sock_d);
uint16_t csum(uint16_t* addr, unsigned short len);

#endif //ICMP_SOCKET_H
