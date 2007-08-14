#include <signal.h>
#define DIMLIB
#include "dim.h"

#ifndef WIN32

#ifndef NOTHREADS
#include <pthread.h>
#include <semaphore.h>
#ifdef solaris
#include <synch.h>
#endif
#ifdef darwin
#include <sys/types.h>
#include <sys/stat.h>
#endif

pthread_t IO_thread = 0;
pthread_t ALRM_thread = 0;
pthread_t INIT_thread = 0;
pthread_t MAIN_thread = 0;
#ifndef darwin
sem_t DIM_INIT_Sema;
sem_t DIM_WAIT_Sema;
#else
sem_t *DIM_INIT_Semap;
sem_t *DIM_WAIT_Semap;
#endif
int INIT_count = 0;
int WAIT_count = 0;
int DIM_THR_init_done = 0;

void *dim_tcpip_thread(void *tag)
{
	extern int dim_tcpip_init();
	extern void tcpip_task();
	/*	
	int prio;
		
	thr_getprio(thr_self(),&prio);
	thr_setprio(thr_self(),prio+10);
	*/
	IO_thread = pthread_self();

	dim_tcpip_init(1);
	while(1)
    {
		if(INIT_thread)
#ifndef darwin
			sem_post(&DIM_INIT_Sema);
#else
			sem_post(DIM_INIT_Semap);
#endif
		tcpip_task();
#ifndef darwin
		sem_post(&DIM_WAIT_Sema);
#else
		sem_post(DIM_WAIT_Semap);
#endif
    }
}

void *dim_dtq_thread(void *tag)
{
	extern int dim_dtq_init();
	extern int dtq_task();
	/*
	int prio;

	thr_getprio(thr_self(),&prio);
	thr_setprio(thr_self(),prio+5);
	*/
	ALRM_thread = pthread_self();

	dim_dtq_init(1);
	while(1)
	  {
		if(INIT_thread)
		  {
#ifndef darwin
			sem_post(&DIM_INIT_Sema);
#else
			sem_post(DIM_INIT_Semap);
#endif
		  }
		dtq_task();
#ifndef darwin
		sem_post(&DIM_WAIT_Sema);
#else
		sem_post(DIM_WAIT_Semap);
#endif
    }
}

void dim_init()
{
	pthread_t t_id;
	void ignore_sigpipe();
	int ret;
	extern int dna_init();
/*
#ifdef LYNXOS
*/
    pthread_attr_t attr;
/*
#endif
*/
	if(!DIM_THR_init_done)
	{
	  /*
		int prio;
	  */
		DIM_THR_init_done = 1;
		dna_init();
		/*
		thr_getprio(thr_self(),&prio);
		thr_setprio(thr_self(),prio+3);
		*/
		INIT_thread = pthread_self();
		MAIN_thread = INIT_thread;
		
#ifndef darwin 
		
		sem_init(&DIM_INIT_Sema, 0, INIT_count);
		sem_init(&DIM_WAIT_Sema, 0, WAIT_count);
#else
		DIM_INIT_Semap = sem_open("/Dim_INIT_Sem", O_CREAT, S_IRUSR | S_IWUSR, INIT_count);
		DIM_WAIT_Semap = sem_open("/Dim_WAIT_Sem", O_CREAT, S_IRUSR | S_IWUSR, WAIT_count);
#endif
		
		ignore_sigpipe();

#if defined (LYNXOS) && !defined (__Lynx__)
		pthread_attr_create(&attr);
		pthread_create(&t_id, attr, dim_dtq_thread, 0);
#else
/*
		pthread_create(&t_id, NULL, dim_dtq_thread, 0);
*/
		pthread_attr_init(&attr);
		pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
		pthread_create(&t_id, &attr, dim_dtq_thread, 0);
#endif
#ifndef darwin
		ret = sem_wait(&DIM_INIT_Sema);
#else
		ret = sem_wait(DIM_INIT_Semap);
#endif
#if defined (LYNXOS) && !defined (__Lynx__)
		pthread_create(&t_id, attr, dim_tcpip_thread, 0);
#else
		pthread_create(&t_id, &attr, dim_tcpip_thread, 0);
#endif
#ifndef darwin
		ret = sem_wait(&DIM_INIT_Sema);
#else
		ret = sem_wait(DIM_INIT_Semap);
#endif
		INIT_thread = 0;
	}
}

