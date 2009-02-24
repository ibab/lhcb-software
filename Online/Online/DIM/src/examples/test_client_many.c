
#include <dic.h>
#include <time.h>

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
/*
void big_rout( tag, buf, size )
int *buf;
int *tag, *size;
{

	printf("Received %d for TestMem\n", *buf);
}
*/
void rout_many( tag, buf, size )
float *buf;
int *tag, *size;
{
	if(*tag == 50000)
	{
		dim_print_date_time();
		printf("Received %f for service %d\n", *buf, *tag);
	}
}

void rout( tag, buf, size )
char *buf;
int *tag, *size;
{
	int conn_id;
	char server[128];

	if(*tag == 1100)
	{
		printf("Received ONCE_ONLY : %s\n",buff);
		return;
	}
	if(*tag == 1200)
	{
		char node[128], str[256];
		int secs, millis;
		time_t tsecs;

		dic_get_dns_node(node);
		printf("DNS node = %s\n",node);
		printf("size = %d\n",*size);
		memcpy(&t, buf, *size);
		printf("t.i = %d, t.d = %2.2f, t.s = %d, t.c = %c, t.f = %2.2f, t.str = %s\n",
			t.i,t.d,t.s,t.c,t.f,t.str);
		dic_get_timestamp(0, &secs, &millis);
		tsecs = secs;
		my_ctime(&tsecs, str, 128);
		str[strlen(str)-1] = '\0';
		printf("timestamp = %s.%d\n",str,millis);

		return;
	}
	else
		printf("%s Received %s for Service%03d\n",client_str,buf, *tag);

/*
	if(conn_id = dic_get_server(server))
		printf("received from %d, %s\n",conn_id, server);
*/

}

main(argc,argv)
int argc;
char **argv;
{
	int i, *ptr;
	char aux[80];
	int id = 123;

/*
	dic_set_debug_on();
*/
/*
	dic_set_dns_node("pclhcb99.cern.ch");
*/

	sprintf(str,"%s/SET_EXIT_HANDLER",argv[2]);
	dic_cmnd_service(str, &id, 4);
	dic_get_id(aux);
	printf("%s\n",aux);
	strcpy(client_str,argv[1]);
/*
	for(i = 0; i< 10; i++)
	{
		sprintf(str,"%s/Service_%03d",argv[2],i);
		dic_info_service( str, TIMED, 10, 0, 0, rout, i,
			  "No Link", 8 );
	}
	
	sprintf(aux,"%s/TEST_SWAP",argv[2]);
	dic_info_service_stamped( aux, TIMED, 5, 0, 0, rout, 1200,
			  &no_link, 4 );
*/
/*	
	sprintf(aux,"%s/TestMem",argv[2]);
	dic_info_service( aux, MONITORED, 0, 0, 0, big_rout, 0,
			  &no_link, 4 );
*/

	for(i = 0; i< 100000; i++)
	{
		sprintf(aux,"%s/ServiceMany%05d",argv[2],i);
		dic_info_service( aux, MONITORED, 60, 0, 0, rout_many, i,
			  &no_link_float, 4 );
	}

	sprintf(aux,"%s/TEST_CMD",argv[2]);
	while(1)
	{
		int index = 0;
		sleep(10);
/*
		printf("Sending Command, size = %d, i = %d\n",sizeof(t), t.i);
		dic_cmnd_service(aux,&t,sizeof(t));
*/
	}
}
