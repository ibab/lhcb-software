#ifndef BITS_DATAFLOWMGR_H
#define BITS_DATAFLOWMGR_H

// Framework include files
#include "RTL/rtl.h"
#include "Framework/IOPort.h"
#include "Framework/Worker.h"
#include "Framework/Timing.h"
#include "Framework/IDataflowMgr.h"
#include "Framework/EventContext.h"

// C++ include files
#include <dispatch/dispatch.h>
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

  // #define _USE_WORKER_SET

  /** @class DataflowMgr DataflowMgr.h Framework/DataflowMgr.h
   *
   * @author  M.Frank
   * @version 1.0
   * @date    20/06/2011
   */
  class DataflowMgr : virtual public IDataflowMgr  {
    friend class Lock<DataflowMgr>;
  protected:
    typedef EventContext::DataMask               DataMask;
    typedef EventContext::AlgorithmMask          AlgorithmMask;
#ifdef _USE_WORKER_SET
    typedef std::set<Worker*>                    Workers;
#else
    typedef std::vector<Worker*>                 Workers;
#endif
    typedef std::list<EventContext*>             EventContexts;
    typedef std::vector<ExecutorFactory*>        Executors;

    /// Lock handle to lock the thread
    lib_rtl_lock_t       m_lock;
    /// Event handle to resume work matching
    lib_rtl_event_t      m_evt;
    /// Idle Worker container
    Workers              m_idleQue;
    /// Event context container defining pending events to be worked at
    EventContexts        m_events;
    /// Executor container to handle each pending event
    Executors            m_executors;
    /// Pointer to data registry
    ItemRegistry*        m_dataRegistry;
    /// Event context creator
    EventContextFactory* m_contextFactory;
    /// Monitoring instance
    Monitor*             m_monitor;
    /// Mask with all algorithms participating 
    AlgorithmMask        m_algMask;
    int                  m_print;

    struct timeval       m_start;
    struct timeval       m_fini;
    /// Execution time collector for complete events
    Timing               m_time;
    /// Execution time collector for submitting new events
    Timing               m_submit;
    /// Execution time collector for completed events
    Timing               m_complete;
    /// Handle to main worker group
    dispatch_group_t     m_group;
    /// Handle ro main worker queue
    dispatch_queue_t     m_queue;

    /// Access the next event in the loop
    EventContext* nextEvent();

    /// Scan the event queue and check if any of the events is already finished
    Status finishWork();

    /// Worker call: finish the work and put worker back into sleep mode _after_ call return
    virtual void workerDone(Worker* worker, 
			    Executor* executor, 
			    EventContext* context, 
			    const struct timeval& start,
			    const struct timeval& stop);

  public:
    /// Standard constructor. Thread is automatically started and put "on hold"
    explicit DataflowMgr(int print_level, Monitor* monitor);

    /// Default destructor. Thread is stopped
    virtual ~DataflowMgr();

    /// Check if there is at least one free worker availible
    bool haveFreeWorker() const;

    /// Allocate the next free worker
    Worker* allocateWorker();

    /// Access to the registry
    ItemRegistry& dataRegistry() const { return *m_dataRegistry; }

    DataMask ioMask(const char** items)  const;
    DataMask ioMask(const std::vector<std::string>& items)  const;

    /// Print execution statistics summary
    void printStatistics(size_t max_evts_parallel=0)   const;

    /// Shutdown the whole stuff
    void shutdown(bool print_statistics=false, bool delete_factories=true);

    /// Match workers to executors as long as work and workers are availible
    Status matchWork();

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

    /** Stop the worker threads "softly". Let them finish the pending work first.
    *
    *  @return  Status code indicating success or failure
    */
    Status stop();

    /** Resume execution and work matching after a "stop: command
    *
    *  @return  Status code indicating success or failure
    */
    Status resume();
  };
}
#endif // BITS_DATAFLOWMGR_H
