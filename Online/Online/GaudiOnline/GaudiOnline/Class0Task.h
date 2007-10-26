#ifndef ONLINE_GAUDIONLINE_CLASS0TASK_H
#define ONLINE_GAUDIONLINE_CLASS0TASK_H

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
  class Class0Task : public GaudiTask {
  public:
    /// Standard constructor
    Class0Task(IInterface* svc);
    /// Default destructor
    virtual ~Class0Task();
    /// Callback on configure transition
    virtual StatusCode configure();
    /// Callback on reset transition
    virtual StatusCode terminate();
  };
}

#endif // ONLINE_GAUDIONLINE_CLASS0TASK_H
