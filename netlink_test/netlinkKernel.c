/*************************************************************************
	> File Name: netlinkKernel.c
	> Author: zzfan
	> Mail: zzfan@mail.ustc.edu.cn 
	> Created Time: Tue 05 Apr 2016 07:32:51 PM HKT
 ************************************************************************/

#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
  

#define NETLINK_USER 31

struct sock *nl_sk = NULL;
static DEFINE_MUTEX(nl_mutex);

int recv_num = 0;
const char *msg="Echo from kernel";

static int hello_nl_recv_msg(struct sk_buff *skb, struct nlmsghdr* nlh)
{
    int pid;
    struct sk_buff *skb_out;
    int msg_size;

    int res;
    msg_size=strlen(msg) + 1;
    
    recv_num++;
    //printk(KERN_INFO "Netlink received msg payload: (%d)\n", skb->len);
    pid = nlh->nlmsg_pid; /*pid of sending process */
    
    skb_out = nlmsg_new(msg_size,0);
    
    if(!skb_out)
    {
    
        printk(KERN_ERR "Failed to allocate new skb\n");
        return 0;
    
    } 
    nlh=nlmsg_put(skb_out,0,0,NLMSG_DONE,msg_size,0);  
    NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
    strncpy(nlmsg_data(nlh),msg,msg_size);
    
    res=nlmsg_unicast(nl_sk,skb_out,pid);
    
    if(res<0)
        printk(KERN_INFO "Error while sending bak to user\n");

	return 0;
}

static void hello_nl_rcv(struct sk_buff* skb)
{
	mutex_lock(&nl_mutex);
	netlink_rcv_skb(skb, hello_nl_recv_msg);
	mutex_unlock(&nl_mutex);
}

static int __init hello_init(void) {
    //This is for 3.6 kernels and above.
    static struct netlink_kernel_cfg cfg = {
		.groups = 0,
		.flags = NL_CFG_F_NONROOT_SEND | NL_CFG_F_NONROOT_RECV,
        .input = hello_nl_rcv,
		.cb_mutex = &nl_mutex,
    };
    printk("Entering: %s\n",__FUNCTION__);    

    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    //nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, 0, hello_nl_recv_msg,NULL,THIS_MODULE);
    if(!nl_sk)
    {
    
        printk(KERN_ALERT "Error creating socket.\n");
        return -10;
    
    }
    
    return 0;
}

static void __exit hello_exit(void) {

    printk(KERN_INFO "exiting hello module\n");
    netlink_kernel_release(nl_sk);
}

module_init(hello_init); module_exit(hello_exit);

MODULE_LICENSE("GPL");
