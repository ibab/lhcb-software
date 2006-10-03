#include <dic.h>
#include <dis.h>
#include <time.h>

void rout( tag, buf, size )
char *buf;
int *tag, *size;
{
	printf("%s Received for Server %d\n", buf, *tag);
}

main()
{
	char str[80], aux[80];
	int i;

	for(i = 0; i< 20; i++)
	{
		sprintf(str,"TEST_SLAC/SRV%d",i);
		dic_info_service( str, TIMED, 60, 0, 0, rout, i,
			  "No Link", 8 );
	}
	for(i = 0; i< 20; i++)
	{
		sprintf(aux,"TEST_SLAC/CLT%d",i);
		dis_add_service(aux, "C", aux, strlen(aux)+1, (void *)0, 0);
	}
	sprintf(aux,"TEST_SLAC/CLT");
	dis_start_serving(aux);
	while(1)
		pause();
}
