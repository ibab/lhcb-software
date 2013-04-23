#include "CPP/CheckpointRestoreWrapper.h"
#include <stdexcept>
#ifdef _WIN32
static CheckpointRestoreWrapper* _get() { return 0; }
#else
#include <dlfcn.h>
#include <unistd.h>
static CheckpointRestoreWrapper* _get() {
  typedef CheckpointRestoreWrapper* (*f_t)();
  union {
    void* p;
    f_t   f;
  } wrap;
  wrap.p = ::dlsym(0,"libProcessRestore_main_instance");
  return wrap.p ? (*wrap.f)() : 0;
}
#endif

CheckpointRestoreWrapper* CheckpointRestoreWrapper__instance() {
  static CheckpointRestoreWrapper* s_instance = _get();
  if ( s_instance ) return s_instance;
  throw std::runtime_error("No valid CheckpointRestore interface defined!");
}

void CheckpointRestoreWrapper__init_instance(int argc, char** argv) {
  CheckpointRestoreWrapper* s = CheckpointRestoreWrapper__instance();
  s->init_instance(argc, argv, environ);
}
