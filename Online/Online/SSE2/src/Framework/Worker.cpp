#include "RTL/rtl.h"
#include "Framework/Lock.h"
#include "Framework/Kernel.h"
#include "Framework/Worker.h"
#include "Framework/IExecutor.h"
#include "Framework/IDataflowMgr.h"

#include <stdexcept>
#include <iostream>
#include <string>

using namespace Framework;
using namespace std;

/// Standard constructor. Thread is automatically started and put "on hold"
Worker::Worker(IDataflowMgr* master) 
: m_lock(0), m_thread(0), m_master(master), m_context(0), m_executor(0), 
  m_wakeup(0), m_state(ACTIVE)
{
  int status = ::lib_rtl_create_event(0,&m_lock);
  if ( !lib_rtl_is_success(status) ) {
    throw std::runtime_error("Failed to create lock for worker thread: "+string(RTL::errorString()));
  }
  status = ::lib_rtl_clear_event(m_lock);
  if ( !lib_rtl_is_success(status) ) {
    throw std::runtime_error("Failed to clear lock for worker thread: "+string(RTL::errorString()));
  }
}

/// Default destructor. Thread is stopped
Worker::~Worker() {
  ::lib_rtl_set_event(m_lock);
  ::lib_rtl_join_thread(m_thread);
  m_thread = 0;
  ::lib_rtl_delete_event(m_lock);
  m_lock = 0;
}

/// Master call [IDataflowMgr]: Start the thread
Status Worker::start()  {
  if ( 0 == m_thread )  {
    int status = ::lib_rtl_start_thread(exec_thread,this,&m_thread);
    if ( !lib_rtl_is_success(status) ) {
      ::lib_rtl_output(LIB_RTL_ERROR,MSG_SRC_FMT"Failed to create worker thread: %s",
        "Thread", this, RTL::errorString());
      return Status::ERROR;
    }
    return Status::SUCCESS;
  }
  ::lib_rtl_output(LIB_RTL_ERROR,MSG_SRC_FMT"This thread is already running.","Thread", this);
  return Status::ERROR;
}

/// Internal hook called by the master thread to start worker thread
Status Worker::unlock() {
  int status = ::lib_rtl_set_event(m_lock);
  if ( lib_rtl_is_success(status) ) return Status::SUCCESS;
  int err = ::lib_rtl_get_error();
  ::lib_rtl_output(LIB_RTL_ERROR,MSG_SRC_FMT"Failed to release worker lock: [%d] %s ",
    "Thread", this, err, RTL::errorString(err));
  return Status::ERROR;
}

/// Internal hook called by the master thread to suspend worker thread
Status Worker::lock() {
  int status = ::lib_rtl_wait_for_event(m_lock);
  if ( lib_rtl_is_success(status) ) return Status::SUCCESS;
  int err = ::lib_rtl_get_error();
  ::lib_rtl_output(LIB_RTL_ERROR,MSG_SRC_FMT "Failed to aquire worker lock: [%d] %s ",
		   "Thread", this, err, RTL::errorString(err));
  return Status::ERROR;
}

/// Static thread startup routing
int Worker::exec_thread(void* arg) {
  Worker* w = (Worker*)arg;
  w->run();
  return 1;
}

#ifdef _WIN32
static __declspec(thread) void* number = 0;
#else
static __thread void* number = 0;
#endif

/// Thread ACTIVE routine: here the work load is executed by the thread
void Worker::run()    {
  struct timeval start = {0,0};
  ::lib_rtl_output(LIB_RTL_INFO,MSG_SRC_FMT"Started...\n","Thread", this);
  m_master->workerActive(this);
  while( m_state == ACTIVE ) {
    lock();
    try  {
      if ( m_state == OFFLINE ) {
        ::lib_rtl_output(LIB_RTL_ALWAYS,MSG_SRC_FMT "Shutdown after %d execution cycles. Static storage:0x%p\n",
			 "Thread", this, m_wakeup, &number);
        m_state = ENDED;
        unlock();
        return;
      }
      ::lib_rtl_output(LIB_RTL_INFO,MSG_SRC_FMT "Start processing event: 0x%p Context:0x%p.\n",
		       "Thread", this, m_executor, m_context);
      ++m_wakeup;
      start = Timing::now();
      m_executor->execute(m_context);
    }
    catch(const std::exception& e) {
      ::lib_rtl_output(LIB_RTL_ERROR,MSG_SRC_FMT "Worker exception: %s.\n",
		       "Thread", this,e.what());
    }
    catch(...) {
      ::lib_rtl_output(LIB_RTL_ERROR,MSG_SRC_FMT "Worker exception [Unknown].\n",
		       "Thread", this);
    }
    // Now finalize the request
    Executor* executor    = m_executor;
    EventContext* context = m_context;
    m_context = 0;
    m_executor = 0;
    m_state = ACTIVE;
    m_master->workerDone(this,executor,context,start,Timing::now());
    ::lib_rtl_output(LIB_RTL_INFO,MSG_SRC_FMT "Finished to process the event.\n",
		     "Thread", this);
  }
  m_state = ENDED;
}

/// Master call [IDataflowMgr]: Stop the worker from processing once idle
Status Worker::queueStop()   {
  State st = m_state;
  m_state = OFFLINE;
  if ( st == ACTIVE )  {
    return unlock();
  }
  return Status::SUCCESS;
}

/// Master call [IDataflowMgr]: Wait until worker has stopped processing, then delete it
Status Worker::release()   {
  if ( m_state == ENDED || m_state == OFFLINE )  {
    Status sc = lock();
    // if the worker was busy, this will wait until the thread is dead
    delete this;
    return Status::SUCCESS;
  }
  ::lib_rtl_output(LIB_RTL_ERROR,MSG_SRC_FMT "Cannot stop if thread has not ENDED.\n",
		   "Thread", this);
  return Status::ERROR;
}

/// Master call [IDataflowMgr]: Setup worker thread and resume execution mode
Status Worker::execute(Executor* executor, EventContext* context) {
  if ( executor && context ) {
    setContext(context);
    setExecutor(executor);
    /// All is done. We can now release the lock on the worker and let him execute
    ::lib_rtl_output(LIB_RTL_DEBUG,MSG_SRC_FMT "Submitted. Executor:0x%p Context:0x%p.\n",
		     "Thread", this, executor, context);
    return unlock();
  }
  return Status::ERROR;
}

/// Master call: Suspend the processing loop of this worker
Status Worker::suspend()   {
  return lock();
}
