//====================================================================
//	Unit test class functionality
//--------------------------------------------------------------------
//
//	Author     : M.Frank
//  History    :
// +---------+----------------------------------------------+---------
// |    Date |                 Comment                      | Who     
// +---------+----------------------------------------------+---------
// | 29/10/98| Initial version                              | MF
// +---------+----------------------------------------------+---------
//
//====================================================================
#ifndef DATASVC_UNITTEST_H
#define DATASVC_UNITTEST_H

#include "GaudiKernel/Kernel.h"
// C/C++ header files
#include <iostream>
#include <sys/time.h>

// Forward declarations
class DataObject;
class ISvcLocator;
class IMessageSvc;
class IAppMgrUI;

namespace Tests {


  /** Small class to perform timing measurements
   *
   *  @author M.Frank
   *  @date   11/2/99
   *
   */
  struct Timer {
    struct timeval m_start, m_stop;
    Timer() {}
    void start() {
      struct timezone tz;
      ::gettimeofday(&m_start,&tz);
    }
    void stop() {
      struct timezone tz;
      ::gettimeofday(&m_stop,&tz);
    }
    double seconds() const {
      long long int usec = 1000000*(m_stop.tv_sec-m_start.tv_sec)+(m_stop.tv_usec-m_start.tv_usec);
      return double(usec)/1e6;
    }
  };

  /** Small class to test the Gaudi services.
   *
   *  @author M.Frank
   *  @date   11/2/99
   *
   */
  class UnitTest  {
  public:
    long m_nerr;
    /// Debug flag
    bool m_debug;
    /// Unit test name
    std::string       m_name;
    /// Pointer to Service locator object
    ISvcLocator*      m_svcLoc;
    /// Pointer to message service instance
    IMessageSvc*      m_msgSvc;
    /// Keep reference to the apoplication managers user interface
    IAppMgrUI*        m_appMgr;

  public:
    /// Standard constructor
    UnitTest(const std::string& nam, bool dbg);
    /// Standard destructor
    virtual ~UnitTest();
    /// Action method
    virtual void run();
    /// Finalize the test...
    virtual void finalize();
    /// Shutdown the test...
    virtual void shutdown();
    /// Access message service
    IMessageSvc* msgSvc() const;
    /// Retrieve current time stamp in seconds
    static longlong currentTime();
    /// Access to the test's name
    const std::string& name()  const    {
      return m_name;
    }
    template <class T>
      StatusCode getService(const std::string& what, T*& ptr)   {
      return getService(what, T::interfaceID(), (void**)&ptr);
    }
    StatusCode getService(const std::string& what, 
			  const InterfaceID& iid, 
			  void** ptr);
    /// Check internally return code and trigger eventually output message
    bool check(StatusCode& status, long expected);
    std::string objectName(DataObject* pObject);
    std::string fullpath(DataObject* pObject);
  };
}

#endif // DATASVC_UNITTEST_H
