#ifndef _RTL_GLOBALSECTION_H
#define _RTL_GLOBALSECTION_H

#include "RTL/rtl.h"
#include <string>

/*
 *   RTL namespace declaration
 */
namespace RTL  {

  /**@class GlobalSection GlobalSection.h RTL/GlobalSection.h
    */
  class GlobalSection  {
    /// Section name
    std::string    m_name;
    /// Lock status
    int            m_status;
    /// Address of mapping
    lib_rtl_gbl_t  m_address;
    /// Lock identifier
    lib_rtl_lock_t m_id;

  public:
    /// Initializing constructor
    GlobalSection(const std::string& nam, 
		  int size, 
		  bool create=false, 
		  bool enable_locking=false);

    /// Standard destructor
    virtual ~GlobalSection();

    /// Access to the mapped memory buffer
    char* buffer()  const   {
      return (m_status&1) ? (char*)m_address->address : 0;
    }
    /// Access to the mapping status after creation
    int status()  const   {
      return m_status;
    }
    /// Operator to check status
    operator bool ()  const  {
      return lib_rtl_is_success(m_status);
    }
    /// Lock the global section
    int lock();
    /// Unlock the global section
    int unlock();
  };
}

#endif // _RTL_GLOBALSECTION_H
