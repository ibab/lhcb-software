#include <stdio.h>
#include <dis.h>
#include <dic.h>

char str[10][80];

void rout( tag, buf, size )
char *buf;
int *tag, *size;
{
	printf("%s Received for Server %d\n", buf, *tag);
}

main(argc,argv)
int argc;
char **argv;
{
	char aux[80];
	int n;

	sscanf(argv[1], "%d", &n);
	sprintf(aux,"TEST_SLAC/SRV%d",n);
	sprintf(str[0], aux);
	dis_add_service(aux, "C", str[0], strlen(str[0])+1, (void *)0, 0);
	sprintf(aux,"TEST_SLAC/%d",n);
	dis_start_serving(aux);
	sprintf(aux,"TEST_SLAC/CLT%d",n);
	dic_info_service( aux, TIMED, 60, 0, 0, rout, n,
			  "No Link", 8 );
	while(1)
	{
		pause();
	}
}

