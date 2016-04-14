/*************************************************************************
	> File Name: test1.c
	> Author: zzfan
	> Mail: zzfan@mail.ustc.edu.cn 
	> Created Time: Thu 14 Apr 2016 01:39:45 PM HKT
 ************************************************************************/

#include <sys/socket.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/ip6.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <time.h>

#define IPv6_HEAD_LEN 40
#define CACHE_HEAD_CODE 55
#define EX_IP_LEN 8
#define LABEL_LEN 6

unsigned char label[] = {0xff,0xff,0xff,0xff,0xff,0xff};

char msg[] = "hello";

int main()
{
    int pton_fd;
    char src_ip[INET6_ADDRSTRLEN], dst_ip[INET6_ADDRSTRLEN];
    int s,status;
    struct sockaddr_in6 daddr;
    int msg_len = strlen(msg);
    
    //define packet and extension headers
    char *packet;
    int packetLen = IPv6_HEAD_LEN + EX_IP_LEN + msg_len;
    if((packet = (char*)malloc(packetLen)) == NULL){
        printf("packet malloc fail!");
    };
    memset(packet, 0, packetLen);

    //original ip header
    struct ip6_hdr *ip = (struct ip6_hdr *)malloc(sizeof(ip6_hdr));
    memset(ip, '\0', 40);

    //additional header
    u_char pexhdr[8];
    pexhdr[0] = 59;
    pexhdr[1] = 1;
    memcpy(&(pexhdr[2]), label, LABEL_LEN);

    if ((s = socket(AF_INET6, SOCK_RAW, IPPROTO_RAW)) < 0) {
        printf("error creat socket");
    }

    /*
    int setsock_offset = 2;
    if(setsockopt(s, IPPROTO_IPV6, IPV6_CHECKSUM, &setsock_offset, sizeof(setsock_offset) < 0))
    {
       perror("setsockopt");
    }
    */
    
    
    daddr.sin6_family = AF_INET6;
    daddr.sin6_port = 0; /* not needed in SOCK_RAW */
    //daddr.sin6_scope_id = if_nametoindex("eth0");
    
    pton_fd = inet_pton(AF_INET6, "2001:da8:d800:1456:20c::1", (struct in6_addr *)&daddr.sin6_addr.s6_addr);
    
    if (pton_fd == 0)
    {
        printf("Does not contain a character string representing a network address");
    }
    else if (pton_fd < 0)
    {
        printf("error pton_fd");
    }
    
    ip->ip6_flow = htonl ((6<<28) | (0<<20) | 0);
    ip->ip6_plen = htons (8);
    ip->ip6_nxt = CACHE_HEAD_CODE;
    ip->ip6_hops = 64;
    
    
    if ((status = inet_pton(AF_INET6, "fe80::921b:eff:fe03:637d", &(ip->ip6_src))) != 1)
    {
        printf("error inet_pton");
    }

    memcpy(packet, ip, 40);
    memcpy(packet+40, pexhdr, 8);
    memcpy(packet+48, msg, msg_len);
    
    if (sendto(s, (char *)packet, sizeof(packet), 0, 
       (struct sockaddr *)&daddr, (socklen_t)sizeof(daddr)) < 0)
       printf("packet send error:");
    else
        printf("data sent\n");

}
