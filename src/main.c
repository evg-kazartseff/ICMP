#include <stdio.h>
#include <getopt.h>
#include <net/if.h>
#include <netinet/in.h>
#include <memory.h>
#include <stdlib.h>
#include "socket.h"
#include "ping.h"

#define T_INIT 0
#define T_PING 1
#define T_TRACEROUTE 2

int main(int argc, char** argv) {
    int type = T_INIT;
    int opt;
    char src_addr[INET_ADDRSTRLEN];
    char dst_addr[INET_ADDRSTRLEN];
    do {
        opt = getopt(argc, argv, "h?ptd:s:");
        switch (opt) {
            case 'p':
                type = T_PING;
                break;
            case 't':
                type = T_TRACEROUTE;
                break;
            case 's':
                strncpy(src_addr, optarg, INET_ADDRSTRLEN);
                break;
            case 'd':
                strncpy(dst_addr, optarg, INET_ADDRSTRLEN);
                break;
            case '?':
            case 'h':
                printf("Use ./ICMP [-s source ip] [-d destination ip] ([-p ping] | [-t traceroute])\n");
                break;
            default:
                type = T_PING;
                break;
        }
    } while (opt != -1);

    int sock;
    if (socket_create(&sock) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }
    ping_t info;
    switch (type) {
        case T_PING:
            info.id = (__be16) rand();
            info.seq = (__be16) rand();
            if (send_ping(sock, dst_addr, src_addr, info) == EXIT_FAILURE) {
                perror("Error");
                break;
            }
            char buff[UINT16_MAX];
            replay_ping(sock, info, buff);
            print_replay((struct icmphdr*) buff);
            break;
        default:
            break;
    }
    close(sock);
    return 0;
}