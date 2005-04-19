#include <stdio.h>
#include <stdlib.h>
#include "SFCClient/SFCClient.h"

#define fatal(s) do{fprintf(stderr,s "fatal error %s:%d\n",__FILE__,__LINE__);abort();}while(0)

static FILE *events;
static char *level_name[] = {"L1","HLT"};

int sfcc_register(char *hostname, int level)
{
	if(level!=SFCC_L1 && level!=SFCC_HLT) return SFCC_ERR_PARAM;
#ifdef SFCC_DEBUG
	printf("sfcc_lib:connecting to host %s for %s...\n",hostname,level_name[level]);
#endif
	events = fopen(hostname,"r");
	if(events==NULL) return SFCC_ERR_CONNECT_FAILED;
#ifdef SFCC_DEBUG
	printf("sfcc_lib:connected!\n");
#endif
	return 0;
}

int sfcc_unregister()  
{
  if ( events )  {
    fclose(events);
    events = 0;
  }
  return 0;
}

int sfcc_read_length(int* len)
{
	int r, event_len = 0;
#ifdef SFCC_DEBUG
	printf("sfcc_lib:sending token\n");
	printf("sfcc_lib:waiting for an event\n");
#endif
	r=fread(&event_len,sizeof(event_len),1,events);
	if(r!=1) {
		if(feof(events)) return SFCC_ERR_DISCONNECTED;
		else fatal("read error");
	}	
#ifdef SFCC_DEBUG
	printf("sfcc_lib:next event is %d bytes long\n",event_len);
#endif
  *len = event_len;
  return 0;
}

int sfcc_read(struct sfcc_read_buffer *rbuf)
{
	int r;
  int event_len = rbuf->buffer_len;
	if(rbuf->max_len<event_len) return SFCC_ERR_NO_MEM;
	r=fread(rbuf->buffer,sizeof(char),event_len,events);
	if(r!=event_len) fatal("read error");
	rbuf->buffer_len = event_len;
	return 0;
}

int sfcc_send(struct sfcc_send_buffer *sbuf)
{
#ifdef SFCC_DEBUG
	printf("sfcc_lib:sending decision (length %d)\n",sbuf->buffer_len);
#endif
	return 0;
}

int sfcc_set_decision(struct sfcc_decision *decision)  {
  return 0;
}

int sfcc_send_decision()  {
  return 0;
}
