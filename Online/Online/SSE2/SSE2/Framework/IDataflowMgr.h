#ifndef FRAMEWORK_IDATAFLOWMGR_H
#define FRAMEWORK_IDATAFLOWMGR_H

// Framework include files
#include "Framework/Status.h"

/*
 *    Framework namespace declaration
 */
namespace Framework {


  // Forward declarations
  class EventContext;
  class Executor;
  class Worker;
  class IDataflowMgr;

  /**@class IDataflowMgr IDataflowMgr.h Framework/IDataflowMgr.h
   *
   *  @author  M.Frank
   *  @version 1.0
   *  @date    10/06/2011
   *
   */
  class IDataflowMgr   {
  protected:
    /// Default destructor - not to be called publicly
    virtual ~IDataflowMgr() {}
  public:
    /// Worker call: Signal master, that the thread is ready to receive work
    virtual Status workerActive(Worker* worker) = 0;
    /// Worker call: finish the work and put worker back into sleep mode _after_ call return
    virtual Status workerDone(Worker* worker, Executor* executor, EventContext* context, const struct timeval& start, const struct timeval& stop) = 0;
  };
}      /* End namespace      */
#endif /* FRAMEWORK_IWORKER_H */
