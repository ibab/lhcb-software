#define TIMINGS 
#include  <stdio.h>
#include  <stdlib.h>
#include  <assert.h>
#include  <sched.h>
#include  <sys/mman.h>
#define fatal(s) do{perror(s);fprintf(stderr,s "fatal error %s:%d\n",__FILE__,__LINE__);abort();}while(0)
#define SFCC_DEBUG 
#define HEADER_TO_SKIP  0
#define rdtscll(val) do { \
     unsigned int __a,__d; \
     asm volatile("rdtsc" : "=a" (__a), "=d" (__d)); \
     (val) = ((unsigned long long)__a) | (((unsigned long long)__d)<<32); \
} while(0)
#define take_timestamp  rdtscll
#define ENVSTRING(v,s) do{v=getenv(#s);if(v==NULL) {fprintf(stderr,"please define "#s"\n");fatal("getenv");}}while(0)
#define ENVINT(i,s) do{char *v=getenv(#s);if(v==NULL) {fprintf(stderr,"please define "#s"\n");fatal("getenv");}else i=atoi(v);}while(0)
#define p_data  0x01
#define p_ack  0x02
#define p_decision  0x04
#define p_token  0x08
#define p_both  (p_token|p_decision)
#define MTU  4080
#define MAX_EVENT  180000
#include  <net/if.h>
#include  <sys/uio.h>
#include  <string.h>
#include  <netinet/ether.h>
#include  <sys/ioctl.h>
#include  <netinet/in.h> 
#include  <sys/socket.h>
#include  <netpacket/packet.h>
#define bug  if(0) printf
#define info  if(0) printf
#define debug  if(0) printf

#ifdef __LOCAL_DIR__
#include "SFCClient.h"
#else
#include <SFCClient/SFCClient.h>
#endif

struct msg_data {
	u_int32_t len;
	u_int32_t rank;
	u_int32_t number;
	u_int32_t total_len;
};

struct msg_decision {
	u_int32_t decision_bytes[8];
#ifdef TIMINGS
	u_int64_t in1,in2,out,_;
#endif
};

struct msg_header {
	u_int32_t dport;
	u_int32_t sport;
	u_int32_t type;
	u_int32_t index;
	union {
		struct msg_data data;
		struct msg_decision decision;
	} content;
	char data[0];
};

static char *sfc_string;
static u_int32_t type;
static u_int32_t port;
static u_int32_t index;
static char *dev_string;
static u_int32_t cpu;

int sock;
int r;

static int his_port = 0;

static char packets[500][MTU+HEADER_TO_SKIP];
static struct iovec vectors[500];
static int next_packet;
static char *recv_packet = packets[0];
static int recv_len;
static char send_packet[MTU+HEADER_TO_SKIP];
static int send_len;
static int total_recved;
static int number; /* event number */
static int next_rank = 0; /* next packet to receive */
static struct msg_header *recv_msg;
static struct msg_header *send_msg = (struct msg_header*)&send_packet[HEADER_TO_SKIP]; /* skip headers */
#if 0
static struct ether_header *eth = (struct ether_header*)send_packet;
#else
#endif
static struct ether_addr *eth_addr;
static struct ifreq ifr;
static int next_event_len = 0;

struct sockaddr_ll who;
int add_len = sizeof(who);
static char *level_name[] = {"L1","HLT"};
static char *sfcc_error_messages[__SFCC_MAX_ERR];
static char unknown_error[] = "Unknown error code";
static int sfcc_errno = SFCC_OK;

char *sfcc_strerror(int err_code)
{
	if(-err_code>=0 && -err_code<__SFCC_MAX_ERR) return sfcc_error_messages[-err_code];
	else return unknown_error;
}

int sfcc_perror(char *prefix)
{
	return fprintf(stderr,"%s: %s\n",prefix,sfcc_strerror(sfcc_errno));
}

int protocol[] = {0x00f3,0x00f4};

