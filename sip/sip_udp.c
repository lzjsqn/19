#include "sip.h"
#define UDP_HTABLE_SIZE 128
static struct udp_pcb *udp_pcb_list[UDP_HTABLE_SIZE];

void udp_pcb_init()
{
	int i = 0;
	for(i = 0; i< UDP_HTABLE_SIZE;i++)
	{
		udp_pcb_list[i] = NULL;
	}
}

struct udp_pcb *udp_alloc_pcb(struct net_device *dev,struct sip_sk_buff *skb,
	__be32 saddr,	__be32 daddr,	__be16 sport,	__be16 dport)
{
	struct udp_pcb *upcb = (struct udp_pcb *)malloc(sizeof(struct udp_pcb ));
	if(!upcb){
		return NULL;
	}

	memset(upcb, 0, sizeof(struct udp_pcb ));
	upcb->next = upcb->prev = NULL;
	upcb->local_ip.s_addr = daddr;
	upcb->local_port = dport;
	upcb->remote_ip.s_addr = saddr;
	upcb->remote_port = sport;
	return upcb;
}
struct udp_pcb * udp_found_pcb(struct net_device *dev,struct sip_sk_buff *skb, 
	__be32 saddr,	__be32 daddr,	__be16 sport,	__be16 dport)
{
	struct udp_pcb *upcb, *result = NULL;
	struct sip_udphdr *udph = skb->h.uh;
	int badness = -1;
	int score = 0;
		
	for(upcb = udp_pcb_list[ntohs(dport)&(UDP_HTABLE_SIZE-1)];upcb != NULL; upcb = upcb->next)
	{
		if (upcb->local_port == dport) 
		{
			if (upcb->local_ip.s_addr) 
			{
				if (upcb->local_ip.s_addr!= daddr)
					continue;
				score+=2;
			}
			if (upcb->remote_ip.s_addr)
			{
				if (upcb->remote_ip.s_addr != saddr)
					continue;
				score+=2;
			}
			if (upcb->remote_port)
			{
				if (upcb->remote_port != sport)
					continue;
				score+=2;
			}
			if(score == 6) 
			{
				result = upcb;
				break;
			} 
			else if(score > badness) 
			{
				result = upcb;
				badness = score;
			}
		}
	}
	return result;
}
__u16 found_a_port()
{
	static __u32 index = 0x0;
	index ++;
	return (__u16)(index&0xFFFF);
}
int udp_add_pcb(struct net_device *dev, struct sip_sk_buff *skb,
	__be32 saddr,	__be32 daddr,	__be32 sport,	__be32 dport)
{
	struct udp_pcb * upcb = NULL;
	struct udp_pcb * found = NULL;
	upcb = udp_found_pcb(dev,skb, saddr, daddr, sport, dport);
	if(upcb == NULL)
	{
		if(skb->upcb == NULL)
		{
			upcb = udp_alloc_pcb(dev,skb,saddr, daddr, sport,dport);
			if(upcb == NULL)
			{
				return -1;
			}
			skb->upcb = upcb;
		}
		else
		{
			upcb = skb->upcb;
		}
		if(dport == 0)
		{
			upcb->local_port = htons(found_a_port());
		}
		if(udp_pcb_list[ntohs(dport)&(UDP_HTABLE_SIZE-1)]==NULL){
			udp_pcb_list[ntohs(dport)&(UDP_HTABLE_SIZE-1)] = upcb;
			upcb->next = NULL;
			upcb->prev = NULL;
		}	else{
			if(udp_pcb_list[ntohs(dport)&(UDP_HTABLE_SIZE-1)]->next == NULL)
			{
				upcb->next = udp_pcb_list[ntohs(dport)&(UDP_HTABLE_SIZE-1)];
				udp_pcb_list[ntohs(dport)&(UDP_HTABLE_SIZE-1)]->prev = upcb;
			}else{
				udp_pcb_list[ntohs(dport)&(UDP_HTABLE_SIZE-1)]->next->prev = upcb;
				upcb->next = udp_pcb_list[ntohs(dport)&(UDP_HTABLE_SIZE-1)]->next;
				udp_pcb_list[ntohs(dport)&(UDP_HTABLE_SIZE-1)]=upcb;
			}

		}
		upcb->next = udp_pcb_list[ntohs(dport)&(UDP_HTABLE_SIZE-1)];
		
		
	}else{
		if(dport == 0)
		{
			upcb->local_port = htons(found_a_port());
		}
		if(upcb->local_ip.s_addr== 0)
			upcb->local_ip.s_addr = dev->ip_host.s_addr;
		else
			upcb->local_ip.s_addr = daddr;
		upcb->remote_ip.s_addr = saddr;
		upcb->remote_port = sport;	

		if(skb->upcb)
			free(skb->upcb);
		skb->upcb = upcb;
	}
}
int udp_remove_pcb(struct net_device *dev,struct sip_sk_buff *skb)
{
	struct udp_pcb * upcb = udp_found_pcb(dev,skb,skb->nh.iph->saddr, skb->nh.iph->daddr,skb->h.uh->source,skb->h.uh->dest);
	if(upcb != NULL)
	{
		if(skb->upcb == upcb){
			(upcb->prev)->next = upcb->next;
			upcb->next->prev = upcb->prev;
			upcb->next = NULL;
			upcb->prev = NULL;					
		}
		free(upcb);
	}

	if(skb->upcb)
	{
		free(skb->upcb);
		skb->upcb = NULL;
	}

	return 0;
}

int udp_input(struct net_device *dev, struct sip_sk_buff *skb)
{
	udp_add_pcb(dev,skb,skb->nh.iph->saddr, skb->nh.iph->daddr,skb->h.uh->source,skb->h.uh->dest);

	struct in_addr addr  ;

	addr.s_addr = skb->nh.iph->saddr;
	DBGPRINT(DBG_LEVEL_ERROR, "UDP packet comming, TOT_LEN:%d,FROM %s:%d",skb->tot_len,inet_ntoa(addr), ntohs(skb->h.uh->source));
	addr.s_addr = skb->nh.iph->daddr;
	DBGPRINT(DBG_LEVEL_ERROR, "TO %s:%d",inet_ntoa(addr), ntohs(skb->h.uh->dest));
	DBGPRINT(DBG_LEVEL_ERROR, "content %d bytes:%s\n", ntohs( skb->h.uh->len) - sizeof(struct sip_udphdr),skb->tail);
	sip_free_skb(skb);
}

int udp_output(struct net_device *dev, struct sip_sk_buff *skb)
{
	struct udp_pcb *upcb = skb->upcb;

	udp_add_pcb(dev,
		skb,
		skb->nh.iph->saddr, 
		skb->nh.iph->daddr,
		skb->h.uh->source,
		skb->h.uh->dest);

	ip_output(dev,skb);
}
