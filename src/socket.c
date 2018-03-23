//
// Created by evgenii on 23.03.18.
//

#include "socket.h"

int socket_create(int* sock_d) {
    *sock_d = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (*sock_d < 0) return EXIT_FAILURE;
    int on = 1;
    if (setsockopt(*sock_d, SOL_IP, IP_HDRINCL, &on, sizeof(on)) < 0) {
        perror("Error: Setsockopt");
        close(*sock_d);
        return  EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

uint16_t csum(uint16_t* addr, unsigned short len)
{
    int nleft = len;
    uint32_t sum = 0;
    uint16_t *w = addr;
    uint16_t answer = 0;

    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }
    /* mop up an odd byte, if necessary */
    if (nleft == 1) {
        *(unsigned char *) (&answer) = *(unsigned char *) w;
        sum += answer;
    }

    /* add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
    sum += (sum >> 16);     /* add carry */
    answer = (uint16_t) ~sum;     /* truncate to 16 bits */
    return (answer);
}