void dim_stop()
{
	int i;
	int n = 0;
	void dim_tcpip_stop(), dim_dtq_stop();

	for( i = 0; i< Curr_N_Conns; i++ )
	{
		if(Net_conns[i].channel != 0)
			n++;
	}
	if(n)
		return;
	if(IO_thread)
		pthread_cancel(IO_thread);
	if(ALRM_thread)
		pthread_cancel(ALRM_thread);
	IO_thread = 0;
	ALRM_thread = 0;
#ifndef darwin 		
	sem_destroy(&DIM_INIT_Sema);
	sem_destroy(&DIM_WAIT_Sema);
#else
	sem_unlink("/Dim_INIT_Sem");
	sem_unlink("/Dim_WAIT_Sem");
	sem_close(DIM_INIT_Semap);
	sem_close(DIM_WAIT_Semap);
#endif
	dim_tcpip_stop();
	dim_dtq_stop();	
	DIM_THR_init_done = 0;
}

int dim_start_thread(thread_ast, tag)
void *(*thread_ast)(void *);
long tag;
{
	pthread_t t_id;
    pthread_attr_t attr;
	
#if defined (LYNXOS) && !defined (__Lynx__)
	pthread_attr_create(&attr);
	pthread_create(&t_id, attr, (void *)thread_ast, (void *)tag);
#else
	pthread_attr_init(&attr);
	pthread_create(&t_id, &attr, thread_ast, (void *)tag);
#endif
	return((int)t_id);
}	

int dim_set_scheduler_class(int pclass)
{
#ifdef __linux__
	int ret, prio, p;
	struct sched_param param;

	if(pclass == 0)
	{
		pclass = SCHED_OTHER;
	}
	else if(pclass == 1)
	{
		pclass = SCHED_FIFO;
	}
	else if(pclass == 2)
	{
		pclass = SCHED_RR;
	}
	prio = sched_get_priority_min(pclass);
	ret = pthread_getschedparam(MAIN_thread, &p, &param);
	if( (p == SCHED_OTHER) || (pclass == SCHED_OTHER) )
		param.sched_priority = prio;
	ret = pthread_setschedparam(MAIN_thread, pclass, &param);   
	if(ret)
	  return 0;
	ret = pthread_getschedparam(IO_thread, &p, &param);   
	if( (p == SCHED_OTHER) || (pclass == SCHED_OTHER) )
		param.sched_priority = prio;
	ret = pthread_setschedparam(IO_thread, pclass, &param);   
	if(ret)
	  return 0;
	ret = pthread_getschedparam(ALRM_thread, &p, &param);   
	if( (p == SCHED_OTHER) || (pclass == SCHED_OTHER) )
		param.sched_priority = prio;
	ret = pthread_setschedparam(ALRM_thread, pclass, &param);   
	if(!ret)
	  return 1;
#endif
	return 0;
}

int dim_get_scheduler_class(int *pclass)
{
#ifdef __linux__
	int ret;
	struct sched_param param;

	ret = pthread_getschedparam(MAIN_thread, pclass, &param);   
	if(!ret)
	  return 1;
#endif
	return 0;
}

int dim_set_priority(int threadId, int prio)
{
#ifdef __linux__
	pthread_t id;
	int ret;
	int pclass;
	struct sched_param param;

	if(threadId == 1)
		id = MAIN_thread;
	else if(threadId == 2)
		id = IO_thread;
	else if(threadId == 3)
		id = ALRM_thread;

	ret = pthread_getschedparam(id, &pclass, &param);   
	param.sched_priority = prio;
	ret = pthread_setschedparam(id, pclass, &param);
	if(!ret)
	  return 1;
#endif
	return 0;
}

