#include "RTL/Lock.h"
#include "Framework/Kernel.h"
#include "Framework/Worker.h"
#include "Framework/Monitor.h"
#include "Framework/Helpers.h"
#include "Framework/IExecutor.h"
#include "Framework/DataflowMgr.h"
#include "Framework/ItemRegistry.h"

#include <stdexcept>
#include <cstring>
#include <string>

using namespace Framework;
using namespace std;
#define FIRST_EVENT 0

#define OUTPUT(x)  MSG_SRC_FMT x, "DataflowMgr", this

/// Standard constructor. Thread is automatically started and put "on hold"
DataflowMgr::DataflowMgr(int print_level, Monitor* monitor) 
  : m_lock(0), m_dataRegistry(0), m_contextFactory(0), m_monitor(monitor), m_print(print_level)
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
  m_group = ::dispatch_group_create();
  m_queue = ::dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT,NULL);
}

/// Default destructor. Thread is stopped
DataflowMgr::~DataflowMgr() {
  deletePtr(m_contextFactory);
  deletePtr(m_dataRegistry);
  ::lib_rtl_delete_event(m_evt);
  m_evt = 0;
  ::lib_rtl_delete_lock(m_lock);
  m_lock = 0;
}

/// Check if there is at least one free worker availible
bool DataflowMgr::haveFreeWorker()   const {
#ifdef _USE_WORKER_SET
  return !m_idleQue.empty();
#else
  for(Workers::const_iterator w=m_idleQue.begin(); w != m_idleQue.end();++w)
    if ( *w ) return true;
  return false;
#endif
}

/// Allocate the next free worker
Worker* DataflowMgr::allocateWorker() {
#ifdef _USE_WORKER_SET
  if ( !m_idleQue.empty() )  { // Keep lock as local as possible
    RTL::Lock lock(m_lock);
    Workers::iterator w = m_idleQue.begin();
    Worker* worker = (*w);
    m_idleQue.erase(w);
    return worker;
  }
#else
  for(Workers::iterator w = m_idleQue.begin(); w != m_idleQue.end(); ++w) {
    if ( *w )  {
      Worker* worker = (*w);
      (*w) = 0;
      //m_idleQue[worker->id()] = 0;
      return worker;
    }
  }
#endif
  return 0;
}

DataflowMgr::DataMask DataflowMgr::ioMask(const char** items)  const {
  vector<string> v;
  for(size_t i=0; items[i]; ++i) v.push_back(items[i]);
  return ioMask(v);
}

DataflowMgr::DataMask DataflowMgr::ioMask(const vector<string>& items)  const {
  return DataMask(m_dataRegistry->registerItems(items));
}

/// Resume execution and work matching
Status DataflowMgr::resume() {
  ::lib_rtl_output(LIB_RTL_DEBUG,MSG_SRC_FMT"resume event processing ...","DataflowMgr", this);
  int status = ::lib_rtl_set_event(m_evt);
  return lib_rtl_is_success(status) ? Status::SUCCESS : Status::ERROR;
}

/// Worker call: finish the work and put worker back into sleep mode _after_ call return
void DataflowMgr::workerDone(Worker* worker,
			     Executor* executor,
			     EventContext* context, 
			     const struct timeval& start,
			     const struct timeval& stop)
{
  {
    RTL::Lock lock(m_lock);
    if ( lock )     {
#ifdef _USE_WORKER_SET
      m_idleQue.insert(worker);
#else
  m_idleQue[worker->id()] = worker;
#endif
  ::lib_rtl_output(LIB_RTL_INFO,"Worker Id:%d finished execution.",worker->id()); 
  if ( context->id() > FIRST_EVENT ) {
    if ( executor ) {
      executor->monitor(start,stop);
    }
    if ( m_monitor ) {
      m_monitor->monitor(executor,context,start,stop);
    }
  }
  context->addData(executor->outputMask());
  context->clearDoneBit(executor->id());
  executor->returnInstance();
  resume();
    }
  }
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
  for(Executors::iterator i = m_executors.begin(); i!=m_executors.end(); ++i)
    (*i)->shutdown(delete_factories);
  if ( delete_factories ) deleteContainer(m_executors);
}

