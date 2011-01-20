#include "CheckPointing/SysCalls.h"
#include "CheckPointing/MemMaps.h"
#include "CheckPointing/LibC.h"
#include "CheckPointing.h"
#include <cstdio>
#include <dlfcn.h>

// dlopen handle for whatever libc.so is loaded with application program
static void *libc_handle = 0;

namespace CHECKPOINTING_NAMESPACE    {

  /**  Set up 'clone_entry' variable
   *    Output:
   *	clone_entry = points to clone routine within libc.so
   */
  void* LibC::i_getSymbol(const char* name) {
    if ( libc_handle == 0 )   {
      struct LibCHandler : public AreaHandler  {
	char text[1024];
	static int s_handle(const AreaHandler* self,int,const Area& a) {
	  if ( m_strfind(a.name,"/libc-") ) {
	    LibCHandler* h=(LibCHandler*)self;
	    m_strcpy(h->text,a.name);
	  }
	  return 1;
	}
	LibCHandler() { text[0]=0; f_handle = s_handle; }
      } hdlr;
      MemMaps mm;
      if ( 1 == mm.scan(hdlr) ) {
	//const char* lib = "/lib64/libc.so.6";
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
}