int dim_get_priority(int threadId, int *prio)
{
#ifdef __linux__
	pthread_t id;
	int ret;
	int pclass;
	struct sched_param param;

	if(threadId == 1)
		id = MAIN_thread;
	else if(threadId == 2)
		id = IO_thread;
	else if(threadId == 3)
		id = ALRM_thread;

	ret = pthread_getschedparam(id, &pclass, &param);   
	*prio = param.sched_priority;
	if(!ret)
	  return 1;
#endif
	return 0;
}

void ignore_sigpipe()
{

  struct sigaction sig_info;
  sigset_t set;
  void pipe_sig_handler();
	    
  if( sigaction(SIGPIPE, 0, &sig_info) < 0 ) 
  {
    perror( "sigaction(SIGPIPE)" );
    exit(1);
  }
  if(sig_info.sa_handler)
    return;
  sigemptyset(&set);
  sig_info.sa_handler = pipe_sig_handler;
  sig_info.sa_mask = set;
#ifndef LYNXOS 
  sig_info.sa_flags = SA_RESTART;
#else
  sig_info.sa_flags = 0;
#endif

  if( sigaction(SIGPIPE, &sig_info, 0) < 0 ) 
  {
    perror( "sigaction(SIGPIPE)" );
    exit(1);
  }
}

void pipe_sig_handler( num )
int num;
{
  /*  

	printf( "*** pipe_sig_handler called ***\n" );
  */
}

void dim_init_threads()
{
    dim_init();
}

void dim_stop_threads()
{
	dim_stop();
}

int dim_wait(void)
{
	pthread_t id;
	
	id = pthread_self();

	if((id == ALRM_thread) || (id == IO_thread))
	  {
		return(-1);
	  }
#ifndef darwin
	sem_wait(&DIM_WAIT_Sema);
#else
	sem_wait(DIM_WAIT_Semap);
#endif

	return(-1);
}

/*
static void show_ast()
{
sigset_t oset;

	sigprocmask(SIG_SETMASK,0,&oset);
	printf("---THREAD id = %d, mask = %x %x\n",
       pthread_self(), oset.__sigbits[1], oset.__sigbits[0]);
}
*/

pthread_t Dim_thr_locker = 0;
int Dim_thr_counter = 0;
#ifdef LYNXOS
pthread_mutex_t Global_DIM_mutex;
#else
pthread_mutex_t Global_DIM_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

void dim_lock()
{
	/*printf("Locking %d ", pthread_self());*/
    if(Dim_thr_locker != pthread_self())
    {
		pthread_mutex_lock(&Global_DIM_mutex);
		Dim_thr_locker=pthread_self();
		/*printf(": Locked ");*/
	}
    /*printf("Counter = %d\n",Dim_thr_counter);*/
    Dim_thr_counter++;
}
void dim_unlock()	
{
	/*printf("Un-Locking %d ", pthread_self());*/
    Dim_thr_counter--;
    /*printf("Counter = %d ",Dim_thr_counter);*/
    if(!Dim_thr_counter)
    {
		Dim_thr_locker=0;
		pthread_mutex_unlock(&Global_DIM_mutex);
		/*printf(": Un-Locked ");*/
	}
	/*     printf("\n");*/
}


#else

void dim_init()
{
}

void dim_init_threads()
{
}

void dim_stop_threads()
{
}

void dim_stop()
{
}

int dim_wait()
{
  pause();
  return(-1);
}

int dim_start_thread(thread_ast, tag)
void (*thread_ast)();
long tag;
{
	printf("dim_start_thread: not available\n");
}

#endif

#else
#include <windows.h>

DWORD IO_thread = 0;
DWORD ALRM_thread = 0;
DWORD MAIN_thread = 0;
HANDLE hIO_thread;
HANDLE hALRM_thread;
HANDLE hMAIN_thread;
DllExp HANDLE Global_DIM_event = 0;
DllExp HANDLE Global_DIM_mutex = 0;
void dim_tcpip_stop(), dim_dtq_stop();

