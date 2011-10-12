#include "Framework/Lock.h"
#include "Framework/Kernel.h"
#include "Framework/Worker.h"
#include "Framework/Monitor.h"
#include "Framework/Helpers.h"
#include "Framework/IExecutor.h"
#include "Framework/DataflowMgr.h"
#include "Framework/ItemRegistry.h"

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <string>

using namespace Framework;
using namespace std;

/// Standard constructor. Thread is automatically started and put "on hold"
DataflowMgr::DataflowMgr() 
  : m_lock(0), m_dataRegistry(0), m_contextFactory(0), m_monitor(0), 
    m_state(OFFLINE), m_starting(0)
{
  int status = ::lib_rtl_create_lock(0,&m_lock);
  if ( !lib_rtl_is_success(status) ) {
    throw std::runtime_error("Failed to create lock for dataflow manager: "+string(RTL::errorString()));
  }
  status = ::lib_rtl_create_event(0,&m_evt);
  if ( !lib_rtl_is_success(status) ) {
    throw std::runtime_error("Failed to create event for dataflow manager: "+string(RTL::errorString()));
  }
  status = ::lib_rtl_clear_event(m_evt); 
  if ( !lib_rtl_is_success(status) ) {
    throw std::runtime_error("Failed to clear event for dataflow manager: "+string(RTL::errorString()));
  }
  m_dataRegistry = new ItemRegistry();
}

/// Default destructor. Thread is stopped
DataflowMgr::~DataflowMgr() {
  ::lib_rtl_delete_event(m_evt);
  m_evt = 0;
  ::lib_rtl_delete_lock(m_lock);
  m_lock = 0;
  deletePtr(m_dataRegistry);
}

/// Internal hook called by the master thread to start worker thread
Status DataflowMgr::unlock() {
  int status = ::lib_rtl_unlock(m_lock);
  if ( lib_rtl_is_success(status) ) return Status::SUCCESS;
  int err = ::lib_rtl_get_error();
  ::lib_rtl_output(LIB_RTL_ERROR,MSG_SRC_FMT"Failed to release worker lock: [%d] %s \n",
    "DataflowMgr", this, err, RTL::errorString(err));
  return Status::ERROR;
}

/// Internal hook called by the master thread to suspend worker thread
Status DataflowMgr::lock() {
  int status = ::lib_rtl_lock(m_lock);
  if ( lib_rtl_is_success(status) ) return Status::SUCCESS;
  int err = ::lib_rtl_get_error();
  ::lib_rtl_output(LIB_RTL_ERROR,MSG_SRC_FMT"Failed to aquire worker lock: [%d] %s\n",
    "DataflowMgr", this, err, RTL::errorString(err));
  return Status::ERROR;
}

/// Worker call: finish the work and put worker back into sleep mode _after_ call return
Status DataflowMgr::workerDone(Worker* worker,
			       Executor* executor,
			       EventContext* context, 
			       const struct timeval& start,
			       const struct timeval& stop)
{
  Lock<DataflowMgr> lck(this);
  if ( lck )     {
    if ( executor ) {
      executor->monitor(start,stop);
    }
    if ( m_monitor ) {
      m_monitor->monitor(executor,context,start,stop);
    }
    Workers::iterator iw = m_busyQue.find(worker);
    context->addData(executor->outputMask());
    context->clearDoneBit(executor->id());
    m_busyQue.erase(iw);
    m_idleQue.insert(worker);
    executor->returnInstance();
    return resume();
  }
  return Status::ERROR;
}

/// Worker call: Signal master that the work is done and the worker is up for new stuff
Status DataflowMgr::workerIdle(Worker* /* worker */ )    {
  return resume();
}

/// Worker call: Signal master, that the thread is ready to receive work
Status DataflowMgr::workerActive(Worker* /*  worker  */ )   {
  Lock<DataflowMgr> lck(this);
  if ( lck ) {
    --m_starting;
    if ( 0 == m_starting )  {
      // InvokeTransition(INITIALIZED,RUNNING);
      // 
      ::lib_rtl_output(LIB_RTL_INFO,MSG_SRC_FMT"All threads started...\n",
        "DataflowMgr", this);
      m_state = RUNNING;
      return resume();
    }
    return Status::SUCCESS;
  }
  return Status::ERROR;
}

