#ifndef __SIP_UDP_H__
#define __SIP_UDP_H__
struct sip_udphdr {
	__be16	source;
	__be16	dest;
	__u16	len;
	__be16	check;
};

struct ip_pcb 
{
	/* Common members of all PCB types */
	/* ip addresses in network byte order */
	struct in_addr local_ip; 
	struct in_addr remote_ip;
	/* Socket options */  
	__u16 so_options;     
	/* Type Of Service */ 
	__u8 tos;              
	/* Time To Live */     
	__u8 ttl  ;            
	/* link layer address resolution hint */ 
	__u8 addr_hint;
};
struct udp_pcb {
	/* Common members of all PCB types */
	/* ip addresses in network byte order */
	struct in_addr local_ip; 
	struct in_addr remote_ip;
	/* Socket options */  
	__u16 so_options;     
	/* Type Of Service */ 
	__u8 tos;              
	/* Time To Live */     
	__u8 ttl ;             
	/* link layer address resolution hint */ 
	__u8 addr_hint;

	

	/* Protocol specific PCB members */
	struct udp_pcb *next;
	struct udp_pcb *prev;

	__u8 flags;
	/* ports are in host byte order */
	__u16 local_port;
	__u16 remote_port;


#if LWIP_UDPLITE
  	/* used for UDP_LITE only */
  	__u16 chksum_len_rx, chksum_len_tx;
#endif /* LWIP_UDPLITE */
	/* receive callback function
	* addr and port are in same byte order as in the pcb
	* The callback is responsible for freeing the pbuf
	* if it's not used any more.
	*
	* @param arg user supplied argument (udp_pcb.recv_arg)
	* @param pcb the udp_pcb which received data
	* @param p the packet buffer that was received
	* @param addr the remote IP address from which the packet was received
	* @param port the remote port from which the packet was received
	*/
	void (* recv)(void *arg, 
				struct udp_pcb *pcb, 
				struct sip_sk_buff *skb,
				struct in_addr *addr, 
				__u16 port);
	/* user-supplied argument for the recv callback */
	void *recv_arg;  
};

#endif /*__SIP_UDP_H__*/
