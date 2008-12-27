#include "sip.h"

struct sip_sk_buff *sip_alloc_skb(unsigned int size)
{
	DBGPRINT(DBG_LEVEL_MOMO,"==>sip_alloc_skb\n");
	struct sip_sk_buff *skb = (struct sip_sk_buff*)malloc(sizeof(struct sip_sk_buff));
	if(!skb){
		DBGPRINT(DBG_LEVEL_ERROR,"Malloc skb error\n");
		goto EXITsip_alloc_skb;
	}
	memset(skb, 0, sizeof(struct sip_sk_buff));

	size = SKB_DATA_ALIGN(size);
	skb->head = (__u8*)malloc(size);
	if(!skb->head)
	{
		DBGPRINT(DBG_LEVEL_ERROR,"Malloc data error\n");
		free(skb);
		goto EXITsip_alloc_skb;
	}
	memset(skb->head, 0, size);
	skb->upcb = NULL;

	

	skb->end = skb->head + size -1;
	skb->data = skb->head;
	skb->tail = skb->data;
	skb->next = NULL;
	skb->tot_len = size;
	skb->len = size;
	DBGPRINT(DBG_LEVEL_MOMO,"<==sip_alloc_skb\n");
	return skb;
EXITsip_alloc_skb:
	return NULL;
}

void sip_free_skb(struct sip_sk_buff *skb)
{
	if(skb){
		if(skb->head)
			free(skb->head);
		free(skb);
	}
}

unsigned char *sip_skb_put(struct sip_sk_buff *skb, unsigned int len)
{
	DBGPRINT(DBG_LEVEL_MOMO,"==>sip_skb_put\n");
	unsigned char *tmp = skb->tail;
	skb->tail += len;
	//skb->len  += len;
	//skb->tot_len += len;

	DBGPRINT(DBG_LEVEL_MOMO,"<==sip_skb_put\n");
	return tmp;
}
#if 0
/* CRC16У��ͼ���icmp_cksum
������
	data:����
	len:���ݳ���
����ֵ��
	��������short����
*/
unsigned short cksum(unsigned char *data,  int len)
{
       int sum=0;/* ������ */
	int odd = len & 0x01;/*�Ƿ�Ϊ����*/

	unsigned short *value = (unsigned short*)data;
	/*�����ݰ���2�ֽ�Ϊ��λ�ۼ�����*/
       while( len & 0xfffe)  {
              sum += *(unsigned short*)data;
		data += 2;
		len -=2;
       }
	/*�ж��Ƿ�Ϊ���������ݣ���ICMP��ͷΪ�������ֽڣ���ʣ�����һ�ֽڡ�*/
       if( odd) {
		unsigned short tmp = ((*data)<<8)&0xff00;
              sum += tmp;
       }
       sum = (sum >>16) + (sum & 0xffff);/* �ߵ�λ��� */
       sum += (sum >>16) ;		/* �����λ���� */
       
       return ~sum; /* ����ȡ��ֵ */
}
#else
static __u16
chksum(void *dataptr, __u16 len)
{
  __u32 acc;
  __u16 src;
  __u8 *octetptr;

  acc = 0;
  /* dataptr may be at odd or even addresses */
  octetptr = (__u8*)dataptr;
  while (len > 1)
  {
    /* declare first octet as most significant
       thus assume network order, ignoring host order */
    src = (*octetptr) << 8;
    octetptr++;
    /* declare second octet as least significant */
    src |= (*octetptr);
    octetptr++;
    acc += src;
    len -= 2;
  }
  if (len > 0)
  {
    /* accumulate remaining octet */
    src = (*octetptr) << 8;
    acc += src;
  }
  /* add deferred carry bits */
  acc = (acc >> 16) + (acc & 0x0000ffffUL);
  if ((acc & 0xffff0000) != 0) {
    acc = (acc >> 16) + (acc & 0x0000ffffUL);
  }
  /* This maybe a little confusing: reorder sum using htons()
     instead of ntohs() since it has a little less call overhead.
     The caller must invert bits for Internet sum ! */
  return htons((__u16)acc);
}

__u16 cksum(void *dataptr, __u16 len)
{
  __u32 acc;

  acc = chksum(dataptr, len);
  while ((acc >> 16) != 0) {
    acc = (acc & 0xffff) + (acc >> 16);
  }
  return (__u16)~(acc & 0xffff);
}
#endif

