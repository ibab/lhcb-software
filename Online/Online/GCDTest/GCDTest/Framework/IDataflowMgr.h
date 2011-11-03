#ifndef FRAMEWORK_IDATAFLOWMGR_H
#define FRAMEWORK_IDATAFLOWMGR_H

// C/C++ include files
#include <ctime>

/*
 *    Framework namespace declaration
 */
namespace Framework {

  // Forward declarations
  class EventContext;
  class Executor;
  class Worker;

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
    /// Worker call: finish the work and put worker back into sleep mode _after_ call return
    virtual void workerDone(Worker* worker, 
			    Executor* executor, 
			    EventContext* context, 
			    const struct timeval& start, 
			    const struct timeval& stop) = 0;
  };
}      /* End namespace      */
#endif /* FRAMEWORK_IWORKER_H */
