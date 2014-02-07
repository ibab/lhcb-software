#ifndef ONLINEKERNEL_RTL_PROCESS_H
#define ONLINEKERNEL_RTL_PROCESS_H

// $ID:
// C++ include files
#include <vector>
#include <string>

/*
 *   RTL namespace declaration
 */
namespace RTL {
  /** @class Process Process.h RTL/Process.h
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  class Process {
  public:
    typedef std::vector<std::string> Arguments;
    enum { INVALID=0, RUNNING, ENDED, STOPPED };
    enum { WAIT_BLOCK, WAIT_NOBLOCK };
  protected:
    /// Process identifier
    int m_pid;
    /// Exit status of this process
    int m_status;
    /// Process state
    int m_state;
    /// Process name (=executable name if not set)
    std::string m_name;
    /// Processes executable
    std::string m_exec;
    /// Output device name
    std::string m_output;
    /// Startup arguments
    Arguments m_args;

    /// Send signal to process specified by m_pid
    virtual int sendSignal(int sig);
    /// Send signal to process specified by -m_pid
    virtual int sendSignalAll(int sig);

  public:
    /// Standard constructor
    Process();
    /// Constructor to explore existing process
    explicit Process(int pid);
    /// Constructor to start new process
    Process(const std::string& name, const std::string& exe, const char* a[]);
    /// Constructor to start new process
    Process(const std::string& name, const std::string& exe, const char* a[], const std::string& o);
    /// Constructor to start new process
    Process(const std::string& name, const std::string& exe, const Arguments& a);
    /// Constructor to start new process
    Process(const std::string& name, const std::string& exe, const Arguments& a, const std::string& o);

    /// Standard destructor
    virtual ~Process();

    /// Debugging flag
    static void setDebug(bool val);
    /// Access to debugging flag
    static bool debug();

    /// Access process identifier
    int pid()  const { return m_pid; }
    /// Access process name
    const std::string& name() const {  return m_name; }
    /// Access startup arguments
    const Arguments& args() const { return m_args; }
    /// Access process name
    void setName(const std::string& val) {  m_name = val; }
    /// Access startup arguments
    void setArgs(const Arguments& val)  { m_args = val; }
    /// Access output device
    const std::string& output() const { return m_output; }
    /// Access process output device
    void setOutput(const std::string& val) {  m_output = val; }
    
    /// Start process
    virtual int start();
    /// Send a signal to the process
    virtual int signal(int signum);
    /// Send a signal to the process and all its children (SIGTERM)
    virtual int signalall(int signum);
    /// Terminate the process (SIGTERM)
    virtual int stop();
    /// Terminate the process and all its children (SIGTERM)
    virtual int stopall();
    /// Kill the process (SIGKILL)
    virtual int kill();
    /// Kill the process and its children (SIGKILL)
    virtual int killall();
    /// Interrupt the process (SIGINT)
    virtual int interrupt();
    /// Wait for process to terminate
    virtual int wait(int flag=WAIT_NOBLOCK);
  };
} // End namespace RTL
#endif  // ONLINEKERNEL_RTL_PROCESS_H