Status DataflowMgr::resume() {
  ::lib_rtl_output(LIB_RTL_DEBUG,MSG_SRC_FMT"resume event processing ...\n","DataflowMgr", this);
  int status = lib_rtl_set_event(m_evt);
  return lib_rtl_is_success(status) ? Status::SUCCESS : Status::ERROR;
}

/// Add new processor factory 
void DataflowMgr::adoptFactory(ExecutorFactory* factory)   {
  if ( factory )      {
    m_executors.push_back(factory);
  }
}

/// Shutdown the whole stuff
void DataflowMgr::shutdown(bool print_stats, bool delete_factories)   {
  if ( print_stats ) printStatistics();
  for(Executors::iterator i = m_executors.begin(); i!=m_executors.end(); ++i)  {
    (*i)->shutdown(delete_factories);
    if ( delete_factories ) delete *i;
  }
}

/// Print execution statistics summary
void DataflowMgr::printStatistics()   const  {
  for(Executors::const_iterator i = m_executors.begin(); i!=m_executors.end(); ++i)
    (*i)->printStatistics();
  Timing tim;
  const char* line = "====================================================";
  tim.collect(m_start,tim.now());
  ::lib_rtl_output(LIB_RTL_ALWAYS,"SUMMARY %s%s\n",line,line); 
  m_time.print("TOTAL TIME");
  m_submit.print("Event submission");
  m_complete.print("Event completion");
  tim.print("Total ellapsed");
  tim.setCalls(m_time.numCalls());
  tim.print("Total ellapsed / Event");
}

/// Set new event context factory
void DataflowMgr::adoptContextFactory(EventContextFactory* factory)    {
  releasePtr(m_contextFactory);
  if ( factory )      {
    m_contextFactory = factory;
    m_contextFactory->declareData();
  }
}

Status DataflowMgr::resolveInputs()  {
  IOMask mask;
  mask.mask_or(m_contextFactory->dataMask());
  Executors tmp(m_executors);
  m_algMask.zero();
  while( !tmp.empty() )  {
    bool found = false;
    for(Executors::iterator i = tmp.begin(); i!=tmp.end(); ++i)  {
      Executors::value_type f = (*i);
      const IOMask& in = f->inputMask();
      if ( mask.match(in) )  {
        const IOMask& out = f->outputMask();
        f->setID(m_evtHandler.size());
        m_algMask.set_bit(f->id());
        m_evtHandler.push_back(f);
        mask.mask_or(out);
        tmp.erase(i);
        found = true;
        ::lib_rtl_output(LIB_RTL_INFO,MSG_SRC_FMT"Matched algorithm '%s'.\n",
          "DataflowMgr", this, f->name().c_str());
        cout << "    -->  " << left << setw(24) << "Data mask:" << hex;
        mask.dump("");
        cout << "    -->  " << left << setw(24) << "Output mask:" << hex;
        out.dump("");
        break;
      }
    }
    if ( !found && !tmp.empty() )  {
      const IOMask& evt_mask=m_contextFactory->dataMask();
      ::lib_rtl_output(LIB_RTL_ERROR,MSG_SRC_FMT"Failed to resolve algorithm dependencies.\n","DataflowMgr", this);
      ::lib_rtl_output(LIB_RTL_ERROR,MSG_SRC_FMT"These executors cannot be matched:\n","DataflowMgr", this);
      cout << "    -->  " << left << setw(24) << "INPUT:" << hex;
      evt_mask.dump("");
      for(Executors::iterator i = tmp.begin(); i!=tmp.end(); ++i)  {     
	Executors::value_type f = (*i);
	::lib_rtl_output(LIB_RTL_ERROR,MSG_SRC_FMT"Check executor %s:\n","DataflowMgr",this,f->name().c_str());
	dataRegistry().printMissing(f->inputMask(), mask);
      }
      for_each(tmp.begin(), tmp.end(), ExecutorFactory::PrintMissing(mask,LIB_RTL_ERROR));
      ::lib_rtl_output(LIB_RTL_INFO,MSG_SRC_FMT"Resolved dependencies:\n","DataflowMgr", this);
      for_each(m_evtHandler.begin(), m_evtHandler.end(), ExecutorFactory::Print(LIB_RTL_INFO));

      throw std::runtime_error("Failed to match Processors according to input data.");
      return Status::ERROR;
    }
  }
  ::lib_rtl_output(LIB_RTL_INFO,MSG_SRC_FMT"Resolved all algorithm dependencies.....\n","DataflowMgr", this);
  cout << "    -->  " << left << setw(24) << "INPUT:" << hex;
  m_contextFactory->dataMask().dump("      ");
  for_each(m_evtHandler.begin(), m_evtHandler.end(), ExecutorFactory::Print(LIB_RTL_INFO));
  return Status::SUCCESS;
}

