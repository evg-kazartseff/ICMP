//
// Created by evgenii on 23.03.18.
//

#include <linux/icmp.h>
#include "ping.h"

size_t send_ping(int sock, char* dst_address, char* src_address, ping_t info) {
    size_t size_ip_hdr = sizeof(struct iphdr);
    size_t size_icmp_hdr = sizeof(struct icmphdr);
    char *packet = calloc(size_ip_hdr * size_icmp_hdr, sizeof(char));

    struct iphdr *ip_hdr = (struct iphdr*) packet;
    struct icmphdr *icmp_hdr = (struct icmphdr*) (packet + size_ip_hdr);

    ip_hdr->ihl = (unsigned int) (size_ip_hdr >> 2);
    ip_hdr->version = IPVERSION;
    ip_hdr->tos = 0;
    ip_hdr->tot_len = htons((uint16_t) (size_ip_hdr + size_icmp_hdr + sizeof(struct udphdr)));
    srand((unsigned int) time(NULL));
    ip_hdr->id = htons(rand());
    ip_hdr->frag_off = 0;
    ip_hdr->ttl = IPDEFTTL;
    ip_hdr->protocol = IPPROTO_ICMP;
    ip_hdr->check = 0;
    ip_hdr->saddr = inet_addr(src_address);
    ip_hdr->daddr = inet_addr(dst_address);
    ip_hdr->check = csum((uint16_t*) ip_hdr, (unsigned short) size_ip_hdr);

    icmp_hdr->type = ICMP_ECHO;
    icmp_hdr->code = 0;
    memcpy(&icmp_hdr->un, &info, sizeof(info));
    icmp_hdr->checksum = csum((uint16_t*) icmp_hdr, (unsigned short) size_icmp_hdr);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(dst_address);
    size_t nbytes = (size_t) sendto(sock, packet, size_ip_hdr + size_icmp_hdr, 0, (const struct sockaddr*) &addr, sizeof(addr));
    return nbytes;
}

int replay_ping(int sock, ping_t info, char* packet) {
    char buff[UINT16_MAX];
    struct icmphdr* icmp = (struct icmphdr*) (buff + sizeof(struct iphdr));
    struct sockaddr addr;
    socklen_t socklen = sizeof(addr);
    ssize_t nbytes = 0;
    int get = 0;
    struct icmphdr* replay;
    do {
        nbytes = recvfrom(sock, buff, UINT16_MAX, 0, &addr, &socklen);
        if (nbytes <= 0) return EXIT_FAILURE;
        switch (icmp->type) {
            case ICMP_ECHOREPLY:
                if (icmp->un.echo.id == info.id && icmp->un.echo.sequence == info.seq)
                    get = 1;
                break;
            case ICMP_DEST_UNREACH:
                replay = (struct icmphdr*) (((char*) icmp) + sizeof(struct icmphdr) + sizeof(struct iphdr));
                if (replay->un.echo.id == info.id && replay->un.echo.sequence == info.seq)
                    get = 1;
                break;
            case ICMP_TIME_EXCEEDED:
                break;
            default:
                break;
        }
    } while (!get);
    memcpy(packet, buff, (size_t) nbytes);
    return EXIT_SUCCESS;
}

void print_replay(struct icmphdr* icmp) {
    printf("ICMP replay:\n");
    switch (icmp->type) {
        case ICMP_ECHOREPLY:
            printf("%s", "Type: ICMP_ECHOREPLY\n");
            break;
        case ICMP_DEST_UNREACH:
            printf("%s", "Type: ICMP_DEST_UNREACH\n");
            break;
        case ICMP_ECHO:
            printf("%s", "Type: ICMP_ECHO\n");
            break;
        case ICMP_TIME_EXCEEDED:
            printf("%s", "Type: ICMP_TIME_EXCEEDED\n");
            break;
        default:
            break;
    }
    printf("code %d, id %d, seq %d\n", icmp->code, icmp->un.echo.id, icmp->un.echo.sequence);
}
