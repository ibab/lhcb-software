#include "ActiveMQ/Log.h"
#include "RTL/rtl.h"

static size_t amq_rtl_print(void* param,int lvl,const char* fmt,va_list args) {
  if ( lvl >= long(param) ) {
    size_t result;
    std::string format = ::lib_rtl_timestr();
    format += " ";
    format += fmt;
    format += "\n";
    result = ::vfprintf(stdout, format.c_str(), args);
    ::fflush(stdout);
    return result;
  }
  return 1;
}

std::ostream& ActiveMQ::log() {
  return std::cout << ::lib_rtl_timestr() << " ";
}

void ActiveMQ::install_printer(int arg) {
  ::lib_rtl_install_printer(amq_rtl_print,(void*)arg);
}

