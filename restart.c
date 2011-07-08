/*
  restart.c - Linux kernel module for remotely rebooting machines
   
  $Id: restart.c,v 1.1.1.1 2005/08/25 16:49:34 pferguso Exp $
  
  $Log: restart.c,v $
  Revision 1.1.1.1  2005/08/25 16:49:34  pferguso
  Initial import

  
*/


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <net/sock.h>
#include <net/protocol.h>

#define IPPROTO_RESTART  222


static struct inet_protocol rs_proto;

static int restart_handler(struct sk_buff *skb);



static int restart_init(void)
{
   int ret;

   printk(KERN_ALERT "Installing restart protocol handler...\n");

   rs_proto.handler = restart_handler;
   rs_proto.err_handler = NULL;

   ret = inet_add_protocol(&rs_proto, IPPROTO_RESTART);
   printk(KERN_ALERT "Return value from add_protocol: %d\n", ret);

   return ret;
}


static void restart_exit(void)
{
   int ret;

   printk(KERN_ALERT "Removing restart protocol handler...\n");

   ret = inet_del_protocol(&rs_proto, IPPROTO_RESTART);
   printk(KERN_ALERT "Return value from del_protocol: %d\n", ret);

}

static int restart_handler(struct sk_buff *skb)
{

   printk(KERN_ALERT "Restarting machine...NOW!\n");
   machine_restart();
   return 0;

}


module_init(restart_init);
module_exit(restart_exit);

MODULE_LICENSE("GPL");