int sfcc_register()
{
	char *eth_string;
	char *my_dev;
	int my_index, myport,my_level;
	ENVSTRING(eth_string,SFCC_SFC_MAC_ADDR);
	ENVSTRING(my_dev,SFCC_DAQ_DEV);
	ENVINT(my_index,SFCC_INDEX);
	ENVINT(myport,SFCC_PORT);
	ENVINT(my_level,SFCC_LEVEL);
        if(my_level!=SFCC_L1 && my_level!=SFCC_HLT) return -SFCC_ERR_PARAM;
        debug("sfcc_lib: SFC is host %s, level is %s, index is %d, device is %s...\n",
		eth_string,level_name[my_level],my_index, my_dev);
	assert(my_level==SFCC_L1||my_level==SFCC_HLT);
	index = my_index;
	type = protocol[my_level];
	
#if 0
	{
	struct sched_param sp;
	int cpu_mask=0;
	#ifndef notdef
	cpu_set_t aff;
	__CPU_ZERO(&aff);
	__CPU_SET(cpu_mask,&aff);
	if(-1==sched_setaffinity(0,&aff))perror("sched_setaffinity");
	#else
	unsigned long int aff=cpu_mask;
	if(-1==sched_setaffinity(0,sizeof(aff),&aff))perror("sched_setaffinity");
	#endif
	sp.sched_priority=sched_get_priority_max(SCHED_FIFO);
	if(sched_setscheduler(0,SCHED_FIFO,&sp)<0)perror("sched_setscheduler");
	}
#endif	
	mlockall(MCL_FUTURE);
	sock = socket(PF_PACKET,SOCK_DGRAM,type);
	if(sock==-1) fatal("socket");
	bzero(&who,sizeof(who));
	sfc_string = eth_string;
	
	who.sll_family = PF_PACKET;
	who.sll_protocol = type;
	who.sll_pkttype = PACKET_HOST;
	who.sll_halen = 6;
	eth_addr = ether_aton(sfc_string); /* dest address */
	if(eth_addr==NULL) fatal("ether_aton");
	memcpy(who.sll_addr,eth_addr,6);
	dev_string = my_dev;
	
	strncpy(ifr.ifr_name,dev_string,IFNAMSIZ);
	{
		int rmem = 1000000;
		int r;
		r=setsockopt(sock,SOL_SOCKET,SO_RCVBUF,&rmem,sizeof(rmem));
		if(r==-1) {perror("setsockopt");abort();}
	}
	r = ioctl(sock,SIOCGIFINDEX,&ifr); if(r<0) fatal("ioctl");
	who.sll_ifindex = ifr.ifr_ifindex;
	r = ioctl(sock,SIOCGIFHWADDR,&ifr); if(r<0) fatal("ioctl");
	r = bind(sock,(struct sockaddr*)&who,sizeof(who));
	if(r<0) fatal("bind");
	port = myport;
	cpu = 0;
	send_msg->index = index;
	send_msg->type = p_token; /* only a token the first time */
        return 0;
}

int sfcc_read_event(struct sfcc_event_buffer *rbuf)
{
        debug("sfcc_lib:sfcc_read_event\n");
	if(next_packet!=1) fatal("sfcc_read_event was not called in proper time");
	while(total_recved!=next_event_len) {
		debug("total_recved=%d,next_event_len=%d\n",
			total_recved,next_event_len);
		recv_packet = packets[next_packet];
		recv_msg = (struct msg_header*)&recv_packet[HEADER_TO_SKIP]; /* skip headers */
		
		for(;;) {
		recv_len = recvfrom(sock,recv_packet,MTU+HEADER_TO_SKIP,0,(struct sockaddr*)&who,&add_len);
		if(recv_len==-1) fatal("recvfrom");
		if(recv_msg->dport == port) break; else {bug("port\n");continue;}
		}
		debug("received %d bytes\n",recv_len);
		if(recv_msg->type != p_data) {bug("type %d\n",recv_msg->type);abort();}
		his_port = recv_msg->sport;
		if(recv_msg->content.data.rank != next_rank) {
				bug("packet loss (%d instead of %d)\n",
					recv_msg->content.data.rank,
					next_rank);
					abort();
				}
		total_recved += recv_msg->content.data.len;
		next_rank++;
		vectors[next_packet].iov_base = recv_msg->data;
		vectors[next_packet].iov_len = recv_msg->content.data.len;
		next_packet++;

	}
	
	{
		int p;
		char *next_byte = rbuf->buffer;
		for(p=0;p<next_packet;p++) {
			debug("copying data of packet %d\n",p);
			memcpy(next_byte,vectors[p].iov_base,vectors[p].iov_len);
			next_byte+=vectors[p].iov_len;
		}
	}
	
	debug("send a ack\n");
	send_msg->type = p_ack;
	send_msg->sport = port;
	send_msg->dport = his_port;
	send_len = sendto(sock,send_packet,sizeof(struct msg_header),0,(struct sockaddr*)&who,sizeof(who));
	if(send_len==-1) fatal("sendto");
	if(send_len!=sizeof(struct msg_header)) {bug("send");abort();}
	send_msg->type = p_token; /* the next message type by default */
	next_packet = 0;
	rbuf->buffer_len = next_event_len;
        debug("sfcc_lib:next event is %d bytes long\n",next_event_len);
	take_timestamp(send_msg->content.decision.in2);
        return 0;
}