/// Create worker threads. The routine returns, whean all threads are alive.
Status DataflowMgr::createThreads(size_t how_many)  {
  Status sc = Status::SUCCESS;
  m_starting = 0;
  m_state = STARTING;
  {
    Lock<DataflowMgr> lck(this);
    while( how_many > m_idleQue.size() )    {
      ++m_starting;
      Worker* w = new Worker(this);
      // Put the worker into the busy queue. They will move themself
      // into the idlequeu once the thread routine is activated.
      m_idleQue.insert(w);
      Status ret = w->start();
      if ( !ret.isSuccess() ) sc = ret;
    }
  }
  int ret = ::lib_rtl_wait_for_event(m_evt);
  if( !lib_rtl_is_success(ret) )  {
    int err = ::lib_rtl_get_error();
    ::lib_rtl_output(LIB_RTL_ERROR,MSG_SRC_FMT"Failed to wait for threads starting: [%d] %s \n",
      "DataflowMgr", this, err, RTL::errorString(err));
    return Status::ERROR;
  }
  return resume();
}

/// Stop the worker threads "softly". Let them finish the pending work first.
Status DataflowMgr::stop()   {
  ::lib_rtl_output(LIB_RTL_INFO,MSG_SRC_FMT "Stop event processing.\n", "DataflowMgr", this);
  m_state = STOPPING;
  while ( !m_events.empty() )  {
    ::lib_rtl_usleep(10);
  }
  ::lib_rtl_output(LIB_RTL_ALWAYS,MSG_SRC_FMT "Stop event processing Busy:%ld Idle:%ld.\n",
                   "DataflowMgr", this, m_idleQue.size(), m_busyQue.size());
  for(Workers::iterator i = m_idleQue.begin(); i != m_idleQue.end(); ++i)  {
    (*i)->queueStop();
  }
  while( !m_idleQue.empty() )  {
    Workers::iterator i = m_idleQue.begin();
    Worker* w = *i;
    m_idleQue.erase(i);
    w->release();
  }
  m_state = STOPPED;
  ::lib_rtl_output(LIB_RTL_ALWAYS,MSG_SRC_FMT "Stopped.\n","DataflowMgr", this);
  return Status::SUCCESS;
}

/// Match workers to executors as long as work and workers are availible
Status DataflowMgr::matchWork()   {
  // Won't work like this tough....
  EventContexts::iterator e;
  for(e=m_events.begin();  !m_idleQue.empty() && e != m_events.end(); ++e) {
    EventContext* evt_ctxt = *e;
    for(size_t i=0, n=m_evtHandler.size(); !m_idleQue.empty() && i<n; ++i)  {
      ExecutorFactory* f = m_evtHandler[i];
      const IOMask& mask = f->inputMask();
      if ( evt_ctxt->testAlgBit(i) && evt_ctxt->matchIO(mask) )  {
        Executor* ex = f->getFreeInstance();
        if ( ex )  {
          Lock<DataflowMgr> lck(this);
          Workers::iterator wi = m_idleQue.begin();
          Worker* w = (*wi);
          m_idleQue.erase(wi);
          m_busyQue.insert(w);
          evt_ctxt->clearAlgBit(i);
          Status sc = w->execute(ex, evt_ctxt);
        }
      }
    }
  }
  return Status::SUCCESS;
}

