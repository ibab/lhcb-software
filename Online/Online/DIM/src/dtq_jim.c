/*
 * DTQ (Delphi Timer Queue) implements the action scheduling for the DIM
 * (Delphi Information Managment) System.
 * It will be used by servers clients and the Name Server.
 *
 * Started date   : 10-11-91
 * Written by     : C. Gaspar
 * UNIX adjustment: G.C. Ballintijn
 */

/* include files */
#include <signal.h>
#include <stdio.h>
#define DIMLIB
#include <dim.h>
#ifdef __linux__
#include <sys/time.h>
#endif
#ifdef VxWorks
#include <time.h>
#endif

/* global definitions */
#define MAX_TIMER_QUEUES	16	/* Number of normal queue's     */
#define SPECIAL_QUEUE		16	/* The queue for the queue-less */
#define WRITE_QUEUE			17 

_DIM_PROTO( static void alrm_sig_handler,  (int num) );
_DIM_PROTO( static void Std_timer_handler, () );
_DIM_PROTO( static int stop_it,			   (int new_time) );
_DIM_PROTO( static int start_it,		   (int new_time) );
_DIM_PROTO( static int scan_it,			   () );
_DIM_PROTO( int dtq_task, (void *dummy) );
_DIM_PROTO( static int my_alarm, (int secs) );
_DIM_PROTO( int dim_dtq_init,	   (int thr_flag) );
#ifndef WIN32
_DIM_PROTO( static void dummy_alrm_sig_handler, (int num) );
#endif

typedef struct {
	TIMR_ENT *queue_head;
	int remove_entries;
} QUEUE_ENT;


static QUEUE_ENT timer_queues[MAX_TIMER_QUEUES + 2] = { 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 
};

static int Inside_ast = 0;
static int Alarm_runs = 0;
static int sigvec_done = 0;

#ifdef VxWorks
static timer_t Timer_id;
#endif

static int DIM_elapsed_time = 0;
static int Threads_off = 0;

static time_t last_time;

/*
 * DTQ routines
 */


#ifndef WIN32

void dim_no_threads()
{
	Threads_off = 1;
	dic_no_threads();
	dis_no_threads();
}

int dim_dtq_init(thr_flag)
int thr_flag;
{
struct sigaction sig_info;
sigset_t set, set1;
int i, pid, ret = 0;

	pid = getpid();
	if( !sigvec_done) 
	{
	    Inside_ast = 0;
	    Alarm_runs = 0;
	    last_time = 0;
	    for(i = 0; i < MAX_TIMER_QUEUES + 2; i++)
	    {
	        timer_queues[i].queue_head = 0;
			timer_queues[i].remove_entries = 0;
	    }
		if( timer_queues[SPECIAL_QUEUE].queue_head == NULL ) {
			timer_queues[SPECIAL_QUEUE].queue_head = (TIMR_ENT *)malloc(sizeof(TIMR_ENT));
			memset(timer_queues[SPECIAL_QUEUE].queue_head, 0, sizeof(TIMR_ENT));
			dll_init( (DLL *)timer_queues[SPECIAL_QUEUE].queue_head);
		}
		if( timer_queues[WRITE_QUEUE].queue_head == NULL ) {
			timer_queues[WRITE_QUEUE].queue_head = (TIMR_ENT *)malloc(sizeof(TIMR_ENT));
			memset(timer_queues[WRITE_QUEUE].queue_head, 0, sizeof(TIMR_ENT));
			dll_init( (DLL *)timer_queues[WRITE_QUEUE].queue_head);
		}
	    if(!thr_flag)
	    {
	        Threads_off = 1;
	    }
		sigemptyset(&set);
	  
		sigaddset(&set,SIGIO);
		
		if(thr_flag)
			sig_info.sa_handler = dummy_alrm_sig_handler;
		else
			sig_info.sa_handler = alrm_sig_handler;
		sig_info.sa_mask = set;
#ifndef LYNXOS
		sig_info.sa_flags = SA_RESTART;
#else
		sig_info.sa_flags = 0;
#endif
		if( sigaction(SIGALRM, &sig_info, 0) < 0 ) {
			perror( "sigaction(SIGALRM)" );
			exit(1);
		}
	    
	    sigvec_done = 1;
	    ret = 1;
	}
	return(ret);
}