int sfcc_set_decision(struct sfcc_decision *dec)
{
        debug("sfcc_lib:setting decision (length %d)\n",dec->buffer_len);
	send_msg->type = p_both;
#if 0
	if(dec->buffer_len!=sizeof(send_msg->content.decision.decision_bytes)) {
#else
	if(0) {
#endif
		printf("decision length is wrong\n");
		return -1;
	}
	memcpy(send_msg->content.decision.decision_bytes,dec->buffer,dec->buffer_len);
        return 0;
}

int sfcc_push_decision(void)
{
	send_msg->type = p_decision;
	
	debug("send decision");
	send_msg->sport = port;
	send_msg->dport = his_port;
	take_timestamp(send_msg->content.decision.out);
	send_len = sendto(sock,send_packet,sizeof(struct msg_header),0,(struct sockaddr*)&who,sizeof(who));
	if(send_len==-1) fatal("sendto");
	if(send_len!=sizeof(struct msg_header)) {bug("send");abort();}
	send_msg->type = p_token; /* the next message type */
	return 0;
}

int sfcc_unregister() { return 0;}

int sfcc_read_length(int *data_length)
{
	
	debug("send decision");
	send_msg->sport = port;
	send_msg->dport = his_port;
	take_timestamp(send_msg->content.decision.out);
	send_len = sendto(sock,send_packet,sizeof(struct msg_header),0,(struct sockaddr*)&who,sizeof(who));
	if(send_len==-1) fatal("sendto");
	if(send_len!=sizeof(struct msg_header)) {bug("send");abort();}
	send_msg->type = p_token;
	if(next_packet==0) {
		next_event_len = 0;
		next_packet=0;
		recv_packet=packets[next_packet];
		recv_msg = (struct msg_header*)&recv_packet[HEADER_TO_SKIP]; /* skip headers */
		total_recved=0;
		next_rank=0;
	}
	else fatal("next_packet!=0\n");
	recv_packet = packets[next_packet];
	recv_msg = (struct msg_header*)&recv_packet[HEADER_TO_SKIP]; /* skip headers */
	
	for(;;) {
	recv_len = recvfrom(sock,recv_packet,MTU+HEADER_TO_SKIP,0,(struct sockaddr*)&who,&add_len);
	if(recv_len==-1) fatal("recvfrom");
	if(recv_msg->dport == port) break; else {bug("port\n");continue;}
	}
	debug("received %d bytes\n",recv_len);
	if(recv_msg->type != p_data) {bug("type %d\n",recv_msg->type);abort();}
	his_port = recv_msg->sport;
	if(recv_msg->content.data.rank != next_rank) {
			bug("packet loss (%d instead of %d)\n",
				recv_msg->content.data.rank,
				next_rank);
				abort();
			}
	total_recved += recv_msg->content.data.len;
	next_rank++;
	take_timestamp(send_msg->content.decision.in1);
	/* a packet has already been received, we know the
		total length of the event, if the buffer is
		too short, we return an error */
	vectors[next_packet].iov_base = recv_msg->data;
	vectors[next_packet].iov_len = recv_msg->content.data.len;
	next_packet++;
	if(next_packet==1) {
		number = recv_msg->content.data.number;
		next_event_len = recv_msg->content.data.total_len;
		debug("new event: %08x, %d\n",number,next_event_len);
	}
	*data_length=next_event_len;
	return 0;
}

