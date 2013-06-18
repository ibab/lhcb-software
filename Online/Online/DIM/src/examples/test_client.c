
#include <dic.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

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

void big_rout( int *tag, int *buf, int *size )
{

	if(size){}
	printf("Received %d for TestMem_%d\n", *buf, *tag);
}


void got_servers( int *tag, char *list, int *size)
{
	if(tag){}
	if(size){}
	printf("%s",list);
}

void got_services( int *tag, char *list, int *size)
{
	if(tag){}
	if(size){}
	printf("%s",list);
}
/*
void rout_list( int *tag, char *buf, int *size )
{
		printf("Received list %d: %s\n",*size, buf);
}
*/

void version_rout( int *tag, int *buf, int *size)
{
	if(tag){}
	printf("Received VERSION %lx, %d\n", (dim_long)buf, *size);
}

void rout( tag, buf, size )
char *buf;
int *tag, *size;
{
/*
	char *format;
	format = dic_get_format(0);
	printf("Received format = %s %08x, %d\n",format, format, *size);
*/
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
		memcpy(&t, buf, (size_t)*size);
		printf("t.i = %d, t.d = %2.2f, t.s = %d, t.c = %c, t.f = %2.2f, t.str = %s\n",
			t.i,t.d,t.s,t.c,t.f,t.str);
		dic_get_timestamp(0, &secs, &millis);
		tsecs = secs;
		my_ctime(&tsecs, str, 128);
		str[(int)strlen(str)-1] = '\0';
		printf("timestamp = %s.%d\n",str,millis);

		return;
	}
	else
		printf("%s Received %s for Service%03d\n",client_str,buf,*tag);

/*
	if(conn_id = dic_get_server(server))
		printf("received from %d, %s\n",conn_id, server);
*/

}

int main(int argc, char **argv)
{
	int i;
	char aux[80];
	int id = 123;

/*
	dic_set_debug_on();
*/
/*
	dic_set_dns_node("pclhcb99.cern.ch");
*/
	if(argc){}
	sprintf(str,"%s/SET_EXIT_HANDLER",argv[2]);
	dic_cmnd_service(str, &id, 4);
	dic_get_id(aux);
	printf("%s\n",aux);
	strcpy(client_str,argv[1]);

	for(i = 0; i< 10; i++)
	{
		sprintf(str,"%s/Service_%03d",argv[2],i);
		dic_info_service( str, TIMED, 10, 0, 0, rout, i,
			  "No Link", 8 );
	}
	
	sprintf(aux,"%s/TEST_SWAP",argv[2]);
	dic_info_service_stamped( aux, TIMED, 5, 0, 0, rout, 1200,
			  &no_link, 4 );

	sprintf(str,"%s/VERSION_NUMBER",argv[2]);
	dic_info_service( str, MONITORED, 0, 0, 0, version_rout, 0,
			  NULL, 0 );
/*
	for(i = 0; i < 20; i++)
	{
		sprintf(aux,"%s/TestMem_%d",argv[2], i);
		dic_info_service( aux, MONITORED, 0, 0, 0, big_rout, i,
			  &no_link, 4 );
	}
*/
/*
	sprintf(aux,"DIS_DNS/SERVER_LIST");
	dic_info_service( aux, MONITORED, 0, 0, 0, rout_list, 0,
			  "DEAD", 5 );
	sprintf(aux,"%s/SERVICE_LIST",argv[2]);
	dic_info_service( aux, MONITORED, 0, 0, 0, rout_list, 0,
			  "DEAD", 5 );
*/
/*
	dic_info_service("DIS_DNS/SERVER_LIST",MONITORED, 0, 0, 0, got_servers, 0,
		"not there", 10);
	dic_info_service("xx/SERVICE_LIST",MONITORED, 0, 0, 0, got_services, 0,
		"not there", 10);
*/
	sprintf(aux,"%s/TEST_CMD",argv[2]);
	while(1)
	{
		sleep(10);
		printf("Sending Command, size = %d, i = %d\n",(int)sizeof(t), t.i);
		dic_cmnd_service(aux,&t,(int)sizeof(t));
	}
	return 1;
}