/// Print execution statistics summary
void DataflowMgr::printStatistics(size_t max_evts_parallel)   const  {
  size_t n_thr = m_idleQue.size();
  for(Executors::const_iterator i = m_executors.begin(); i!=m_executors.end(); ++i) {
    ExecutorFactory::Statistics stat = (*i)->statistics();
    (*i)->print(m_print,stat);
    if ( m_monitor ) {
      m_monitor->saveSummary((*i)->name(),(*i)->id(),max_evts_parallel,n_thr,stat.first,stat.second);
    }
  }
  Timing tim;
  const char* line = "====================================================";
  tim.collect(m_start,tim.now());
  ::lib_rtl_output(LIB_RTL_ALWAYS," SUMMARY %s%s",line,line); 
  m_time.print("TOTAL TIME");
  m_submit.print("Event submission");
  m_complete.print("Event completion");
  tim.print("Total ellapsed");
  tim.setCalls(m_time.numCalls());
  tim.print("Total ellapsed / Event");
  if ( m_monitor ) {
    m_monitor->saveSummary("Event processing",    1000000, max_evts_parallel, n_thr, 1, m_time);
    m_monitor->saveSummary("Event submission",    1000001, max_evts_parallel, n_thr, 1, m_submit);
    m_monitor->saveSummary("Event completion",    1000002, max_evts_parallel, n_thr, 1, m_complete);
    m_monitor->saveSummary("Total Ellapsed Time", 1000003, max_evts_parallel, n_thr, 1, tim);
  }
}

/// Set new event context factory
void DataflowMgr::adoptContextFactory(EventContextFactory* factory)    {
  deletePtr(m_contextFactory);
  if ( factory )      {
    m_contextFactory = factory;
  }
}

Status DataflowMgr::resolveInputs()  {
  IOMask input;
  input.mask_or(m_contextFactory->dataMask());
  Executors tmp(m_executors);
  m_algMask.zero();
  m_executors.clear();
  while( !tmp.empty() )  {
    bool found = false;
    for(Executors::iterator i = tmp.begin(); i!=tmp.end(); ++i)  {
      Executors::value_type f = (*i);
      const IOMask& in = f->inputMask();
      if ( input.match(in) )  {
        const IOMask& out = f->outputMask();
        f->setID(m_executors.size());
        m_algMask.set_bit(f->id());
        m_executors.push_back(f);
        input.mask_or(out);
        tmp.erase(i);
        found = true;
        ::lib_rtl_output(LIB_RTL_DEBUG,OUTPUT("Matched algorithm '%s'."),_T(f->name()));
        ::lib_rtl_output(LIB_RTL_DEBUG,OUTPUT("    -->  %-24s %s"),"Input  mask:",_T(input.toString()));
        ::lib_rtl_output(LIB_RTL_DEBUG,OUTPUT("         %-24s %s"),"Output mask:",_T(out.toString()));
        break;
      }
    }
    if ( !found && !tmp.empty() )  {
      const IOMask& evt_mask=m_contextFactory->dataMask();
      ::lib_rtl_output(LIB_RTL_ERROR,OUTPUT("Failed to resolve algorithm dependencies."));
      ::lib_rtl_output(LIB_RTL_ERROR,OUTPUT("These executors cannot be matched:"));
      ::lib_rtl_output(LIB_RTL_INFO,OUTPUT("    -->  %-24s %s"),"INPUT:",_T(evt_mask.toString()));
      for(Executors::iterator i = tmp.begin(); i!=tmp.end(); ++i)  {     
	Executors::value_type f = (*i);
	::lib_rtl_output(LIB_RTL_ERROR,OUTPUT("Check executor %s:"),_T(f->name()));
	dataRegistry().printMissing(f->inputMask(), input);
      }
      for(Executors::const_iterator i=m_executors.begin(); i!=m_executors.end();++i)  {
	const ExecutorFactory* f = *i;
	char text[1024];
	vector<size_t> bits = input.missing(f->inputMask());
	::lib_rtl_output(LIB_RTL_ALWAYS,"    -->  %-24s Input  %s",_T(f->name()),_T(f->inputMask().toString()));
	text[0] = 0;
	for(size_t i=0,j=0; i<bits.size();++i,j=::strlen(text)) 
	  ::snprintf(text+j,sizeof(text)-j,"%d ",int(bits[i]));
	text[sizeof(text)-1]=0;
	::lib_rtl_output(LIB_RTL_ALWAYS,"         %-24s %s","Missing bits:",text);
	::lib_rtl_output(LIB_RTL_ALWAYS,"         %-24s Output %s","",_T(f->outputMask().toString()));
      }
      ::lib_rtl_output(LIB_RTL_INFO,OUTPUT("Resolved dependencies:"));
      for(Executors::const_iterator i=m_executors.begin(); i!=m_executors.end();++i)  {
	const ExecutorFactory* f = *i;
	::lib_rtl_output(LIB_RTL_INFO,"    -->  %-24s Input  %s",_T(f->name()),_T(f->inputMask().toString()));
	::lib_rtl_output(LIB_RTL_INFO,"         %-24s Output %s","",_T(f->outputMask().toString()));
      }
      throw std::runtime_error("Failed to match Processors according to input data.");
    }
  }
  ::lib_rtl_output(LIB_RTL_INFO,OUTPUT("Resolved all algorithm dependencies....."));
  ::lib_rtl_output(LIB_RTL_INFO,OUTPUT("    -->  %-24s"),"INPUT",_T(m_contextFactory->dataMask().toString()));
  for(Executors::const_iterator i=m_executors.begin(); i!=m_executors.end();++i)  {
    const ExecutorFactory* f = *i;
    ::lib_rtl_output(LIB_RTL_INFO,"    -->  %-24s Input  %s",_T(f->name()),_T(f->inputMask().toString()));
    ::lib_rtl_output(LIB_RTL_INFO,"         %-24s Output %s","",_T(f->outputMask().toString()));
  }
  return Status::SUCCESS;
}

