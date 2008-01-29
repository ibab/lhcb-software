#include <dic.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char str[80];
char str_res[10][80];
char client_str[80];
int no_link = -1;
float no_link_float = -1.0;
char buff[80];

typedef struct {
	int i;
	int j;
	int k;
	double d;
	short s;
        char c;
	short t;
	float f;
	char str[20];
}TT;

TT t;

void rout( tag, buf, size )
char *buf;
int *tag, *size;
{
	int conn_id;
	char server[128];

	if(*tag == 11)
	{
		printf("Received ONCE_ONLY : %s\n",buff);
		return;
	}
	if(*tag == 10)
	{
		char node[128], str[256];
		int secs, millis;
		dim_get_dns_node(node);
		printf("DNS node = %s\n",node);
		printf("size = %d\n",*size);
		memcpy(&t, buf, *size);
		printf("t.i = %d, t.d = %2.2f, t.s = %d, t.c = %c, t.f = %2.2f, t.str = %s\n",
			t.i,t.d,t.s,t.c,t.f,t.str);
		dic_get_timestamp(0, &secs, &millis);
		my_ctime((time_t *)&secs, str, 128);
		str[strlen(str)-1] = '\0';
		printf("timestamp = %s.%d\n",str,millis);

		return;
	}
	else
		printf("%s Received 0x%08x for Service%03d\n",client_str,
			*((int *) buf), *tag);
/*
	if(conn_id = dic_get_server(server))
		printf("received from %d, %s\n",conn_id, server);
*/
}
int
main(argc,argv)
int argc;
char **argv;
{
	int i, *ptr;
	char aux[80];
	int id = 123;

	if (argc < 2) {
		fprintf(stderr, "usage: %s <servicename>\n", argv[0]);
		exit(1);
	}
/*
	dic_set_debug_on();
*/
/*
	dim_set_dns_node("pclhcb99.cern.ch");
*/
#if 0
	sprintf(str,"%s/SET_EXIT_HANDLER",argv[2]);
	dic_cmnd_service(str, &id, 4);
#endif
	dic_get_id(aux);
	printf("%s\n",aux);
	strcpy(client_str,argv[1]);

	sprintf(str,"%s",argv[2]);
	dic_info_service( str, TIMED, 10, 0, 0, rout, 0,
			  "No Link", 8 );
#if 0	
	sprintf(aux,"%s/TEST_SWAP",argv[2]);
	dic_info_service_stamped( aux, TIMED, 5, 0, 0, rout, 10,
			  &no_link, 4 );
#endif
#if 0	
	sprintf(aux,"%s/TEST_CMD",argv[2]);
	while(1)
	{
		sleep(10);
		printf("Sending Command, size = %d\n",sizeof(t));
		dic_cmnd_service(aux,&t,sizeof(t));
	}
#endif
	sleep(100);
}
