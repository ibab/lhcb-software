#define MBM_IMPLEMENTATION

#include <cstdio>
#include <cstdlib>
#include "bm_struct.h"

#ifdef VMS
#define lock_prio  14
typedef struct  {
  short len,code;
  int addr,retaddr;
}  TABNAM_LIST;
static int def_prio;
#include <secdef.h>
#include <psldef.h>

#elif linux

#include <sys/mman.h>

#elif _WIN32
#include <windows.h>
#endif

#ifdef _WIN32
static char mutexName[32] = "";
#endif

