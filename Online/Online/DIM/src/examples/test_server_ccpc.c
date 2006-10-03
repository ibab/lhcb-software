#include <stdio.h>
#include <dis.h>

int registers[10];
int ids[10];

void set_register(tag, data, size)
int *tag, *data, *size;
{
int index, value;

	index = data[0];
	value = data[1];
	printf("Setting register %d to value %d\n", index, value);
/* here we set the register, read it back and update the service*/
	registers[index] = value;
	dis_update_service(ids[index]);
	
}

main(argc,argv)
int argc;
char **argv;
{
	int i;
	char aux[80];

	for(i = 0; i< 10; i++)
	{
		sprintf(aux,"Register%03d",i);
		ids[i] = dis_add_service( aux, "I", &registers[i], sizeof(int), 
			(void *)0, 0 );
	}

	dis_add_cmnd("SET_REGISTER","I:2",set_register, 0);

	dis_start_serving( "TEST_REGISTERS" );

	while(1) 
	{
		pause();
	}
}

