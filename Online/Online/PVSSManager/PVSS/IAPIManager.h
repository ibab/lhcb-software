#ifndef  PVSSMANAGER_IAPIMANAGER_H
#define  PVSSMANAGER_IAPIMANAGER_H

/*
 * PVSS namespace
 */
namespace PVSS {

  /** @class APIManager APIManager.h APIManager.h
    *
    *  Declaration of our APIManager-interface
    *
    *  @author  M.Frank
    *  @version 1.0
    *
    */
  class IAPIManager {
  public:
    /// Initialize manager
    virtual int initialize() = 0;
    /// Run the Manager: Execute PVSS dispatch loop
	  virtual void run() = 0;
    /// Execute PVSS dispatch loop in a seperate thread and return to command line
    virtual void start() = 0;
    /// Stop PVSS dispatch loop
    virtual void stop() = 0;
    /// Graceful exit with manager cleanup
    virtual void exit(int exit_code) = 0;
  };
}
#endif  /* PVSSMANAGER_IAPIMANAGER_H    */
