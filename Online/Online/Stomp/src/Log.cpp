#include "Stomp/Log.h"
#include "RTL/rtl.h"

std::ostream& Stomp::log() {
  return std::cout << ::lib_rtl_timestr() << " ";
}
