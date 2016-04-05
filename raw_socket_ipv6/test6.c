/*************************************************************************
	> File Name: test.c
	> Author: zzfan
	> Mail: zzfan@mail.ustc.edu.cn 
	> Created Time: Sun 03 Apr 2016 10:52:04 AM HKT
 ************************************************************************/


#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#define __FAVOR_BSD
#include <netinet/udp.h>

#define MAXHOSTNAMELEN 256
#define MSGSIZE 64
#define PORT 42
#define TTL 23
#define MESSAGE "azertyuiopqsdf"
//这个只是为了测试下是否可以制定ipv6地址
#define SOURCE 0

#ifndef SOL_UDP
#define SOL_UDP 17
#endif

static int      verbose_flag, v6_flag = 0;
static char     source_addr[INET6_ADDRSTRLEN];
static int      port = 0;

static char     progname[MAXHOSTNAMELEN + 1];

struct opacket6 {
    struct ip6_hdr  ip;
    struct udphdr   udp;
    char            payload[MSGSIZE];
} __attribute__ ((packed));


void
usage()
{
    printf("Usage: %s hostname\n", progname);
}

char*
text_of(struct sockaddr *address)
{
    char *text = (char *)malloc(INET6_ADDRSTRLEN);
    struct sockaddr_in6 *address_v6;
    if (address->sa_family == AF_INET6) {
        address_v6 = (struct sockaddr_in6 *) address;
        inet_ntop(AF_INET6, &address_v6->sin6_addr, text, INET6_ADDRSTRLEN);
    } else {
        strcpy(text, "Unknown address family");
    }
    return text;
}

int
main(int argc, char **argv)
{
    //前面的定义有好多是多余的，不要问我为什么，乱写的
    char            c;
    char            hostname[MAXHOSTNAMELEN + 1];
    char            message[MSGSIZE];
    int             status;
    struct addrinfo hints_numeric, hints;
    struct addrinfo *result, *source;
    size_t          packetsize, headersize, extraipheadersize;
    struct opacket6 op6;
    struct sockaddr_in6 *sockaddr6;
    void           *packet;
    int             on = 1;
    strncpy(progname, argv[0], MAXHOSTNAMELEN);
    progname[MAXHOSTNAMELEN] = 0;
    int             sd;         /* 描述符 */
    ssize_t         num;
    strcpy(message, MESSAGE);
    strcpy(source_addr, "");
    verbose_flag = 1;
    v6_flag = 1;

    //关于选项的，这个是我抄的～～
    strncpy(hostname, argv[optind], MAXHOSTNAMELEN);
    hostname[MAXHOSTNAMELEN] = 0;

    memset(&hints_numeric, 0, sizeof(hints_numeric));
    hints_numeric.ai_flags = AI_NUMERICHOST;
    hints_numeric.ai_socktype = SOCK_RAW;
    hints_numeric.ai_protocol = IPPROTO_RAW;

    /*指定本机ipv6地址，暂时还不知道*/

    //暂时关了
    #ifdef SOURCE
    if (strcmp(source_addr, "") != 0) {
        source = (addrinfo *)malloc(sizeof(struct addrinfo));
        status = getaddrinfo(source_addr, NULL, &hints_numeric, &source);
        if (status) {
            fprintf(stderr, "Not an IP address: %s\n", source_addr);
            abort();
        }
        else{
            fprintf(stdout, "IP address: %s\n", source_addr);
        }
    } else {
        source = NULL;
    }
    #endif

    memset(&hints, 0, sizeof(hints));
    if (v6_flag) {
        hints.ai_family = AF_INET6;
    } else {
        hints.ai_family = AF_UNSPEC;
    }
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_RAW;
    result = (addrinfo *)malloc(sizeof(struct addrinfo));
    status = getaddrinfo(hostname, NULL, &hints_numeric, &result);
    if (status && status == EAI_NONAME) {       
        status = getaddrinfo(hostname, NULL, &hints, &result);
        if (status) {
            fprintf(stderr, "Nothing found about host name %s\n", hostname);
            abort();
        }
        else{
            fprintf(stdout, "host name %s\n", hostname);
        }
    }

    sd = socket(result->ai_family, SOCK_RAW, IPPROTO_RAW);
    if (sd < 0) {
        fprintf(stderr, "Cannot create raw socket: %s\n", strerror(errno));
        abort();
    }
    else{
        fprintf(stdout, "Connecting to %s...\n", text_of(result->ai_addr));
    }

//关于port的多选项，觉得听麻烦的就没有写多个选项的，自己制定
    if (port == 0) {
        port = PORT;
    }
        sockaddr6 = (struct sockaddr_in6 *) result->ai_addr;
        memset(&op6.ip, '\0', sizeof(op6.ip));
        op6.ip.ip6_vfc = 6 << 4;
        op6.ip.ip6_dst = sockaddr6->sin6_addr;
        if (source == NULL) {
            //不要问我这里为什么是空，因为我也不知道怎么处理
        } else {
            sockaddr6 = (struct sockaddr_in6 *) source->ai_addr;
            op6.ip.ip6_src = sockaddr6->sin6_addr;
        }
        op6.ip.ip6_nxt = SOL_UDP;
        op6.ip.ip6_hlim = TTL;
        headersize = sizeof(op6.ip) + sizeof(op6.udp);
        packetsize = headersize + strlen(message);
        op6.ip.ip6_plen = htons((uint16_t) packetsize);
        memset(&op6.udp, '\0', sizeof(op6.udp));
        op6.udp.uh_dport = htons(port);
        op6.udp.uh_sport = htons(port);
        op6.udp.uh_ulen = htons(strlen(message) + sizeof(op6.udp));
        op6.udp.uh_sum = 0;
        memset(op6.payload, '\0', MSGSIZE);
        strcpy(op6.payload, message);
        packet = &op6;
        extraipheadersize = sizeof(op6.ip);

    num =
        sendto(sd, packet, packetsize + extraipheadersize, 0, result->ai_addr,
               result->ai_addrlen);
    if (num < 0) {
        fprintf(stderr, "Cannot send message:  %s\n", strerror(errno));
        abort();
    }
    else{
        fprintf(stdout, "Sending %i bytes (%i for the headers)...\n",
                (int) packetsize, (int) headersize);
    }
    return (0);
}
