#ifndef _ONLINEKERNEL_RTL_INTERCEPTHANDLER_H
#define _ONLINEKERNEL_RTL_INTERCEPTHANDLER_H

#include "RTL/rtl.h"

/*
 *   RTL namespace declaration
 */
namespace RTL  {

  /**@class InterceptHandler InterceptHandler.h RTL/InterceptHandler.h
   */
  template<bool mode=true> class InterceptHandler  {
  public:
    /// Initializing constructor for inverted lock
    InterceptHandler() {
      mode ? lib_rtl_disable_intercept() : lib_rtl_enable_intercept();
    }
    /// Standard destructor
    ~InterceptHandler()  {
      mode ? lib_rtl_disable_intercept() : lib_rtl_enable_intercept();
    }
  };
  typedef InterceptHandler<true>  InterceptDisabler;
  typedef InterceptHandler<false> InterceptEnabler;
}

#endif // _ONLINEKERNEL_RTL_INTERCEPTHANDLER_H
