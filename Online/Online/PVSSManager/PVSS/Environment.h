#ifndef PVSS_ENVIRONMENT_H
#define PVSS_ENVIRONMENT_H


/*
 * PVSS namespace
 */
namespace PVSS {

  // Framework forward declarations  
  class DevAnswer;

  /** @class Environment Environment.h PVSS/Environment.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *
    */
  class Environment  {
    /// Standard (private) singleton constructor)
    Environment();
  public:
    /// Singleton accessor
    static Environment& instance();
    /// Wait until command completed
    bool waitForAnswer(DevAnswer* answer);
  };
}
#endif  /* PVSS_ENVIRONMENT_H  */
