#ifndef _RTL_SYSTIME_H
#define _RTL_SYSTIME_H

#include "RTL/rtl.h"
#include "RTL/que.h"

/*
 *   RTL namespace declaration
 */
namespace RTL  {

  /** @class SysTime
   */
  class SysTime {
  private:
    SysTime() {}
  public:
    static unsigned int start();
    static unsigned int now();
    static unsigned int uptime() {
      return now() - start(); 
    }
  };
}
#endif // _RTL_SYSTIME_H
