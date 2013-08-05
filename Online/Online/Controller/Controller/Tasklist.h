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
#ifndef ONLINE_CONTROLLER_TASKLIST_H
#define ONLINE_CONTROLLER_TASKLIST_H

// C/C++ include files
#include <string>
#include <vector>
#include <map>

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine   {

  /**@class Tasklist  Tasklist.h Controller/Tasklist.h
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  struct Tasklist  {
  public:
    struct Timeout {
      std::string action;
      int timeout;
      Timeout() : timeout(0) {}
      Timeout(const std::string& a, int t) : action(a), timeout(t) {}
      Timeout(const Timeout& c) : action(c.action), timeout(c.timeout) {}
      Timeout& operator=(const Timeout& c) { 
	if ( &c != this )  {
	  action=c.action;
	  timeout=c.timeout;
	}
	return *this;
      }
    };
    /// Defintiion of the timeout container
    typedef std::vector<Timeout> Timeouts;

    /// Definition of the parameter container
    typedef std::pair<std::string,std::string> Param;
    typedef std::vector<Param> Params;

    /** @class Task  Tasklist.h Controller/Tasklist.h
     *
     * @author  M.Frank
     * @date    01/03/2013
     * @version 0.1
     */
    struct Task {
      /// Task name
      std::string name;
      /// UTGID
      std::string utgid;
      /// Task user name
      std::string user;
      /// Task group name
      std::string group;
      /// Task multiplier
      size_t      instances;
      /// Task command
      std::string command;
      /// Map of FMC startup arguments
      Params fmcParams;
      /// Map of I/O parameters
      Params ioParams;
      /// Map of program arguments
      Params args;
      /// Timeouts for this task type
      Timeouts timeouts;

      /// Constructor
      Task();
      /// Standard destructor
      virtual ~Task();
      /// String with the FMC parameters propery concatenated
      std::string fmcStartParams() const;
      /// String with the argument list propery concatenated
      std::string arguments() const;
    };

    /// Definition of the task container
    typedef std::vector<Task*> Tasks;

    /// Container of tasks used for this configuration
    Tasks tasks;
    
    public:
    /// Constructor
    Tasklist();
    /// Standard destructor
    virtual ~Tasklist();
    /// End-iterator over the task container
    Tasks::const_iterator end()   const { return tasks.end(); }
    /// Begin-iterator over the task container
    Tasks::const_iterator begin() const { return tasks.begin(); }
  };   //  End class Tasklist
}      //  End namespace 
#endif //  ONLINE_CONTROLLER_TASKLIST_H

