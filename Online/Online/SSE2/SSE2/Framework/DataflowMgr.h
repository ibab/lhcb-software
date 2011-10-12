#ifndef BITS_DATAFLOWMGR_H
#define BITS_DATAFLOWMGR_H

// Framework include files
#include "Framework/IOPort.h"
#include "Framework/Worker.h"
#include "Framework/Timing.h"
#include "Framework/IDataflowMgr.h"
#include "Framework/EventContext.h"

// C++ include files
#include <vector>
#include <list>
#include <map>
#include <set>

/*
 *    Framework namespace declaration
 */
namespace Framework {

  // Forward declarations
  template <class T> class Lock;
  class EventContextFactory;
  class ExecutorFactory;
  class ItemRegistry;
  class Executor;
  class Monitor;
  class Worker;

  /** @class DataflowMgr DataflowMgr.h Framework/DataflowMgr.h
   *
   * @author  M.Frank
   * @version 1.0
   * @date    20/06/2011
   */
  class DataflowMgr : virtual public IDataflowMgr  {
    friend class Lock<DataflowMgr>;
  protected:
    typedef EventContext::Mask                   EventMask;
    typedef std::set<Worker*>                    Workers;
    typedef std::list<EventContext*>             EventContexts;
    typedef std::vector<ExecutorFactory*>        Executors;

    /// Lock handle to lock the thread
    lib_rtl_lock_t       m_lock;
    /// Event handle to resume work matching
    lib_rtl_event_t      m_evt;
    /// Idle Worker container
    Workers              m_idleQue;
    /// Busy Worker container
    Workers              m_busyQue;
    /// Event context container defining pending events to be worked at
    EventContexts        m_events;
    /// Executor container to handle each pending event
    Executors            m_executors;
    /// Executor container to handle each pending event
    Executors            m_evtHandler;
    /// Pointer to data registry
    ItemRegistry*        m_dataRegistry;
    /// Event context creator
    EventContextFactory* m_contextFactory;
    /// Monitoring instance
    Monitor*             m_monitor;
    EventContext::AlgorithmMask m_algMask;
    struct timeval       m_start;
    struct timeval       m_fini;
    /// Execution time collector for complete events
    Timing               m_time;
    /// Execution time collector for submitting new events
    Timing               m_submit;
    /// Execution time collector for completed events
    Timing               m_complete;
    /// State 
    enum State { OFFLINE=1, STARTING=2, RUNNING=3, PAUSED=4, STOPPING=5, STOPPED=6 };
    State                m_state;
    size_t               m_starting;

    Status lock();
    Status unlock();

    void push(Worker* worker);
    void push(Executor* worker);

    /// Internal hook called by the worker (through workerIdle) to start worker thread
    Status resume();

    /// Internal hook called by the worker (through workerIdle) to suspend worker thread
    Status suspend();
  
    /// Access the next event in the loop
    EventContext* nextEvent();

    /// Scan the event queue and check if any of the events is already finished
    Status finishWork();

    /// Worker call: Signal master, that the thread is ready to receive work
    virtual Status workerActive(Worker* worker);

    /// Worker call: finish the work and put worker back into sleep mode _after_ call return
    virtual Status workerDone(Worker* worker, 
			      Executor* executor, 
			      EventContext* context, 
			      const struct timeval& start,
			      const struct timeval& stop);

  public:
    /// Standard constructor. Thread is automatically started and put "on hold"
    explicit DataflowMgr();

    /// Default destructor. Thread is stopped
    virtual ~DataflowMgr();

    /// Access to the registry
    ItemRegistry& dataRegistry() const { return *m_dataRegistry; }

    /// Access manager state
    virtual State state() const        { return m_state;         }

    /// Set monitor entity
    void setMonitor(Monitor* mon)      { m_monitor = mon;        }

    /// Print execution statistics summary
    void printStatistics()   const;

    /// Shutdown the whole stuff
    void shutdown(bool print_statistics=false, bool delete_factories=true);

    /// Worker call: Signal master that the work is done and the worker is up for new stuff
    Status workerIdle(Worker* worker);

    /// Match workers to executors as long as work and workers are availible
    Status matchWork();

    /// Schedule next worker(s) according to mask of availible data and required algorithms
    Status match(EventContext& mask);

    /// Add new processor factory 
    void adoptFactory(ExecutorFactory* factory);

    /// Set new event context factory
    void adoptContextFactory(EventContextFactory* factory);

    /** Resolve the execution order of the algorithms according to the input requirements.
    *
    *  @return  Status code indicating success or failure
    */
    Status resolveInputs();

    /** Create worker threads. The routine returns, whean all threads are alive.
    *  @param   num_threads        [IN]   Number of events to process during this call
    *
    *  @return  Status code indicating success or failure
    */
    Status createThreads(size_t num_threads);

    /** Main thread entry point: on resume will match workers
    *  @param   num_evts           [IN]   Number of events to process during this call
    *  @param   max_evts_parallel  [IN]   Maximum number of events to be processed in parallel
    *
    *  @return  Status code indicating success or failure
    */
    Status start(size_t num_evts, size_t max_evts_parallel);

    Status restart();

    /** Stop the worker threads "softly". Let them finish the pending work first.
    *
    *  @return  Status code indicating success or failure
    */
    Status stop();
  };
}
#endif // BITS_DATAFLOWMGR_H
