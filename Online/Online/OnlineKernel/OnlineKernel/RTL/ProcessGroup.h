// $Header: $
// ====================================================================
//
// --------------------------------------------------------------------
//
// Author    : Markus Frank
//
// ====================================================================
#ifndef ONLINEKERNEL_RTL_PROCESSGROUP_H
#define ONLINEKERNEL_RTL_PROCESSGROUP_H
// $ID:

// Framework include files
#include "RTL/Process.h"

// C++ include files
#include <vector>
#include <string>

///  RTL (Run-Time-Library) namespace declaration
namespace RTL {

  // Forward declarations
  class Process;

  /// Class, which allows to manipulate entire groups of processes
  /** 
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  class ProcessGroup : public Process {
  public:
    /// Definition of process container
    typedef std::vector<Process*> Processes;
  protected:
    /// Container of managed processes
    Processes m_procs;

  public:
    /// Standard constructor
    ProcessGroup();
    /// Standard destructor
    virtual ~ProcessGroup();
    /// Get a process by PID
    Process* get(int pid) const;
    /// Add a new process
    void add(Process* p);
    /// Remove a process from the list
    int remove(const std::string& name, Process** proc=0);
    /// Remove all processes
    int removeAll();
    /// Start process group
    virtual int start();
    /// Start process
    virtual int start(bool new_process_group);
    /// Wait for process group.end
    virtual int wait(int flag=WAIT_NOBLOCK);
  };
} // End namespace RTL
#endif  // ONLINEKERNEL_RTL_PROCESSGROUP_H
