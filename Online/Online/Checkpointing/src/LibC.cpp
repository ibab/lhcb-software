#include "Checkpointing/SysCalls.h"
#include "Checkpointing/MemMaps.h"
#include "Checkpointing/LibC.h"
#include "Checkpointing.h"
#include <cstdio>
#include <dlfcn.h>

// dlopen handle for whatever libc.so is loaded with application program
static void *libc_handle = 0;
static void *libpthread_handle = 0;

namespace CHECKPOINTING_NAMESPACE    {

  struct LibHandler : public AreaHandler  {
    char text[1024];
    const char* lib_name;
    static long s_handle(const AreaHandler* self,int,const Area& a) {
      LibHandler* h=(LibHandler*)self;
      if ( m_strfind(a.name,h->lib_name) ) {
	m_strcpy(h->text,a.name);
      }
      return 1;
    }
    LibHandler(const char* lib) { lib_name = lib; text[0]=0; f_handle = s_handle; }
  };

  /**  Set up 'clone_entry' variable
   *    Output:
   *	clone_entry = points to clone routine within libc.so
   */
  void* LibC::i_getSymbol(const char* name) {
    if ( libc_handle == 0 )   {
      LibHandler hdlr("/libc-");
      MemMaps mm;
      if ( 1 == mm.scan(hdlr) ) {
	const char* lib = hdlr.text;
	if ( lib[0] != 0 )   {
	  libc_handle = ::dlopen(lib,RTLD_LAZY|RTLD_GLOBAL);
	  if ( libc_handle == 0 )   {
	    mtcp_output(MTCP_FATAL,"LibC::getSymbol: error opening %s: %s\n",lib,dlerror());
	  }
	}
      }
      if ( 0 == libc_handle ) {
	mtcp_output(MTCP_FATAL,"LibC::getSymbol: failed to locate libc\n");
      }
    }
    void *temp = ::dlsym(libc_handle,name);
    if (temp == 0) {
      mtcp_output(MTCP_FATAL,"LibC::getSymbol: error getting %s from libc.\n",dlerror());
    }
    return temp;
  }

  void* LibPThread::i_getSymbol(const char* name) {
    if ( libpthread_handle == 0 )   {
      LibHandler hdlr("/libpthread");
      MemMaps mm;
      if ( 1 == mm.scan(hdlr) ) {
	const char* lib = hdlr.text;
	if ( lib[0] != 0 )   {
	  libpthread_handle = ::dlopen(lib,RTLD_LAZY|RTLD_GLOBAL);
	  if ( libpthread_handle == 0 )   {
	    mtcp_output(MTCP_FATAL,"Pthread::getSymbol: error opening %s: %s\n",lib,dlerror());
	  }
	}
      }
      if ( 0 == libpthread_handle ) {
	mtcp_output(MTCP_FATAL,"Pthread::getSymbol: failed to locate pthread\n");
      }
    }
    void *temp = ::dlsym(libpthread_handle,name);
    if (temp == 0) {
      mtcp_output(MTCP_FATAL,"Pthread::getSymbol: error getting %s from pthread.\n",dlerror());
    }
    return temp;
  }

  void* LocalProcess::i_getSymbol(const char* name) {
    void *temp = ::dlsym(0,name);
    if (temp == 0) {
      //mtcp_output(MTCP_FATAL,"Pthread::getSymbol: error getting %s from pthread.\n",dlerror());
    }
    return temp;
  }
}
