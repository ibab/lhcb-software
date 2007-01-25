#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dis.h>

char str[10][80];

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
int big_buff[1024];
*/

void cmnd_rout(tag, buf, size)
int *tag, *size;
TT *buf;
{

	printf("Command received, size = %d, TT size = %d:\n", *size,
	       sizeof(TT));
	printf("buf->i = %d, buf->d = %2.2f, buf->s = %d, buf->c = %c, buf->f = %2.2f, buf->str = %s\n",
			buf->i,buf->d,buf->s,buf->c,buf->f,buf->str);
}

void client_exited(tag)
int *tag;
{
	char name[84];

	if(dis_get_client(name))
		printf("Client %s (%d) exited\n", name, *tag);
	else
		printf("Client %d exited\n", *tag);
}

void exit_cmnd(code)
int *code;
{
	printf("Exit_cmnd %d\n", *code);
	exit(*code);
}

main(argc,argv)
int argc;
char **argv;
{
	int i, id, *ptr;
	char aux[80];
	char name[84];
	int index = 0;

	dis_add_exit_handler(exit_cmnd);
	dis_add_client_exit_handler(client_exited);
	for(i = 0; i< 10; i++)
	{
		sprintf(str[i],"%s/Service_%03d",argv[1],i);
		dis_add_service( str[i], "C", str[i], strlen(str[i])+1, 
			(void *)0, 0 );
	}
	t.i = 123;
	t.j = 123;
	t.k = 123;
	t.d = 56.78;
	t.s = 12;
	t.t = 12;
	t.c = 'a';
	t.f = (float)4.56;
	ptr = (int *)&t;
	strcpy(t.str,"hello world");

	sprintf(aux,"%s/TEST_SWAP",argv[1]);
	id = dis_add_service( aux, "l:3;d:1;s:1;c:1;s:1;f:1;c:20", &t, sizeof(t), 
		(void *)0, 0 );

	sprintf(aux,"%s/TEST_CMD",argv[1]);
	dis_add_cmnd(aux,"l:3;d:1;s:1;c:1;s:1;f:1;c:20",cmnd_rout, 0);

/*
	big_buff[0] = 1;
	for(i = 0; i < 20; i++)
	{
		sprintf(aux,"%s/TestMem_%d",argv[1], i);
		big_ids[i] = dis_add_service( aux, "I", big_buff, 1024*sizeof(int), 
			(void *)0, 0 );
	}
*/
	dis_start_serving( argv[1] );
	if(dis_get_client(name))
	{
		printf("client %s\n",name);
	}
	while(1)
	{
/*
		for(i = 0; i < 20; i++)
		{
			index++;
			big_buff[0] = index;
			dis_update_service(big_ids[i]);
		}
		sleep(1);
*/
		pause();
	}
}

