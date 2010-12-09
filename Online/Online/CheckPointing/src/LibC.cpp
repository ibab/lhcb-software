#include "CheckPointing/SysCalls.h"
#include "CheckPointing/LibC.h"
#include <cstdio>
#include <dlfcn.h>

// dlopen handle for whatever libc.so is loaded with application program
static void *handle = 0;

/**  Set up 'clone_entry' variable
 *    Output:
 *	clone_entry = points to clone routine within libc.so
 */
void* LibC::i_getSymbol(const char* name) {
  if ( handle == 0 )   {
    const char* lib = "/lib64/libc.so.6";
    handle = ::dlopen(lib,RTLD_LAZY|RTLD_GLOBAL);
    if ( handle == 0 )   {
      mtcp_printf("LibC::getSymbol: error opening %s: %s\n",lib,dlerror());
      mtcp_abort();
    }
  }
  void *temp = ::dlsym(handle,name);
  if (temp == 0) {
    mtcp_printf("LibC::getSymbol: error getting %s from libc.\n",dlerror());
    mtcp_abort();
  }
  return temp;
}
