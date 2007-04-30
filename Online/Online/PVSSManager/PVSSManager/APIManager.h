#ifndef  PVSSMANAGER_APIMANAGER_H
#define  PVSSMANAGER_APIMANAGER_H

#include "PVSS/IAPIManager.h"
#include "Manager.hxx"
#include <string>


/*
 * PVSS namespace
 */
namespace PVSS {

  /** @class APIManager APIManager.h APIManager.h
    *
    *  Declaration of our APIManager-class
    *
    *  @author  M.Frank
    *  @version 1.0
    *
    */
  class APIManager : public Manager, virtual public IAPIManager    {
  public:
    /// Default constructor
    APIManager(const std::string& dll, const std::string& call);
    /// Initialize manager
    virtual int initialize();
    /// Run the Manager: Execute PVSS dispatch loop
    virtual void run();
    /// Execute PVSS dispatch loop in a seperate thread and return to command line
    virtual void start();
    /// Stop PVSS dispatch loop
    virtual void stop();
    /// Graceful exit with manager cleanup
    virtual void exit(int exit_code);
    /// Optional execution through loaded library
    virtual int exec(bool threaded=true);
 
    /// handle incoming hotlinks by group
    void handleHotLink(const DpHLGroup &group);
    
    
  private:
    /// callback from signal handler
    virtual void signalHandler(int sig);
    /// Bootstrap execution by loading external DLL
    std::string m_dll;
    /// Bootstrap execution by calling function in above DLL
    std::string m_call;
  };
}
#endif  // PVSSMANAGER_APIMANAGER_H
