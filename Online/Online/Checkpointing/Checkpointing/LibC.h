#ifndef CHECKPOINTING_LIBC_H
#define CHECKPOINTING_LIBC_H
#include "Checkpointing/Namespace.h"

namespace CHECKPOINTING_NAMESPACE    {

  struct LibC {
    private:
    static void* i_getSymbol(const char* name);
    public:
    template<class T> static int getSymbol(const char* name, T& func_ptr) {
      func_ptr = (T)i_getSymbol(name);
      return func_ptr == 0 ? 0 : 1;
    }
  };
}

#endif // CHECKPOINTING_LIBC_H
