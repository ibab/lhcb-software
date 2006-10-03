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

void cmnd_rout(tag, buf, size)
int *tag, *size;
TT *buf;
{
int i,*ptr;

	printf("Command received, size = %d, TT size = %d:\n", *size,
	       sizeof(TT));
	printf("buf->i = %d, buf->d = %2.2f, buf->s = %d, buf->c = %c, buf->f = %2.2f, buf->str = %s\n",
			buf->i,buf->d,buf->s,buf->c,buf->f,buf->str);
}

void client_exited(tag)
int *tag;
{
	char name[84];
	char *ptr;

	if(dis_get_client(name))
		printf("Client %s (%d) exited\n", name, *tag);
	else
		printf("Client %d exited\n", *tag);
}

/*
#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#include <dic.h>

void timr_rout(int tag)
{
	int code = 2004;
#ifdef WIN32
	DWORD id;
	id = GetCurrentThreadId();
#else
	pthread_t id;
	id = pthread_self();
#endif
	printf("in timr_rout, thread id = %d\n", id);
	dic_cmnd_service("taskManager/sendKill", &code, sizeof(int));
}

void kill_rout(tag, buf, size)
int *tag, *size;
int *buf;
{
#ifdef WIN32
	DWORD id;
	id = GetCurrentThreadId();
#else
	pthread_t id;
	id = pthread_self();
#endif
	printf("Command Kill received = %d\n", *buf);
	printf("thread id = %d\n", id);
	dim_set_priority(3, 1);
}
*/

void exit_cmnd(code)
int *code;
{
/*
#ifdef WIN32
	DWORD id;
	id = GetCurrentThreadId();
#else
	pthread_t id;
	id = pthread_self();
#endif
*/
	printf("Exit_cmnd %d\n", *code);
	exit(*code);
/*
	sleep(5);
	printf("after_sleep, thread id = %d\n", id);
	dim_set_priority(3, 60);
	dtq_start_timer(5, timr_rout, 0);
*/
}

main(argc,argv)
int argc;
char **argv;
{
	int i, j, id, *ptr;
	char aux[80];
	char name[84], *ptrc;

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
	t.f = 4.56;
	ptr = (int *)&t;
	strcpy(t.str,"hello world");
	
	sprintf(aux,"%s/TEST_SWAP",argv[1]);
	id = dis_add_service( aux, "l:3;d:1;s:1;c:1;s:1;f:1;c:20", &t, sizeof(t), 
		(void *)0, 0 );

	sprintf(aux,"%s/TEST_CMD",argv[1]);
	dis_add_cmnd(aux,"l:3;d:1;s:1;c:1;s:1;f:1;c:20",cmnd_rout, 0);

	dis_add_cmnd("taskManager/sendKill","I",kill_rout, 0);
	dis_start_serving( argv[1] );
/*
	{	  
	int prio = -1, ret, pclass = -1;
	dim_get_scheduler_class(&pclass);
	printf("Process class: %d\n",pclass);
	dim_get_priority(1, &prio);
	printf("Main Thread: %d\n",prio);
	dim_get_priority(2, &prio);
	printf("IO Thread: %d\n",prio);
	dim_get_priority(3, &prio);
	printf("Timer Thread: %d\n",prio);
#ifndef WIN32
	ret = dim_set_scheduler_class(2);
	printf("ret = %d\n",ret);
	ret = dim_set_priority(1, 20);
	printf("ret = %d\n",ret);
	ret = dim_set_priority(2, 50);
	printf("ret = %d\n",ret);
#endif
	dim_get_scheduler_class(&pclass);
	printf("Process class: %d\n",pclass);
	dim_get_priority(1, &prio);
	printf("Main Thread: %d\n",prio);
	dim_get_priority(2, &prio);
	printf("IO Thread: %d\n",prio);
	dim_get_priority(3, &prio);
	printf("Timer Thread: %d\n",prio);
	}
*/
	if(dis_get_client(name))
	{
		printf("client %s\n",name);
	}
	
	while(1)
	{
		pause();
	}
/*
	sleep(5);
	{
	  int i;
	  for(i = 0; i <= 999999999; i++);
	}
	printf("Normal Exit\n");
*/
}

