#include <stdio.h>
#include <dis.h>

int dev_data;
int serv_id;

void do_cmnd(tag, cmnd, size)
int *tag, *size;
char *cmnd;
{
	printf("Got Command: %s\n",cmnd);
	if(!strcmp(cmnd,"RESET"))
		dev_data = 0;
	if(!strcmp(cmnd,"CONFIGURE"))
		dev_data = 1;
	if(!strcmp(cmnd,"START"))
		dev_data = 2;
	if(!strcmp(cmnd,"STOP"))
		dev_data = 1;
	dis_update_service(serv_id);
}

main(argc,argv)
int argc;
char **argv;
{
	char aux[80];

	dev_data = 0;
	sprintf(aux,"DEVICE/%s/DATA",argv[1]);
	serv_id = dis_add_service(aux, "I", &dev_data, sizeof(int), 0, 0);

	sprintf(aux,"DEVICE/%s/CMD",argv[1]);
	dis_add_cmnd(aux, "C", do_cmnd, 0);

	dis_start_serving( argv[1] );
	while(1)
		pause();
}
