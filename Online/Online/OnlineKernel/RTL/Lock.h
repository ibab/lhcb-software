#ifndef _RTL_LOCK_H
#define _RTL_LOCK_H

#include "RTL/rtl.h"

/*
 *   RTL namespace declaration
 */
namespace RTL  {

  /**@class Lock Lock.h RTL/Lock.h
    */
  class Lock  {
    /// Lock status
    int            m_status;
    /// Lock identifier
    lib_rtl_lock_t m_id;
  public:
    /// Initializing constructor
    Lock(lib_rtl_lock_t id);
    /// Standard destructor
    virtual ~Lock();
    /// Create now named lock
    static int create(const char* name, lib_rtl_lock_t& id);
    /// Validity check
    operator int ()   {
      return lib_rtl_is_success(m_status);
    }
    /// Access to object status
    int status()      const   {
      return m_status;
    }
  };
}

#endif // _RTL_LOCK_H
