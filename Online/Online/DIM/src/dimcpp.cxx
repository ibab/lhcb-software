#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __VMS
#include <starlet.h>
#endif

#define DIMLIB
#include <dim_core.hxx>
#include <dim.hxx>

int DimCore::inCallback = 0;
int DimUtil::itsBufferSize = 0;
char *DimUtil::itsBuffer = (char *)0;

extern "C" {
static void timer_user_routine(void *tp)
{
	DimTimer *t = (DimTimer *)tp;
	DimCore::inCallback = 1;
	t->firedFlag = 1;
	t->runningFlag = 0;
	t->timerHandler();
	DimCore::inCallback = 0;
}
}

int DimTimer::start(int time)
{
	if(runningFlag)
		return 0;
	runningFlag = 1;
	firedFlag = 0;
	dtq_start_timer(time, timer_user_routine, this);
	return 1;
}

int DimTimer::stop() 
{
	firedFlag = 0;
	runningFlag = 0;
	return dtq_stop_timer(this);
}

DimTimer::DimTimer() 
{ 
	firedFlag = 0;
	runningFlag = 0; 
}
	
DimTimer::DimTimer(int time)
{ 
	firedFlag = 0;
	runningFlag = 0; 
	start(time);
}

DimTimer::~DimTimer()
{
	if(runningFlag)
		stop();
}

// Threads

extern "C" {
static void thread_user_routine(void *tp)
{
	DimThread *t = (DimThread *)tp;
//	DimCore::inCallback = 1;
//	t->firedFlag = 1;
//	t->runningFlag = 0;
	t->threadHandler();
	t->itsId = 0;
//	DimCore::inCallback = 0;
}
}

DimThread::DimThread() 
{
//	start();
	itsId = 0;
}
	
DimThread::~DimThread()
{
//	if(itsId)
//		stop();
}

int DimThread::start()
{
	if(!itsId)
	{
		itsId = (long)dim_start_thread(thread_user_routine, this);
		return 1;
	}
	return 0;
}
/*
int DimThread::stop()
{
	int ret = dim_stop_thread(itsId);
	itsId = 0;
	return ret;
}
*/

DimUtil::DimUtil() 
{
}
	
DimUtil::~DimUtil()
{
}

char *DimUtil::getEnvVar(char *name)
{
	int size;

	size = dim_get_env_var(name, 0, 0);
	if(!size)
		return (char *)0;
	if((itsBufferSize < size ) && (itsBufferSize != 0))
	{
		delete[] itsBuffer;
		itsBufferSize = 0;
	}
	if(!itsBufferSize)
	{
		itsBuffer = new char[size];
		itsBufferSize = size;
	}
	dim_get_env_var(name, itsBuffer, itsBufferSize);
	return itsBuffer;
}
