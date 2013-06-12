#ifndef ROMON_ROMONDEF_H
#define ROMON_ROMONDEF_H
#include <iostream>
#include <string>
#include <cstdarg>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {
  std::ostream& log();
  void print_startup(const char* msg);
  void ro_gettime(int* seconds, unsigned int* milli_seconds);
  std::string strupper(const std::string& n);
  std::string strlower(const std::string& n);
  size_t ro_rtl_print(void*,int,const char* fmt,va_list args);
  /// Extract node/service name from DNS info
  void getServiceNode(char* s, std::string& svc, std::string& node);
  class DimLock {
  public:
    DimLock();
    ~DimLock();
  };
  class DimReverseLock {
  public:
    DimReverseLock();
    ~DimReverseLock();
  };
  template <typename A, typename B> inline A ro_min(const A& a, const B& b) {
    return a < b ? a : b;
  }
}
#endif // ROMON_ROMONDEF_H