int dim_start_thread(thread_ast, tag)
#ifndef STDCALL
void (*thread_ast)();
long tag;
#else
unsigned long (*thread_ast)(void *);
void *tag;
#endif
{
DWORD threadid = 0;
HANDLE hthread;

#ifndef STDCALL
    hthread = CreateThread( 
        NULL,                        /* no security attributes			*/
        0,                           /* use default stack size			*/
        (void *)thread_ast,          /* thread function					*/
        (void *)tag,			             /* argument to thread function		*/
        0,                           /* use default creation flags		*/
        &threadid);				     /* returns the thread identifier	*/
#else
    hthread = CreateThread( 
        NULL,                        /* no security attributes			*/
        0,                           /* use default stack size			*/
		thread_ast,					 /* thread function					*/
        tag,			             /* argument to thread function		*/
        0,                           /* use default creation flags		*/
        &threadid);					 /* returns the thread identifier	*/
#endif
	return (int)threadid;
}


int dim_stop_thread(thread_id)
long thread_id;
{
	int ret;

	ret = TerminateThread((HANDLE)thread_id, 0);
	return ret;
}


void create_io_thread()
{
	int tcpip_task(void *);

#ifndef STDCALL
    hIO_thread = CreateThread( 
        NULL,                        /* no security attributes			*/
        0,                           /* use default stack size			*/
        (void *)tcpip_task,          /* thread function					*/
        0,			                 /* argument to thread function		*/
        0,                           /* use default creation flags		*/
        &IO_thread);                 /* returns the thread identifier	*/
#else
    hIO_thread = CreateThread( 
        NULL,                        /* no security attributes			*/
        0,                           /* use default stack size			*/
        tcpip_task,					 /* thread function					*/
        0,			                 /* argument to thread function		*/
        0,                           /* use default creation flags		*/
        &IO_thread);                 /* returns the thread identifier	*/
#endif
}

void create_alrm_thread()
{

	int dtq_task(void *);

#ifndef STDCALL
    hALRM_thread = CreateThread(
        NULL,
        0,
        (void *)dtq_task,
        0,
        0,
        &ALRM_thread);
#else
    hALRM_thread = CreateThread(
        NULL,
        0,
        dtq_task,
        0,
        0,
        &ALRM_thread);
#endif
}

void dim_init_threads()
{
	static int done = 0;

	if(!done)
	{
		hMAIN_thread = GetCurrentThread();
		done = 1;
	}
}

void dim_stop_threads()
{
	int i;
	int n = 0;

	for( i = 0; i< Curr_N_Conns; i++ )
	{
		if(Net_conns[i].channel != 0)
			n++;
	}
	if(n)
		return;
	if(hIO_thread)
		TerminateThread(hIO_thread, 0);
	if(hALRM_thread)
		TerminateThread(hALRM_thread, 0);
	if(Global_DIM_mutex) 
		CloseHandle(Global_DIM_mutex);
	if(Global_DIM_event) 
		CloseHandle(Global_DIM_event);
	hIO_thread = 0;
	hALRM_thread = 0;
	Global_DIM_mutex = 0;
	Global_DIM_event = 0;
	dim_tcpip_stop();
	dim_dtq_stop();
}

void dim_stop()
{
	dim_stop_threads();
}

int dim_set_scheduler_class(int pclass)
{
	HANDLE hProc;
	int ret;
	DWORD p;

	hProc = GetCurrentProcess();

	if(pclass == -1)
		p = IDLE_PRIORITY_CLASS;
/*
	else if(pclass == -1)
		p = BELOW_NORMAL_PRIORITY_CLASS;
*/
	else if(pclass == 0)
		p = NORMAL_PRIORITY_CLASS;
/*
	else if(pclass == 1)
		p == ABOVE_NORMAL_PRIORITY_CLASS;
*/
	else if(pclass == 1)
		p = HIGH_PRIORITY_CLASS;
	else if(pclass == 2)
		p = REALTIME_PRIORITY_CLASS;
	ret = SetPriorityClass(hProc, p);
	if(ret)
	  return 1;
	ret = GetLastError();
	printf("ret = %x %d\n",ret, ret);
	return 0;
}

