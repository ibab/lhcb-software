#ifndef _RTL_THREAD_H
#define _RTL_THREAD_H

#include "RTL/rtl.h"

/*
 *   RTL namespace declaration
 */
namespace RTL  {

  /**@class Thread Thread.h RTL/Thread.h
   */
  class Thread  {
    /// Thread status
    int            m_status;
    /// Thread identifier
    //lib_rtl_thread_t m_id;
  public:
    /// Initializing constructor
    Thread();
    /// Standard destructor
    virtual ~Thread();
    /// Create now named Thread
    static int create(const char* name, lib_rtl_thread_t& id);
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

#endif // _RTL_THREAD_H
