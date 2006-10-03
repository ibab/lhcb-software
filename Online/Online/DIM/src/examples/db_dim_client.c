// db_dim_client.c : Defines the entry point for the console application.
//
// Dietrich Beck
// 
// This is a very simple DIM service. It listens to a DIM service
// "SERV_BY_BUFFER" via a callback routine. Each time THIS client
// receives a service, it sends a new command to the DIM server.

#include <stdio.h>
#include <dic.h>

#define BUFFSIZE 10000000

int buffer[BUFFSIZE];
int buff[BUFFSIZE];
char message[1024];
int no_link = -1;
int version;
int counter;
int lastValue;
int i;
unsigned service_id;

serv_received(tag,address,size)
int *tag;
char *address;
int *size;
{
    for (i=0;i<BUFFSIZE;i++) buff[i] = buffer[i]; //copy data to do something useful
	counter++;
	sprintf(message, "service received %d\n", counter);
	printf(message);
}

void main(int argc, char* argv[])
{
	
	//service_id =  dic_info_service("testBuffer",MONITORED,0,0,0,serv_received,0,&no_link,4);
//		printf("registering to service\n");
//		counter = 0;
//		service_id =  dic_info_service("testBuffer",MONITORED,0,0,0,serv_received,0,&no_link,4);
	while(1)
	{
		printf("registering to service\n");
		counter = 0;
		service_id =  dic_info_service("testBuffer",MONITORED,0,0,0,serv_received,0,&no_link,4);
		sleep(1);
		printf("releasing service\n");
		dic_release_service(service_id);
		printf("released service\n");
		sleep(1);
		//sprintf(message, "%s", dic_get_error_services());
	}
}