int dim_get_scheduler_class(int *pclass)
{
	HANDLE hProc;
	DWORD ret;

	hProc = GetCurrentProcess();

	ret = GetPriorityClass(hProc);
	if(ret == 0)
	  return 0;
	if(ret == IDLE_PRIORITY_CLASS)
		*pclass = -1;
/*
	else if(ret == BELOW_NORMAL_PRIORITY_CLASS)
		*pclass = -1;
*/
	else if(ret == NORMAL_PRIORITY_CLASS)
		*pclass = 0;
/*
	else if(ret == ABOVE_NORMAL_PRIORITY_CLASS)
		*pclass = 1;
*/
	else if(ret == HIGH_PRIORITY_CLASS)
		*pclass = 1;
	else if(ret == REALTIME_PRIORITY_CLASS)
		*pclass = 2;
	return 1;
}

int dim_set_priority(int threadId, int prio)
{
	HANDLE id;
	int ret, p;

	if(threadId == 1)
		id = hMAIN_thread;
	else if(threadId == 2)
		id = hIO_thread;
	else if(threadId == 3)
		id = hALRM_thread;

	if(prio == -3)
		p = THREAD_PRIORITY_IDLE;
	if(prio == -2)
		p = THREAD_PRIORITY_LOWEST;
	if(prio == -1)
		p = THREAD_PRIORITY_BELOW_NORMAL;
	if(prio == 0)
		p = THREAD_PRIORITY_NORMAL;
	if(prio == 1)
		p = THREAD_PRIORITY_ABOVE_NORMAL;
	if(prio == 2)
		p = THREAD_PRIORITY_HIGHEST;
	if(prio == 3)
		p = THREAD_PRIORITY_TIME_CRITICAL;

	ret = SetThreadPriority(id, p); 
	if(ret)
	  return 1;
	return 0;
}

int dim_get_priority(int threadId, int *prio)
{
	HANDLE id;
	int ret, p;

	if(threadId == 1)
		id = hMAIN_thread;
	else if(threadId == 2)
		id = hIO_thread;
	else if(threadId == 3)
		id = hALRM_thread;

	ret = GetThreadPriority(id); 
	if(ret == THREAD_PRIORITY_ERROR_RETURN)
	  return 0;
	if(ret == THREAD_PRIORITY_IDLE)
		p = -3;
	if(ret == THREAD_PRIORITY_LOWEST)
		p = -2;
	if(ret == THREAD_PRIORITY_BELOW_NORMAL)
		p = -1;
	if(ret == THREAD_PRIORITY_NORMAL)
		p = 0;
	if(ret == THREAD_PRIORITY_ABOVE_NORMAL)
		p = 1;
	if(ret == THREAD_PRIORITY_HIGHEST)
		p = 2;
	if(ret == THREAD_PRIORITY_TIME_CRITICAL)
		p = 3;
	*prio = p;
	return 1;
}

void dim_init()
{
}

void dim_no_threads()
{
}

int dim_wait()
{
	pause();
	return(1);
}

void dim_lock()
{
	if(!Global_DIM_mutex)
	{ 
		Global_DIM_mutex = CreateMutex(NULL,FALSE,NULL);
	}
	WaitForSingleObject(Global_DIM_mutex, INFINITE);
}

void dim_unlock()
{
	ReleaseMutex(Global_DIM_mutex);
}

void dim_pause()
{
	if(!Global_DIM_event)
	{ 
		Global_DIM_event = CreateEvent(NULL,FALSE,FALSE,NULL);
	}
	else 
	{
		WaitForSingleObject(Global_DIM_event, INFINITE);
	}
}

void dim_wake_up()
{
	SetEvent(Global_DIM_event);
}

void dim_sleep(unsigned int t)
{
	Sleep(t*1000);
}

void dim_win_usleep(unsigned int t)
{
	Sleep(t/1000);
}

#endif
