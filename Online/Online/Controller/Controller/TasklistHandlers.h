/*============================================================
**
**      Finite state machine implementation to control 
**      and manipulate process groups
**
**  AUTHORS:
**
**      M.Frank  CERN/LHCb
**
**==========================================================*/
#ifndef ONLINE_CONTROLLER_TASKLISTHANDLERS_H
#define ONLINE_CONTROLLER_TASKLISTHANDLERS_H

// Framework include files
#include "Controller/Tasklist.h"

// Forward declarations
namespace DD4hep { namespace XML { struct Handle_t; }}
// Forward declarations
typedef DD4hep::XML::Handle_t xml_h;

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine   {

  /**@class TasklistPrinter  Tasklisthandlers.h Controller/Tasklisthandlers.h
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  struct TasklistPrinter  {  
    struct Args      {  void operator()(const xml_h& h);  };
    struct Params    {  void operator()(const xml_h& h);  };
    struct Timeouts  {  void operator()(const xml_h& h);  };
    void operator()(const xml_h& h);
  };

  /**@class TasklistAnalyzer  Tasklisthandlers.h Controller/Tasklisthandlers.h
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  struct TasklistAnalyzer  {  
    /// Definition of the task type
    typedef Tasklist::Task Task;
    /**@class TasklistAnalyzer::Args  Tasklisthandlers.h Controller/Tasklisthandlers.h
     *
     * @author  M.Frank
     * @date    01/03/2013
     * @version 0.1
     */
    struct Base      {  
      /// Reference to current task object
      Task* task;  
    public:
      /// Constructor
      Base(Task* t);
    };
    /**@class TasklistAnalyzer::Args  Tasklisthandlers.h Controller/Tasklisthandlers.h
     *
     * @author  M.Frank
     * @date    01/03/2013
     * @version 0.1
     */
    struct Args : public Base  {
      /// Constructor
      Args(Task* t);
      /// Action operator when analyzing data
      void operator()(const xml_h& h);
    };
    /**@class TasklistAnalyzer::Params  Tasklisthandlers.h Controller/Tasklisthandlers.h
     *
     * @author  M.Frank
     * @date    01/03/2013
     * @version 0.1
     */
    struct Params : public Base   {
      /// Constructor
      Params(Task* t);
      /// Action operator when analyzing data
      void operator()(const xml_h& h);   
    };
    /**@class TasklistAnalyzer::Timeouts  Tasklisthandlers.h Controller/Tasklisthandlers.h
     *
     * @author  M.Frank
     * @date    01/03/2013
     * @version 0.1
     */
    struct Timeouts : public Base   {
      /// Constructor
      Timeouts(Task* t);
      /// Action operator when analyzing data
      void operator()(const xml_h& h);   
    };
    /// Reference to task list
    Tasklist& tasks;
    /// Constructor
    TasklistAnalyzer(Tasklist& t);
    /// Action operator when analyzing data
    void operator()(const xml_h& h);
  };

  /// Constructor
  inline TasklistAnalyzer::TasklistAnalyzer(Tasklist& t) : tasks(t) {}
  /// Constructor
  inline TasklistAnalyzer::Base::Base(Task* t) : task(t)       {}
  /// Constructor
  inline TasklistAnalyzer::Args::Args(Task* t) : Base(t)       {}
  /// Constructor
  inline TasklistAnalyzer::Params::Params(Task* t) : Base(t)   {}
  /// Constructor
  inline TasklistAnalyzer::Timeouts::Timeouts(Task* t) : Base(t)   {}
}      //  End namespace 
#endif //  ONLINE_CONTROLLER_TASKLISTHANDLERS_H