/// Create worker threads. The routine returns, whean all threads are alive.
Status DataflowMgr::createThreads(size_t how_many)  {
#ifdef _USE_WORKER_SET
  {
    RTL::Lock lock(m_lock);
    while( how_many > m_idleQue.size() )    {
      Worker* w = new Worker(this, int(m_idleQue.size()));
      m_idleQue.insert(w);
    }
  }
#else
  m_idleQue.resize(how_many);
  for(size_t i=0; i<how_many; ++i)
    m_idleQue[i] = new Worker(this, int(i));
#endif
  return resume();
}

/// Stop the worker threads "softly". Let them finish the pending work first.
Status DataflowMgr::stop()   {
  ::lib_rtl_output(LIB_RTL_INFO,OUTPUT("Stop event processing."));
  while ( !m_events.empty() )  {
    ::lib_rtl_usleep(10);
  }
  ::lib_rtl_output(m_print,OUTPUT("Stop event processing. All %ld threads are idle."),m_idleQue.size());
  ::dispatch_group_wait(m_group,DISPATCH_TIME_FOREVER);
  while( !m_idleQue.empty() )  {
    Workers::iterator i = m_idleQue.begin();
    Worker* w = *i;
    m_idleQue.erase(i);
    delete w;
  }
  ::lib_rtl_output(m_print,OUTPUT("Stopped."));
  return Status::SUCCESS;
}

