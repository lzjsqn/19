#include "sip.h"

inline int IP_IS_BROADCAST(struct net_device *dev, __be32 ip)
{
	int retval = 1;
	if((ip == IP_ADDR_ANY_VALUE) ||(~ip == IP_ADDR_ANY_VALUE))
	{
		DBGPRINT(DBG_LEVEL_NOTES, "IP is ANY ip\n");
		retval = 1;
		goto EXITip_addr_isbroadcast;
	}else if(ip == dev->ip_host.s_addr)	{
		DBGPRINT(DBG_LEVEL_NOTES, "IP is local ip\n");
		retval = 0;
		goto EXITip_addr_isbroadcast;
	}else if(((ip&dev->ip_netmask.s_addr) 
					== (dev->ip_host.s_addr &dev->ip_netmask.s_addr))
		&& ((ip & ~dev->ip_netmask.s_addr) 
					==(IP_ADDR_BROADCAST_VALUE & ~dev->ip_netmask.s_addr))){
		DBGPRINT(DBG_LEVEL_NOTES, "IP is ANY ip\n");
		retval =1;
		goto EXITip_addr_isbroadcast;
	}else{
		retval = 0;
	}
EXITip_addr_isbroadcast:
	return retval;
}


int ip_input(struct net_device *dev, struct sip_sk_buff *skb)
{
	DBGPRINT(DBG_LEVEL_TRACE,"==>ip_input\n");
	struct sip_iphdr *iph = skb->nh.iph;
	int ret = 0;

	if(iph->daddr != dev->ip_host.s_addr){
		DBGPRINT(DBG_LEVEL_NOTES, "IP address NOT to local\n");
		sip_free_skb( skb);
		return;
	}

	if(cksum(skb->nh.raw, sizeof(struct sip_iphdr)))
	{
		DBGPRINT(DBG_LEVEL_ERROR, "IP check sum error\n");
		sip_free_skb(skb);
		ret = -1;
		goto EXITip_input;
	}
	else
	{
		skb->ip_summed = CHECKSUM_HW;
		DBGPRINT(DBG_LEVEL_NOTES, "IP check sum success\n");
	}
	switch(iph->protocol)
	{
		case SIP_IPPROTO_ICMP:
			skb->h.icmph = (struct sip_icmphdr*)sip_skb_put(skb, sizeof(struct sip_icmphdr));
			icmp_input(dev, skb);
			break;
		case SIP_IPPROTO_UDP:
			skb->h.uh = (struct sip_udphdr*)sip_skb_put(skb, sizeof(struct sip_udphdr));
			udp_input(dev, skb);
			break;
		default:
			break;
	}
EXITip_input:	
	DBGPRINT(DBG_LEVEL_TRACE,"<==ip_input\n");
	return ret;
}

int ip_output(struct net_device *dev, struct sip_sk_buff *skb)
{
	struct sip_iphdr *iph = skb->nh.iph;
	dev->ip_dest.s_addr = iph->saddr;
	iph->check = 0;
	iph->protocol = SIP_IPPROTO_ICMP;
	iph->tos = 0;
	iph->ttl = IP_MAX_TTL;

	__be32 swap = iph->saddr;
	iph->daddr = swap;
	iph->saddr = dev->ip_host.s_addr;
	iph->check = (cksum(skb->nh.raw, sizeof(struct sip_iphdr)));
	if(cksum(skb->nh.raw, sizeof(struct sip_iphdr)))
	{
		DBGPRINT(DBG_LEVEL_ERROR, "ICMP check IP sum error\n");		
	}
	else
	{
		DBGPRINT(DBG_LEVEL_NOTES, "ICMP check IP sum success\n");
	}
	skb->len =skb->tot_len;
	
	dev->output( &skb,dev);
}

