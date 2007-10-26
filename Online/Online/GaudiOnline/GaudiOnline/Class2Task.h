#ifndef ONLINE_GAUDIONLINE_CLASS2TASK_H
#define ONLINE_GAUDIONLINE_CLASS2TASK_H

// Framework includes
#include "GaudiOnline/GaudiTask.h"

/*
 * LHCb namespace declaration
 */
namespace LHCb {
  /** @class Class0Task Class0Task.h GaudiOnline/Class0Task.h
    *
    * Object implementing the functionality of a Class 0 task
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class Class2Task : public GaudiTask {
  public:
    /// Standard constructor
    Class2Task(IInterface* svc);
    /// Default destructor
    virtual ~Class2Task() {}
    /// Callback on start transition
    virtual StatusCode initialize();
    /// Callback on stop transition
    virtual StatusCode finalize();
  };
}

#endif // ONLINE_GAUDIONLINE_CLASS2TASK_H