/// Match workers to executors as long as work and workers are availible
Status DataflowMgr::matchWork()   {
  // Won't work like this tough....
  for(EventContexts::iterator e=m_events.begin(); e != m_events.end(); ++e) {
    EventContext* context = *e;
    for(size_t i=0, n=m_executors.size(); i<n; ++i)  {
      if ( haveFreeWorker() )  {
	ExecutorFactory* f = m_executors[i];
	const IOMask& mask = f->inputMask();
	if ( context->testAlgBit(i) && context->matchIO(mask) )  {
	  Executor* executor = f->getFreeInstance();
	  if ( executor )  {
	    Worker* worker = allocateWorker();
	    if ( worker ) {
	      context->clearAlgBit(i);
	      ::dispatch_group_async_f(m_group,m_queue,worker->args(executor,context),Worker::exec);
	      ::lib_rtl_output(LIB_RTL_DEBUG,OUTPUT("Submitted. Executor:0x%p Context:0x%p."),
			       executor, context);
	    }
	  }
        }
      }
      else {
	return Status::SUCCESS;
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
      ::lib_rtl_output(LIB_RTL_INFO,OUTPUT("Mark for deletion. Context:0x%p [%d]"),
		       context, context->id());
      m_events.erase(e);
      e = m_events.begin();
      struct timeval tv = context->execTime();
      ::lib_rtl_output(m_print,OUTPUT("Event %p [%d] finished. removing after %d.%04d seconds."),
		       context, context->id(), int(tv.tv_sec), int(tv.tv_usec/100));
      struct timeval now = Timing::now();
      if ( context->id() > FIRST_EVENT ) {
	m_time.collect(context->start(),now);
	if ( m_monitor ) m_monitor->add("TotalExecutionTime",100001,context->id(),context->start(),now);
	m_complete.collect(m_fini,now);
      }
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
  if ( m_contextFactory )  {
    return m_contextFactory->create();
  }
  ::lib_rtl_output(LIB_RTL_ERROR,OUTPUT("No event context factory present."));
  return 0;
}

/// Main thread entry point: on resume will match workers
Status DataflowMgr::start(size_t num_evts, size_t max_evts_parallel)   {
  enum State { OFFLINE=1, STARTING=2, RUNNING=3, PAUSED=4, STOPPING=5, STOPPED=6 };
  struct timeval now, last = Timing::now();
  Status status = Status::SUCCESS;
  State state = RUNNING;
  bool done = false;
  size_t nevts = 0;

  m_fini = m_start = Timing::now();
  m_time.initialize();
  m_submit.initialize();
  m_complete.initialize();
  ::lib_rtl_output(LIB_RTL_INFO,OUTPUT("Start event processing."));
  while( lib_rtl_is_success(::lib_rtl_wait_for_event(m_evt)) ) {
    // First we clean-up events, which were already processed
    status = finishWork();
    if ( !status.isSuccess() ) {
      ::lib_rtl_output(LIB_RTL_INFO,MSG_SRC_FMT "Failed to process events.", 
		       "DataflowMgr", this);
      break;
    }
    bool have_workers = haveFreeWorker();
    ::lib_rtl_output(LIB_RTL_INFO,OUTPUT("Dispatching new work... workers availible:%s"),
		     have_workers ? "YES" : "NO");
    // If there are no idle workers, no need to check for work
    if ( have_workers )    {
      // Now, let's see it we can read in a new event
      if ( nevts < num_evts &&
           state == RUNNING && 
           m_events.size() < max_evts_parallel )   {
	RTL::Lock lock(m_lock);
        EventContext* context = nextEvent();
        if ( context )    {
          ::lib_rtl_output(m_print,OUTPUT("Event %p [%d] starting."),context,int(context->id()));
	  now = Timing::now();
	  if ( context->id() > FIRST_EVENT ) {
	    m_submit.collect(last,now);
	    if ( m_monitor ) m_monitor->add("SubmitDelayTime",100002,context->id(),last,now);
	  }
	  last = now;
          context->setAlgMask(m_algMask);
          m_events.push_back(context);
          ++nevts;
        }
        else  {
          ::lib_rtl_output(LIB_RTL_ALWAYS,OUTPUT("End of event input reached."));
          done = true;
        }
      }
      status = matchWork();
      if ( !status.isSuccess() ) {
        ::lib_rtl_output(LIB_RTL_INFO,OUTPUT("Failed to match processors."));
        break;
      }
    }
    if ( nevts == num_evts ) { // debug only
      state = PAUSED;
    }
    // Termination condition: No more events to process
    if ( m_events.empty() )   {
      if ( nevts == num_evts )  {
        ::lib_rtl_output(LIB_RTL_ALWAYS,OUTPUT("Requested number of events [%d] processed."),nevts);
        break;
      }
      else if ( done || state == PAUSED )  {
        ::lib_rtl_output(LIB_RTL_ALWAYS,OUTPUT("Event processing terminating."));
        break;
      }
    }
  }
  return status;
}