void dummy_alrm_sig_handler( num )
int num;
{
}

#else

int dim_dtq_init(thr_flag)
int thr_flag;
{
	int i, tid = 1;
	void create_alrm_thread();

	if( !sigvec_done ) {
		Inside_ast = 0;
	    Alarm_runs = 0;
	    last_time = 0;
	    for(i = 0; i < MAX_TIMER_QUEUES + 2; i++)
	    {
	        timer_queues[i].queue_head = 0;
			timer_queues[i].remove_entries = 0;
	    }
		if( timer_queues[SPECIAL_QUEUE].queue_head == NULL ) {
			timer_queues[SPECIAL_QUEUE].queue_head = (TIMR_ENT *)malloc(sizeof(TIMR_ENT));
			memset(timer_queues[SPECIAL_QUEUE].queue_head, 0, sizeof(TIMR_ENT));
			dll_init( (DLL *)timer_queues[SPECIAL_QUEUE].queue_head);
		}
		if( timer_queues[WRITE_QUEUE].queue_head == NULL ) {
			timer_queues[WRITE_QUEUE].queue_head = (TIMR_ENT *)malloc(sizeof(TIMR_ENT));
			memset(timer_queues[WRITE_QUEUE].queue_head, 0, sizeof(TIMR_ENT));
			dll_init( (DLL *)timer_queues[WRITE_QUEUE].queue_head);
		}
/*
#ifndef STDCALL
		tid = _beginthread((void *)(void *)dtq_task,0,NULL);
#else
		tid = _beginthreadex(NULL, NULL,
			dtq_task,0,0,NULL);
#endif
*/
		create_alrm_thread();
		sigvec_done = 1;
	}
	return(tid);
}

#endif


int my_alarm(int secs)
{
	int ret;

#ifndef WIN32
	if(Threads_off)
	{
		if( secs < 0)
		{
			kill(getpid(),SIGALRM);
			return(0);
		}
		else
		{
			return(alarm(secs));
		}
	}
	else
	{
#endif

		ret = DIM_elapsed_time/10;
		DIM_elapsed_time = secs*10;
		return(ret);
#ifndef WIN32
	}
#endif
}

void dim_usleep(int usecs)
{

#ifndef WIN32
	struct timeval timeout;

	timeout.tv_sec = 0;
	timeout.tv_usec = usecs;
	select(FD_SETSIZE, NULL, NULL, NULL, &timeout);
#else
	usleep(usecs);
#endif
}

int dtq_task(void *dummy)
{
int i;
int done = 0;

	while(1)
	{
		if(DIM_elapsed_time < 0)
		{
			DIM_elapsed_time = 0;
			alrm_sig_handler(2);
#ifndef WIN32
			return(1);
#endif
		}
		else if(DIM_elapsed_time > 0)
		{
			dim_usleep(100000);
			DIM_elapsed_time--;
			if(!DIM_elapsed_time)
			{
				alrm_sig_handler(2);
#ifndef WIN32
				return(1);
#endif
			}
/*
			New_elapsed_time = 0;
			for(i = 0; i < 10; i++)
			{
				if(New_elapsed_time)
				{
					New_elapsed_time = 0;
					if(DIM_elapsed_time < 0)
						break;
					i = 0;
				}
				dim_usleep(100000);
				DIM_deci_counter = i+1;
			}
			if(!New_elapsed_time)
			{
				DIM_elapsed_time--;
			}
			if(!DIM_elapsed_time)
			{
				alrm_sig_handler(2);
				done = 1;
			}
#ifndef WIN32
			if(done)
			  return(1);
#endif
*/
		}
		else
		{
			dim_usleep(1000);
		}
	}
}

