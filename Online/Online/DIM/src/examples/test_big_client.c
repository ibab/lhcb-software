#include <dic.h>

#define MAX_SERVICES 40000
/*
int Data[MAX_SERVICES];
int Recvd[MAX_SERVICES];
*/
int no_link = -1;

void rout(tag, buff, size)
int *tag, *size;
int *buff;     
{
static int bad = 0;
int i;
static int n_recvd = 0;

	if(!bad)
	{
		if (*buff == no_link)
		{
/*
			for(i = 0; i < MAX_SERVICES; i++)
				Recvd[i] = 0;
*/
			bad = 1;
			n_recvd = 0;
		}
	}
	if(bad)
	{
		if (*buff != no_link)
		{
/*
			for(i = 0; i < MAX_SERVICES; i++)
				Recvd[i] = 0;
*/
			bad = 0;
			n_recvd = 0;
		}
	}
/*
	if(bad)
	{
		Recvd[*tag] = *buff;
	}
	else
	{
		Recvd[*tag] = (*buff)+1;
	}
*/
	n_recvd++;
/*
	for(i = 0; i <= MAX_SERVICES; i++)
	{
		if(Recvd[i] != 0)
			n_recvd++;
	}
*/
/*
  if(!((*tag) % 1000))
*/

	if(!(n_recvd % 1000))
		printf("Received n = %d, %d = %d\n",n_recvd, *tag, *buff);
}

main(argc,argv)
int argc;
char **argv;
{
	int i;
	char name[64];
	int id = 123;

	dic_set_dns_node("lxplus050.cern.ch");

/*	
	dic_set_debug_on();
*/	
	for(i = 0; i< MAX_SERVICES; i++)
	{
		sprintf(name,"%s/Service_%d",argv[2],i);
		dic_info_service( name, TIMED, 0, 0, 0,
				  rout, i,&no_link, sizeof(no_link) );
	}
	while(1)
	  {
	    pause();
	  }
}
