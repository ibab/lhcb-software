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
    /// Address of mapping
    void*          m_address[2];
    /// Lock status
    int            m_status;
    /// Lock identifier
    lib_rtl_lock_t m_id;
  public:
    GlobalSection(const std::string& nam);
    GlobalSection(const std::string& nam, int size);
    virtual ~GlobalSection();
    char* buffer()  const   {
      return (m_status&1) ? (char*)m_address[0] : 0;
    }
    int status()  const   {
      return m_status;
    }
    operator bool ()  const  {
      return lib_rtl_is_success(m_status);
    }
  };
}

#endif // _RTL_GLOBALSECTION_H
