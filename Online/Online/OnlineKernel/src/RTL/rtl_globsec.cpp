#define RTL_IMPLEMENTATION
#include <map>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "rtl_internal.h"
#include "fcntl.h"

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
#include "unistd.h"
#include <sys/mman.h>

#elif _WIN32
#include <windows.h>
#endif

namespace {
  struct GblLock  {
    GblLock() {}
    int status() const { return 1; }
    operator bool()  const { return true; }
  };
}


/// Create named global section
int lib_rtl_create_section(const char* sec_name, int size, lib_rtl_gbl_t* address) {
  std::auto_ptr<lib_rtl_gbl> h(new lib_rtl_gbl);
  sprintf(h->name,"/%s",sec_name);
  h->addaux = h.get();
  h->size = size;
#ifdef VMS
  int* add = (int*)address;
  int inadd[2]={0,0};
  DESCRIPTOR(name,sec_name);
  int size  = (size + 511)>>9;
  int flags   = SEC$M_GBL+SEC$M_EXPREG+SEC$M_WRT+SEC$M_PERM+SEC$M_SYSGBL+SEC$M_PAGFIL;
  str$upcase (&name, &name);
  int sc = sys$crmpsc (inadd, add, PSL$C_USER, flags, &name, 0,0,0,size,0,0,0); 
  if ( sc & 1 )  {
    return sc;
  }
#elif linux
  int sysprot  = PROT_READ+PROT_WRITE;
  int sysflags = MAP_SHARED;
  h->fd = ::shm_open(h->name,O_RDWR|O_CREAT,0644);
  if ( h->fd ) {
    ::ftruncate(h->fd, h->size);
  }
  h->address = ::mmap (0, h->size, sysprot, sysflags, h->fd, 0);
  if ( h->address != 0 )  {
    *address = h.release();
    return 1;
  }
  ::shm_unlink(h->name);
#elif _WIN32
  void** add = (void**)address;
  // Setup inherited security attributes (FIXME: merge somewhere else)
  SECURITY_ATTRIBUTES	sa = {sizeof(SECURITY_ATTRIBUTES), NULL, true};
  DWORD  siz  = (size/4096)*4096 + (((size%4096)==0) ? 0 : 4096);  //  multiple of page size
  h->addaux = ::CreateFileMapping(INVALID_HANDLE_VALUE,&sa,PAGE_READWRITE,0,siz,h->name);
  if (h->addaux != NULL && ::GetLastError() == ERROR_ALREADY_EXISTS)   { 
    ::CloseHandle(h->addaux); 
    h->addaux = ::OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,h->name);
  }
  if ( h->addaux )  {
    h->address = ::MapViewOfFile(h->addaux,FILE_MAP_ALL_ACCESS,0,0,0);
    if ( h->address != 0 )  {
      h->fd = 0;
      *address = h.release();
      return 1;
    }
  }
#endif
  int err = getError();
  ::printf("error mapping section [%s]. Status %d [%s]\n",h->name,err,errorString(err));
  return 0;
}

/// Delete named global section
int lib_rtl_delete_section(lib_rtl_gbl_t h)    {
  int sc = 0;
  if ( h )  {
#ifdef linux
    sc = ::shm_unlink(h->name)==0 ? 1 : 0;
    if ( 0 == sc ) delete h;
#else
    sc = 1;
#endif
  }
  return sc;
}

/// Map global section a a specific address
int lib_rtl_map_section(const char* sec_name, int size, lib_rtl_gbl_t* address)   {
  std::auto_ptr<lib_rtl_gbl> h(new lib_rtl_gbl);
  sprintf(h->name,"/%s",sec_name);
  h->addaux = h.get();
  h->size = size;
#ifdef VMS
  int* add = (int*)address;
  DESCRIPTOR(name,sec_name);
  int inadd[2]={0,0};
  int flags   = SEC$M_EXPREG+SEC$M_WRT+SEC$M_SYSGBL;
  str$upcase (&name, &name);
  int status = sys$mgblsc (inadd,add,PSL$C_USER,flags,&name,0,0);
  return status;
#elif linux
  int sysprot  = PROT_READ+PROT_WRITE;
  int sysflags = MAP_SHARED;
  h->fd = ::shm_open(n.c_str(),O_RDWR|O_CREAT,0644);
  if ( h->fd ) {
    ::ftruncate(h->fd, h->size);
  }
  h->address = ::mmap (0, h->size, sysprot, sysflags, h->fd, 0);
  if ( h->address != 0 )  {
    *address = h.release();
    return 1;
  }
  int err = getError();
  ::shm_unlink(h->name);
#elif _WIN32
  h->addaux = ::OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,h->name);
  if ( h->addaux )  {
    h->address = ::MapViewOfFile(h->addaux,FILE_MAP_ALL_ACCESS,0,0,0);
    if ( h->address != 0 )  {
      h->fd = 0;
      *address = h.release();
      return 1;
    }
  }
  int err = getError();
#endif
  ::printf("error mapping section [%s]. Status %d [%s]\n",h->name,err,errorString(err));
  return 0;
}

/// Unmap global section: address is quadword: void*[2]
int lib_rtl_unmap_section(lib_rtl_gbl_t h)   {
  if ( h )  {
#ifdef VMS
    int radd[2];
    int* inadd = (int*)&address;
    int status = sys$deltva (inadd, radd, PSL$C_USER);
    int sc = sys$purgws (inadd);
#elif linux
    int sc = ::munmap(h->address[0],h->size)==0 ? 1 : 0;
#elif _WIN32
    int sc = (::UnmapViewOfFile(h->address) == 0) ? 0 : 1;
#endif
    return sc;
  }
  return 0;
}

/// Flush global section to disk file
int lib_rtl_flush_section(lib_rtl_gbl_t h)   {
  if ( h )  {
#if _WIN32
    DWORD sc = ::FlushViewOfFile(h->addaux,h->size);
#elif linux
    ::msync(h->address, h->size, MS_INVALIDATE|MS_SYNC);
#endif
  }
  return 1;
}