int dtq_create()
{
	int i;

	if(!Threads_off)
	{
		dim_init_threads();
	}
	dim_dtq_init(0);
	for( i = 1; i < MAX_TIMER_QUEUES; i++ )
		if( timer_queues[i].queue_head == 0 )
			break;

	if( i == MAX_TIMER_QUEUES )
		return(0);

	timer_queues[i].queue_head = (TIMR_ENT *)malloc( sizeof(TIMR_ENT) );
	memset( timer_queues[i].queue_head, 0, sizeof(TIMR_ENT) );
	dll_init( (DLL *)timer_queues[i].queue_head);

	return(i);
}


int dtq_delete(queue_id)
int queue_id;
{
	TIMR_ENT *queue_head, *entry;

	DISABLE_AST
	queue_head = timer_queues[queue_id].queue_head;
	if(queue_head)
	{
		while(!dll_empty((DLL *)queue_head))
		{
			entry = queue_head->next;
			dll_remove(entry);
			free(entry);
		}
		free(queue_head);
		timer_queues[queue_id].queue_head = 0;
	}
	ENABLE_AST
	return(1);			
}
	
TIMR_ENT *dtq_add_entry(queue_id, time, user_routine, tag)
int queue_id, time;
long tag;
void (*user_routine)();
{
	TIMR_ENT *new_entry, *queue_head, *auxp, *prevp;
	int next_time;

	DISABLE_AST 
	next_time = time;
	if(Alarm_runs)
	{
	    if((DIM_elapsed_time > time*10) || (queue_id == SPECIAL_QUEUE))
	    {
			next_time = stop_it(time);
	    }
	}
	new_entry = (TIMR_ENT *)malloc( sizeof(TIMR_ENT) );
	new_entry->time = time;
    if( user_routine )
   	   	new_entry->user_routine = user_routine;
	else
       	new_entry->user_routine = Std_timer_handler;
	new_entry->tag = tag;
	new_entry->time_left = time;

	queue_head = timer_queues[queue_id].queue_head;
	if(!time)
	{
		dll_insert_after((DLL *)queue_head->prev, (DLL *)new_entry);
	}
	else
	{
		if(queue_head)
		{
			auxp = queue_head;
			prevp = auxp;
			while(auxp = (TIMR_ENT *)dll_get_prev((DLL *)queue_head, (DLL *)auxp))
			{
				if(time >= auxp->time)
				{
					break;
				}
				prevp = auxp;
			}
                        /* What the hell is this about???
			if(auxp)
			{
				if(queue_id != SPECIAL_QUEUE)
				{
					if(auxp->time_left > 0)
					{
						if(auxp->time == time)
							new_entry->time_left = auxp->time_left;
					}
				}
				prevp = auxp;
			} */
			dll_insert_after((DLL *)prevp, (DLL *)new_entry);
		}
	}
	if(!Alarm_runs)
	{
		if(!next_time)
			next_time = -10;
		start_it(next_time);
	}
	ENABLE_AST
	return(new_entry); 
}

int dtq_clear_entry(entry)
TIMR_ENT *entry;
{
	int time_left;

	DISABLE_AST
	time_left = entry->time_left;
	entry->time_left = entry->time + DIM_elapsed_time/10;
	ENABLE_AST
	return(time_left);
}


int dtq_rem_entry(queue_id, entry)
int queue_id;
TIMR_ENT *entry;
{
	int time_left;

	DISABLE_AST
	time_left = entry->time_left;
	if( Inside_ast ) 
	{
		timer_queues[queue_id].remove_entries++;
		entry->time = -1;
		ENABLE_AST
		return(time_left);
	}
	dll_remove(entry);
	free(entry);

	ENABLE_AST
	return(time_left);
}

static int rem_deleted_entries(queue_id)
int queue_id;
{
	TIMR_ENT *auxp, *prevp, *queue_head;
	int n;

	DISABLE_AST
	queue_head = timer_queues[queue_id].queue_head;
	n = timer_queues[queue_id].remove_entries;
	if(queue_head)
	{
		auxp = queue_head;
		prevp = auxp;
		while(auxp = (TIMR_ENT *)dll_get_next((DLL *)queue_head, (DLL *)auxp))
		{
			if(auxp->time == -1)
			{
				dll_remove(auxp);
				free(auxp);
				auxp = prevp;
				n--;
				if(!n)
					break;
			}
			else
				prevp = auxp;
		}
	}
	ENABLE_AST;
	return(1);
}

