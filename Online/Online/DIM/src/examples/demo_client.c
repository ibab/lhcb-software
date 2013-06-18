#include <dic.h>

int no_link = -1;

void got_data( tag, data, size )
int *data;
int *tag, *size;
{

	if(*data == -1)
		printf("Server is dead\n");
	else
		printf("got data: %d\n",*data);
}

main(argc,argv)
int argc;
char **argv;
{
	char aux[80], cmnd[16];


	sprintf(aux,"DEVICE/%s/DATA",argv[1]);
	dic_info_service( aux, MONITORED, 0, 0, 0, got_data, 0,
			  &no_link, 4 );

	sprintf(aux,"DEVICE/%s/CMD",argv[1]);
	while(1)
	{
		scanf("%s",cmnd);
		printf("Sending Command: %s\n",cmnd);
		dic_cmnd_service(aux,cmnd,(int)strlen(cmnd)+1);
	}
}
