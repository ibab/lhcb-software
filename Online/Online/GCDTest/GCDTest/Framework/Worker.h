#ifndef FRAMEWORK_WORKER_H
#define FRAMEWORK_WORKER_H

// Framework include files
#include "Framework/Status.h"

// C++ include files
//---None


/*
 *    Framework namespace declaration
 */
namespace Framework {

  // Forward declarations
  template <class T> class Lock;
  class DataflowMgr;
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
    friend class DataflowMgr;

  protected:
    /// reference to master dataflow manager
    IDataflowMgr*    m_master;
    /// Unique worker identifier
    int              m_id;
    struct Args {
      /// Reference to self
      Worker*          worker;
      /// Reference to current executor
      Executor*        executor;
      /// Reference to the current event context
      EventContext*    context;
      /// Constructor
      Args* operator()(Executor* e, EventContext* c) { 
	executor = e;
	context = c;
	return this;
      }
    } args;

  protected:
    /// Standard constructor. Thread is automatically started and put "on hold"
    Worker(IDataflowMgr* master, int id) : m_master(master), m_id(id)  {  args.worker = this;  }

    /// Default destructor. Thread will be stopped if not already exited
    virtual ~Worker() {}

    /// Access to worker identifier
    int id() const {  return m_id; }

    /// Thread running routine: here the work load is executed by the thread
    void run(Executor* executor, EventContext* context);

    /// Static GCD thread startup routing
    static void exec(void* arg);
  };

}      /* End namespace      */
#endif /* FRAMEWORK_WORKER_H */
