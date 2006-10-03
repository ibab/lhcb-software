#include <dic.h>

int no_link = -1;
int RegisterSet;
int RegisterValue;

void register_callback( tag, data, size )
int *tag, *data, *size;
{
	RegisterValue = *data;
	RegisterSet = *tag;
/* In linux this is not necessary */
#ifdef WIN32
	dim_wake_up();
#endif
}

int set_register_wait(index, value)
int index, value;
{
	int pars[2];
		
	pars[0] = index;
	pars[1] = value;
	RegisterSet = -1;
	dic_cmnd_service("SET_REGISTER",pars,sizeof(pars));
	while(RegisterSet == -1)
	{
		dim_wait();
	}
	if(RegisterSet == index)
		return(RegisterValue);
	return(-1);
}

main(argc,argv)
int argc;
char **argv;
{
	int i, index = 0, value;
	char aux[80];

	for(i = 0; i< 10; i++)
	{
		sprintf(aux,"Register%03d",i);
		dic_info_service( aux, MONIT_ONLY, 0, 0, 0, 
			register_callback, i, &no_link, sizeof(int) );
	}

	while(1)
	{
		value = set_register_wait(index%10, index);
		printf("Register %d: wrote %d, readback = %d\n", 
			index%10, index, value);
		index ++;
		sleep(1);
	}
}
