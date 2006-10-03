#include <dis.h>

#define MAX_SERVICES 40000
int Data[MAX_SERVICES];

main(argc,argv)
int argc;
char **argv;
{
	int i;
	char name[64];
	int ids[MAX_SERVICES];
/*
	dic_set_dns_node("lxplus059.cern.ch");
*/
  for(i = 0; i< MAX_SERVICES; i++)
	{
	  Data[i] = i;
	  sprintf(name,"%s/Service_%d",argv[1],i);
	  ids[i] = dis_add_service( name, "I", &Data[i], 
			sizeof(Data[i]), (void *)0, 0 );
	}
	dis_start_serving( argv[1] );
	while(1)
	  pause();
}



