#ifndef FRAMEWORK_WORKER_H
#define FRAMEWORK_WORKER_H

// Framework include files
#include "RTL/rtl.h"
#include "Framework/Status.h"

// C++ include files
//---None


/*
 *    Framework namespace declaration
 */
namespace Framework {

  // Forward declarations
  template <class T> class Lock;
  class IDataflowMgr;
  class EventContext;
  class Executor;
  class Worker;

  /**@class Worker Worker.h Framework/Worker.h
   *
   *  @author  M.Frank
   *  @version 1.0
   *  @date    10/06/2011
   *
   */
  class Worker   {
    friend class Lock<Worker>;
  public:
    enum State { OFFLINE=0, ACTIVE=1, EXECUTING=2, ENDED=3 };
  protected:
    /// Lock handle to halt/resume the thread
    // lib_rtl_lock_t   m_lock;
    lib_rtl_event_t  m_lock;
    /// Thread handle of the underlying process
    lib_rtl_thread_t m_thread;
    /// reference to master dataflow manager
    IDataflowMgr*    m_master;
    /// Reference to the current event context
    EventContext*    m_context;
    /// Reference to current executor
    Executor*        m_executor;
    /// Counter to monitor execution cycles
    long long int    m_wakeup;
    /// Flag to indicate execution stop (== worker state)
    State            m_state;

  protected:
    /// Assign event context to current worker
    void setContext(EventContext* context)  { m_context = context;   }
    /// Setup executor
    void setExecutor(Executor* executor)    { m_executor = executor; }
    /// Internal hook called by the master thread to suspend worker thread
    Status lock();
    /// Internal hook called by the master thread to start worker thread
    Status unlock();
    /// Thread running routine: here the work load is executed by the thread
    void run();
    /// Static thread startup routing
    static int exec_thread(void* arg);

  public:

    /// Standard constructor. Thread is automatically started and put "on hold"
    Worker(IDataflowMgr* master);

    /// Default destructor. Thread will be stopped if not already exited
    virtual ~Worker();
    
    /// Master call [IDataflowMgr]: Setup worker thread and resume execution mode
    Status execute(Executor* executor, EventContext* context);

    /// Master call [IDataflowMgr]: Access worker state
    State state() const        { return m_state;         }

    /// Master call [IDataflowMgr]: Start the thread
    Status start();

    /// Master call [IDataflowMgr]: Stop the worker from processing once idle
    Status queueStop();

    /// Master call: Suspend the processing loop of this worker
    Status suspend();

    /// Master call [IDataflowMgr]: Wait until worker has stopped processing, then delete it
    Status release();
  };

}      /* End namespace      */
#endif /* FRAMEWORK_WORKER_H */
