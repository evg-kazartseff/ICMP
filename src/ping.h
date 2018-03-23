//
// Created by evgenii on 23.03.18.
//

#ifndef ICMP_PING_H
#define ICMP_PING_H

#include <linux/icmp.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>
#include <memory.h>
#include "socket.h"

typedef struct {
    __be16 id;
    __be16 seq;
} ping_t;

size_t send_ping(int sock, char* dst_address, char* src_address, ping_t info);
int replay_ping(int sock, ping_t info, char* packet);
void print_replay(struct icmphdr* icmp);

#endif //ICMP_PING_H
