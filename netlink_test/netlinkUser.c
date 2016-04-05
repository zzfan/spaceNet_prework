/*************************************************************************
	> File Name: netlinkUser.c
	> Author: zzfan
	> Mail: zzfan@mail.ustc.edu.cn 
	> Created Time: Tue 05 Apr 2016 07:33:29 PM HKT
 ************************************************************************/

#include <sys/socket.h>
#include <linux/netlink.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define NETLINK_USER 31

#define MAX_PAYLOAD 1024 /* maximum payload size*/
struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;
struct msghdr msg;
char buf[1024];

int main()
{
sock_fd=socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
if(sock_fd<0)
return -1;

FILE *file = fopen("data", "r");
clock_t start, end;

memset(&src_addr, 0, sizeof(src_addr));
src_addr.nl_family = AF_NETLINK;
    //获得pid
src_addr.nl_pid = getpid(); 

bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));

memset(&dest_addr, 0, sizeof(dest_addr));
memset(&dest_addr, 0, sizeof(dest_addr));
dest_addr.nl_family = AF_NETLINK;
dest_addr.nl_pid = 0; 
dest_addr.nl_groups = 0; /* unicast */

int buf_num = 0;
fread(buf, sizeof(buf[0]), 1024, file);
printf("Sending message to kernel\n");
start = clock();
while(buf_num<1024){
    buf_num++;
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;
    
    strcpy(NLMSG_DATA(nlh), buf);
    
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    
    if(!sendmsg(sock_fd,&msg,0)){
        //这就是我为什么说没有出错，因为一直能发出去而且没报错
        //不要问我为什么，因为我也不知道
        printf("sendbuf full\n");
    }
}
end = clock();
printf("Waiting for message from kernel\n");
double duration = (double)(end - start)/CLOCKS_PER_SEC;
printf("send 1M need: %.0f ms\n", duration*1000);


/* Read message from kernel */
//这里没有写，如果有可以的话可以自己开一个线程来读内核的回复
#if 0
recvmsg(sock_fd, &msg, 0);
printf("Received message payload: %s\n", (char *)NLMSG_DATA(nlh));
close(sock_fd);
#endif
}