static int stop_it(new_time)
int new_time;
{
	TIMR_ENT *auxp, *queue_head;
	int queue_id;
	int min_time = 100000;
	int n = 0;
    time_t now;
    int deltat = 0;

	DISABLE_AST

    if (Alarm_runs)
    {
        time(&now);
        deltat = now - last_time;
        last_time = now;

	    Alarm_runs = 0;
    }
	if(new_time != -1)
	{
		min_time = new_time;
		if(new_time == 0)
			min_time = -10;
	}
	queue_head = timer_queues[WRITE_QUEUE].queue_head;
	if( dll_get_next((DLL *)queue_head,(DLL *)queue_head))
		min_time = -10;
	if((min_time != -10) || deltat)
	{
		if((queue_head = timer_queues[SPECIAL_QUEUE].queue_head) != NULL)
		{
			auxp = queue_head;
			while(auxp = (TIMR_ENT *)dll_get_next((DLL *)queue_head,(DLL *)auxp))
			{
				auxp->time_left -= deltat;
				if(auxp->time_left > 0)
				{
					if(auxp->time_left < min_time)
					{
						min_time = auxp->time_left;
					}
				}
			}
		}
		for( queue_id = 0; queue_id < MAX_TIMER_QUEUES; queue_id++ ) 
		{
			if( (queue_head = timer_queues[queue_id].queue_head) == NULL )
				continue;
			auxp = queue_head;
			while(auxp = (TIMR_ENT *)dll_get_next((DLL *)queue_head,(DLL *)auxp))
			{		
				auxp->time_left -= deltat;
				if(auxp->time_left > 0)
				{
					if(auxp->time_left < min_time)
					{
						min_time = auxp->time_left;
					}
				}
				else
				{
					if(auxp->time < min_time)
					{
						min_time = auxp->time;
					}
				}
				n++;
				if((!deltat) && (n == 50))
					break;
			}
		}
	}
	if(min_time == 100000)
		min_time = 0;
	ENABLE_AST
	return(min_time);
}

static int start_it(new_time)
int new_time;
{
	int next_time;
	TIMR_ENT *queue_head;

	DISABLE_AST
	next_time = new_time;
	if(next_time > 0)
	{
		queue_head = timer_queues[WRITE_QUEUE].queue_head;
		if( dll_get_next((DLL *)queue_head,(DLL *)queue_head))
		{
			next_time = -10;
		}
	}
	if(next_time)
	{
		Alarm_runs = 1;
		my_alarm(next_time);
	}

    time(&last_time);

	ENABLE_AST
	return(1);
}

