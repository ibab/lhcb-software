#ifndef ROMON_ROMONDEF_H
#define ROMON_ROMONDEF_H
#include <iostream>
namespace ROMon {
  std::ostream& log();
  void print_startup(const char* msg);
  void ro_gettime(int* seconds, unsigned int* milli_seconds);
}
#endif // ROMON_ROMONDEF_H