/// Scan the event queue and check if any of the events is already finished
Status DataflowMgr::finishWork()  {
  for(EventContexts::iterator e=m_events.begin(); e != m_events.end(); ) {
    EventContext* context = *e;
    // No more processors triggering: Need to eject this event....
    if ( context->doneMaskEmpty() )    {
      ::lib_rtl_output(LIB_RTL_INFO,MSG_SRC_FMT"Mark for deletion. Context:0x%p [%d]\n",
        "DataflowMgr", this, context, context->id());
      m_events.erase(e);
      e = m_events.begin();
      struct timeval tv = context->execTime();
      ::lib_rtl_output(LIB_RTL_ALWAYS,MSG_SRC_FMT "Event %p [%d] finished. removing after %d.%04d seconds.\n",
		       "DataflowMgr", this, context, context->id(), int(tv.tv_sec), int(tv.tv_usec/100));
      struct timeval now = Timing::now();
      m_time.collect(context->start(),now);
      m_complete.collect(m_fini,now);
      m_fini = now;
      delete context;
      continue;
    }
    ++e;
  }
  return Status::SUCCESS;
}

/// Access the next event in the loop
EventContext* DataflowMgr::nextEvent() {
  if ( 0 == m_contextFactory )  {
    ::lib_rtl_output(LIB_RTL_ERROR,MSG_SRC_FMT "No event context factory present.\n", 
      "DataflowMgr", this);
    return 0;
  }
  return m_contextFactory->create();
}

Status DataflowMgr::restart()   {
  m_state = RUNNING;
  return resume();
}

/// Main thread entry point: on resume will match workers
Status DataflowMgr::start(size_t num_evts, size_t max_evts_parallel)   {
  bool done = false;
  size_t nevts = 0;
  struct timeval now, last = Timing::now();
  Status status = Status::SUCCESS;
  m_fini = m_start = Timing::now();
  m_time.initialize();
  m_submit.initialize();
  m_complete.initialize();
  ::lib_rtl_output(LIB_RTL_INFO,MSG_SRC_FMT"Start event processing.\n", "DataflowMgr", this);
  while( lib_rtl_is_success(::lib_rtl_wait_for_event(m_evt)) ) {
    // First we clean-up events, which were already processed
    if ( m_monitor ) m_monitor->save();
    status = finishWork();
    if ( !status.isSuccess() ) {
      ::lib_rtl_output(LIB_RTL_INFO,MSG_SRC_FMT "Failed to process events.\n", 
		       "DataflowMgr", this);
      break;
    }
    // If there are no idle workers, no need to check for work
    if ( !m_idleQue.empty() )   {
      // Now, let's see it we can read in a new event
      if ( nevts < num_evts &&
           m_state == RUNNING && 
           m_events.size() < max_evts_parallel )   {
        Lock<DataflowMgr> lck(this);
        EventContext* ctxt = nextEvent();
        if ( ctxt )    {
          ::lib_rtl_output(LIB_RTL_ALWAYS,MSG_SRC_FMT "Event %p [%d] starting.\n", 
			   "DataflowMgr", this, ctxt, int(ctxt->id()));
	  now = Timing::now();
	  m_submit.collect(last,now);
	  last = now;
          ctxt->setAlgMask(m_algMask);
          m_events.push_back(ctxt);
          ++nevts;
        }
        else  {
          ::lib_rtl_output(LIB_RTL_ALWAYS,MSG_SRC_FMT "End of event input reached.\n", 
			   "DataflowMgr", this);
          done = true;
        }
      }
      status = matchWork();
      if ( !status.isSuccess() ) {
        ::lib_rtl_output(LIB_RTL_INFO,MSG_SRC_FMT "Failed to match processors.\n", 
			 "DataflowMgr", this);
        break;
      }
    }
    if ( nevts == num_evts ) { // debug only
      m_state = PAUSED;
    }
    // Termination condition: No more events to process
    if ( m_events.empty() )   {
      if ( nevts == num_evts )  {
        ::lib_rtl_output(LIB_RTL_ALWAYS,MSG_SRC_FMT "Requested number of events [%d] processed.\n",
			 "DataflowMgr", this, nevts);
        break;
      }
      else if ( done || m_state == PAUSED )  {
        ::lib_rtl_output(LIB_RTL_ALWAYS,MSG_SRC_FMT "Event processing terminating.\n",
			 "DataflowMgr", this);
        break;
      }
    }
  }
  return status;
}
