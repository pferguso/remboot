/*
  remboot.c - Client program for remotely rebooting machines
  
  $Id: remboot.c,v 1.1.1.1 2005/08/25 16:49:34 pferguso Exp $
  
  $Log: remboot.c,v $
  Revision 1.1.1.1  2005/08/25 16:49:34  pferguso
  Initial import

  
*/ 

// gcc   -g -O2 -Wall -c remboot.c
// gcc  -g -O2 -Wall   -o remboot  remboot.o -lnet

#define IPPROTO_RESTART   222

#define HOST_NAME_MAX   512

#include <libnet.h>
#include <stdio.h>
#include <unistd.h>


char *valid_hosts[] = {"atgddr.us.oracle.com", 
                       "ap9019pc.us.oracle.com", 
		               "ap9000pc.us.oracle.com", 
		               NULL};


void usage(char *);


int main(int argc, char *argv[])
{
    int c;
    libnet_t *l;
    char *device = "eth0";
    char *dst = NULL;
    char  src[HOST_NAME_MAX+1];
    u_long src_ip, dst_ip;
    char errbuf[LIBNET_ERRBUF_SIZE];
    libnet_ptag_t ip_ptag = 0;
    char *cur_host;
    int   valid;
    u_char payload[255] = {0x11, 0x11, 0x22, 0x22, 0x00, 0x08, 0xc6, 0xa5};
    u_long payload_s = 8;
    int  i;

   
    while ((c = getopt(argc, argv, "d:i:h")) != EOF)
    {
        switch (c)
        {
            case 'd':
		        dst = optarg;
                break;

	        case 'i':
		        device = optarg;
		        break;

	        case 'h':
		        usage(argv[0]);
		        exit(EXIT_SUCCESS);

            default:
                exit(EXIT_FAILURE);
        }
    }


    if (!dst) {
       usage(argv[0]);
       exit(EXIT_FAILURE); 
    }
    
    gethostname(src, HOST_NAME_MAX);
    cur_host = valid_hosts[0];
    for (i=0, valid=0; cur_host; i++) {
      if (strstr(cur_host, src)) {
        valid = 1;
	    break;
      }
      cur_host = valid_hosts[i];
    }
    
    if (!valid) {
      fprintf(stderr, "Uh uh. I don't think so.\n");
      exit(EXIT_FAILURE); 
    }
    
    
    l = libnet_init(
	    LIBNET_RAW4,                            /* injection type */
	    device,                                 /* network interface */
        errbuf);                                /* error buffer */

    

    if (l == NULL)
    {
        fprintf(stderr, "libnet_init() failed: %s", errbuf);
        exit(EXIT_FAILURE); 
    }

    if ((dst_ip = libnet_name2addr4(l, dst, LIBNET_RESOLVE)) == -1)
    {
	  fprintf(stderr, "Bad destination IP address: %s\n", dst);
	  exit(EXIT_FAILURE);
    }
    
    if ((src_ip = libnet_name2addr4(l, src, LIBNET_RESOLVE)) == -1)
    {
	  fprintf(stderr, "Bad source IP address: %s\n", src);
	  exit(EXIT_FAILURE);
    }
    

    
    ip_ptag = libnet_build_ipv4(
        LIBNET_IPV4_H + payload_s,                  /* length */
        0,                                          /* TOS */
        242,                                        /* IP ID */
        0,                                          /* IP Frag */
        64,                                         /* TTL */
        IPPROTO_RESTART,                            /* protocol */
        0,                                          /* checksum */
        src_ip,                                     /* source IP */
        dst_ip,                                     /* destination IP */
        payload,                                    /* payload */
        payload_s,                                  /* payload size */
        l,                                          /* libnet handle */
        ip_ptag);                                   /* libnet id */
        
        
    if (ip_ptag == -1)
    {
        fprintf(stderr, "Can't build IP header: %s\n", libnet_geterror(l));
        goto bad;
    }

   
    c = libnet_write(l);
    if (c == -1)
    {
        fprintf(stderr, "Write error: %s\n", libnet_geterror(l));
        goto bad;
    }
    else
    {
        fprintf(stderr, "Sent restart packet to %s.\n", dst);
    }

    libnet_destroy(l);
    return (EXIT_SUCCESS);
bad:
    libnet_destroy(l);
    return (EXIT_FAILURE);
}

void usage(char *name)
{
    fprintf(stderr, "usage: %s [-i iface] -d destination_ip payload\n", name);
}


