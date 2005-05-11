#include  <stdio.h>
#include  <stdlib.h>
#include  <SFCClient/SFCClient.h>
#define fatal(s) do{perror(s);fprintf(stderr,s "fatal error %s:%d\n",__FILE__,__LINE__);abort();}while(0)
#define SFCC_DEBUG 
#define HEADER_TO_SKIP  0
#define p_conn_req  0x01
#define p_conn_ack  0x02
#define p_conn_ack_ack  0x03
#define p_data  0x04
#define p_ack  0x05
#define p_decision  0x06
#define p_go  0x07
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
#define bug  if(1) printf
#define info  if(1) printf
#define debug  if(1) printf


struct msg_data {
	u_int32_t len;
	u_int32_t rank;
	u_int32_t number;
	u_int32_t total_len;
};

struct msg_decision {
	u_int32_t number;
	u_int32_t decision;
};

struct msg_header {
	u_int32_t dport;
	u_int32_t sport;
	u_int32_t type;
	u_int32_t key;
	u_int32_t id;
	union {
		struct msg_data data;
		struct msg_decision decision;
	} content;
	char data[0];
};

char *sfc_string;
u_int32_t type;
u_int32_t port;
char *dev_string;
u_int32_t cpu;
u_int32_t level;

int sock;
int r;

static int his_port = 0;

static char packets[100][MTU+HEADER_TO_SKIP];
static char control_packet[MTU+HEADER_TO_SKIP];
static struct iovec vectors[100];
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
static int decision_has_been_sent = 0;
static int next_event_len = 0;

struct sockaddr_ll sfc, who;
int add_len = sizeof(who);

int id;
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

int sfcc_register(const char *eth_string, int myport, int level)
{
        if(level!=SFCC_L1 && level!=SFCC_HLT) return -SFCC_ERR_PARAM;
        debug("sfcc_lib:connecting to host %s for %s...\n",eth_string,level_name[level]);
	sfc_string = eth_string;
	type = protocol[level];
	port = myport;
	dev_string = "eth3";
	cpu = 0;
	
	sock = socket(PF_PACKET,SOCK_DGRAM,type);
	if(sock==-1) fatal("socket");
	
	bzero(&sfc,sizeof(sfc));
	eth_addr = ether_aton(sfc_string); /* dest address */
	if(eth_addr==NULL) fatal("ether_aton");
	#if 0
	memcpy(eth->ether_dhost,eth_addr,6);
	#endif
	strncpy(ifr.ifr_name,dev_string,IFNAMSIZ);
	r = ioctl(sock,SIOCGIFINDEX,&ifr);
	if(r<0) fatal("ioctl");
	sfc.sll_ifindex = ifr.ifr_ifindex;
	r = ioctl(sock,SIOCGIFHWADDR,&ifr);
	if(r<0) fatal("ioctl");
	sfc.sll_family = PF_PACKET;
	sfc.sll_protocol = type;
	sfc.sll_pkttype = PACKET_HOST;
	memcpy(sfc.sll_addr,eth_addr,6);
	sfc.sll_halen = 6;
	#if 0
	memcpy(eth->ether_shost,sfc.sll_addr,6);
	#endif
	r = bind(sock,(struct sockaddr*)&sfc,sizeof(sfc));
	if(r<0) fatal("bind");
	#if 0
	eth->ether_type = type;
	#endif
	
	recv_packet = control_packet;
	recv_msg = (struct msg_header*)&recv_packet[HEADER_TO_SKIP];
	send_msg->type = p_conn_req;
	send_msg->sport = port;
	send_msg->dport = his_port;
	send_len = sendto(sock,send_packet,sizeof(struct msg_header),0,(struct sockaddr*)&sfc,sizeof(sfc));
	if(send_len==-1) fatal("sendto");
	if(send_len!=sizeof(struct msg_header)) {bug("send");abort();}
	for(;;) {
	recv_len = recvfrom(sock,recv_packet,MTU+HEADER_TO_SKIP,0,(struct sockaddr*)&who,&add_len);
	if(recv_len==-1) fatal("recvfrom");
	if(recv_msg->dport == port) break; else {bug("port\n");continue;}
	}
	debug("received %d bytes\n",recv_len);
	if(recv_msg->type != p_conn_ack) bug("type\n");
	if(recv_msg->sport != his_port) bug("port\n");
	id = recv_msg->id;
	send_msg->id = id;
	send_msg->key = recv_msg->key;
	
	debug("send a conack\n");
	send_msg->type = p_conn_ack_ack;
	send_msg->sport = port;
	send_msg->dport = his_port;
	send_len = sendto(sock,send_packet,sizeof(struct msg_header),0,(struct sockaddr*)&who,sizeof(who));
	if(send_len==-1) fatal("sendto");
	if(send_len!=sizeof(struct msg_header)) {bug("send");abort();}
	
	debug("prepare decision\n");
	send_msg->content.decision.decision = 0xab;
	send_msg->content.decision.number = number;
	send_msg->type = p_decision;
	
        if(0) return -SFCC_ERR_CONNECT_FAILED;
        debug("sfcc_lib:connected!\n");
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

	};
	
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
	decision_has_been_sent = 0;
	next_packet = 0;
	rbuf->buffer_len = next_event_len;
        debug("sfcc_lib:next event is %d bytes long\n",next_event_len);
        return 0;
}

int sfcc_set_decision(struct sfcc_decision *dec)
{
        debug("sfcc_lib:setting decision (length %d)\n",dec->buffer_len);
	
	debug("prepare decision\n");
	send_msg->content.decision.decision = 0xab;
	send_msg->content.decision.number = number;
	send_msg->type = p_decision;
        return 0;
}

int sfcc_push_decision(void)
{
	
	debug("send decision");
	send_msg->type = p_decision;
	send_msg->sport = port;
	send_msg->dport = his_port;
	send_len = sendto(sock,send_packet,sizeof(struct msg_header),0,(struct sockaddr*)&who,sizeof(who));
	if(send_len==-1) fatal("sendto");
	if(send_len!=sizeof(struct msg_header)) {bug("send");abort();}
	decision_has_been_sent=1;
	return 0;
}

int sfcc_unregister() { return 0;}

int sfcc_read_length(int *data_length)
{
	if(decision_has_been_sent==0) {
		
		debug("send decision");
		send_msg->type = p_decision;
		send_msg->sport = port;
		send_msg->dport = his_port;
		send_len = sendto(sock,send_packet,sizeof(struct msg_header),0,(struct sockaddr*)&who,sizeof(who));
		if(send_len==-1) fatal("sendto");
		if(send_len!=sizeof(struct msg_header)) {bug("send");abort();}
		decision_has_been_sent=1;
	}
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

