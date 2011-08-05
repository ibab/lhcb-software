#ifndef FRAMEWORK_KERNEL_H
#define FRAMEWORK_KERNEL_H
#include <cstdarg>
namespace {
  inline int error(const char* /* fmt */, ...) {
    return 0;
  }
}
#define MSG_SRC_FMT "%-12s [0x%p] "
#endif  // FRAMEWORK_KERNEL_H