static int scan_it()
{
	int queue_id, i, n = 0;
	static int curr_queue_id = 0;
	static TIMR_ENT *curr_entry = 0;
	TIMR_ENT *auxp, *prevp, *queue_head;
	TIMR_ENT *done[100];

	DISABLE_AST
	queue_head = timer_queues[WRITE_QUEUE].queue_head;
	auxp = queue_head;
	while(auxp = (TIMR_ENT *)dll_get_next((DLL *)queue_head,(DLL *)auxp))
	{	
		done[n++] = auxp;
		if(n == 50)
			break;
	}
	ENABLE_AST
	for(i = 0; i < n; i++)
	{
		auxp = done[i];
		auxp->user_routine( auxp->tag );
	}
	{
		DISABLE_AST
		for(i = 0; i < n; i++)
		{
			auxp = done[i];
			dll_remove(auxp);
			free(auxp);
		}
		if(n == 50)
		{
			ENABLE_AST
			return(1);
		}
		ENABLE_AST
	}
	{
	DISABLE_AST
	queue_head = timer_queues[SPECIAL_QUEUE].queue_head;
	auxp = queue_head;
	prevp = auxp;
	while(auxp = (TIMR_ENT *)dll_get_next((DLL *)queue_head,(DLL *)auxp))
	{	
		if(auxp->time_left <= 0)
		{
			dll_remove(auxp);
			auxp->user_routine( auxp->tag );
			free(auxp);
			auxp = prevp;
			n++;
			if(n == 50)
			{
				ENABLE_AST
				return(1);
			}
		}
		else
			prevp = auxp;
	}
	for( queue_id = curr_queue_id; queue_id < MAX_TIMER_QUEUES; queue_id++ ) 
	{
		if( (queue_head = timer_queues[queue_id].queue_head) == NULL )
			continue;
		Inside_ast = 1;
		if((curr_entry) && (queue_id == curr_queue_id))
			auxp = curr_entry;
		else
			auxp = queue_head;
		while(auxp = (TIMR_ENT *)dll_get_next((DLL *)queue_head,(DLL *)auxp))
		{	
			if(auxp->time_left <= 0)
			{
				auxp->user_routine( auxp->tag );
				auxp->time_left = auxp->time; /*restart clock*/
				n++;
				if(n == 50)
				{
					curr_queue_id = queue_id;
					curr_entry = auxp;
					ENABLE_AST
					return(1);
				}
			}
		}
		Inside_ast = 0;
		if( timer_queues[queue_id].remove_entries ) {
			rem_deleted_entries( queue_id );
			timer_queues[queue_id].remove_entries = 0;
		}
	}
	curr_queue_id = 0;
	curr_entry = 0;
	ENABLE_AST
	}
	return(0);
}

static void alrm_sig_handler( num)
int num;
{
	int next_time;
        
	next_time = stop_it(-1);
	if(scan_it())
		next_time = -10;
	if(!Alarm_runs)
		start_it(next_time);
}

static void Std_timer_handler()
{
}

void dtq_start_timer(time, user_routine, tag)
int time;
long tag;
void (*user_routine)();
{
	if(!Threads_off)
	{
		dim_init_threads();
	}
	dim_dtq_init(0);
	if(time != 0)
		dtq_add_entry(SPECIAL_QUEUE, time, user_routine, tag);
	else
		dtq_add_entry(WRITE_QUEUE, time, user_routine, tag);
}


int dtq_stop_timer(tag)
long tag;
{
	TIMR_ENT *entry, *queue_head, *prevp;
	int time_left = -1;

	queue_head = timer_queues[SPECIAL_QUEUE].queue_head;
	entry = queue_head;
	prevp = entry;
	while(entry = (TIMR_ENT *)dll_get_next((DLL *)queue_head,(DLL *)entry))
	{
		if( entry->tag == tag ) 
		{
			time_left = dtq_rem_entry( SPECIAL_QUEUE, entry );
			break;
		}
	}
	return(time_left);
}

static int Dtq_sleeping = 0;

void dtq_sleep_rout(tag)
long tag;
{
	Dtq_sleeping = 0;
#ifdef WIN32
	wake_up();
#endif
}

#ifndef WIN32

int dtq_sleep(secs)
int secs;
{

#ifndef NOTHREADS
	int i;
	for(i = 0; i < secs*2; i++)
    {
		dim_usleep(500000);
    }
	return(0);
#else
	sigset_t set, oset;

	sigemptyset(&set);
	sigaddset(&set,SIGALRM);
	sigprocmask(SIG_UNBLOCK, &set, &oset);
	Dtq_sleeping = 1;
	dtq_start_timer(secs, dtq_sleep_rout, (void *)123);
    do{
		pause();
	}while(Dtq_sleeping);
    sigprocmask(SIG_SETMASK,&oset,0);
	return(0);
#endif
}

#else

int dtq_sleep(secs)
int secs;
{
	Dtq_sleeping = 1;
	dtq_start_timer(secs, dtq_sleep_rout, 1);
	do{
		dim_wait();
	}while(Dtq_sleeping);
	return(0);
}

#endif
