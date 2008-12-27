#ifndef __SIP_ETHER_H__
#define __SIP_ETHER_H__

#define ETH_P_IP	0x0800		/* Internet Protocol packet	*/
#define ETH_P_ARP	0x0806		/* Address Resolution packet	*/
#define SIP_ETH_ALEN	6		/* Octets in one ethernet addr	 */
#define SIP_ETH_HLEN	14		/* Total octets in header.	 */
#define SIP_ETH_ZLEN	60		/* Min. octets in frame sans FCS */
#define SIPETH_DATA_LEN	1500		/* Max. octets in payload	 */
#define SIP_ETH_FRAME_LEN	1514		/* Max. octets in frame sans FCS */


/*
 *	This is an Ethernet frame header.
 */ 
struct sip_ethhdr {
	unsigned char	h_dest[SIP_ETH_ALEN];	/* destination eth addr	*/
	unsigned char	h_source[SIP_ETH_ALEN];	/* source ether addr	*/
	__be16		h_proto;		/* packet type ID field	*/
} ;
struct sip_sk_buff;
struct net_device {
	char			name[IFNAMSIZ];
	struct sockaddr to;
	/** pointer to next in linked list */
	struct net_device *next;

	/** IP address configuration in network byte order */
	struct in_addr	ip_host;	/* Internet address		*/
	struct in_addr 	ip_netmask;
	struct in_addr 	ip_broadcast;
	struct in_addr 	ip_gw;
	struct in_addr 	ip_dest;
	int		addr_len;
	__u16	type;
	int s;

	/** This function is called by the network device driver
	*  to pass a packet up the TCP/IP stack. */
	__u8 (* input)(struct sip_sk_buff **pskb, struct net_device *dev);
	/** This function is called by the IP module when it wants
	*  to send a packet on the interface. This function typically
	*  first resolves the hardware address, then sends the packet. */
	__u8 (* output)(struct sip_sk_buff **pskb, struct net_device *dev);
	/** This function is called by the ARP module when it wants
	*  to send a packet on the interface. This function outputs
	*  the sip_sk_buff as-is on the link medium. */
	__u8 (* linkoutput)(struct sip_sk_buff **pskb, struct net_device *dev);
	/** This field can be set by the device driver and could point
	*  to state information for the device. */
	void *state;
	/** number of bytes used in hwaddr */
	__u8 hwaddr_len;
	/** link level hardware address of this interface */
	__u8 dev_addr[SIP_ETH_ALEN];
	__u8 broadcast[SIP_ETH_ALEN];
	/** maximum transfer unit (in bytes) */
	__u8 mtu;
	/** flags (see NETIF_FLAG_ above) */
	__u8 flags;
	/** number of this interface */
	__u8 num;
};

#endif /*__SIP_ETHER_H__*/

