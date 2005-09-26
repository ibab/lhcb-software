#define RTL_IMPLEMENTATION

#include <cstdio>
#include <cstdlib>
#include "RTL/rtl.h"

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

/// Create named global section
int lib_rtl_create_section(const char* sec_name,int size, void* address) {
  int* add = (int*)address;
#ifdef VMS
  int inadd[2]={0,0};
  DESCRIPTOR(name,sec_name);
  int size  = (size + 511)>>9;
  int flags   = SEC$M_GBL+SEC$M_EXPREG+SEC$M_WRT+SEC$M_PERM+SEC$M_SYSGBL+SEC$M_PAGFIL;
  str$upcase (&name, &name);
  return sys$crmpsc (inadd, add, PSL$C_USER, flags, &name, 0,0,0,size,0,0,0); 
#elif linux

  return 1;
#elif _WIN32
  // Setup inherited security attributes (FIXME: merge somewhere else)
  SECURITY_ATTRIBUTES	sa = {sizeof(SECURITY_ATTRIBUTES), NULL, true};
  DWORD  siz  = (size/4096)*4096 + (((size%4096)==0) ? 0 : 4096);  //  multiple of page size
  HANDLE hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE,&sa,PAGE_READWRITE,0,siz,sec_name);
  if (hMap != NULL && ::GetLastError() == ERROR_ALREADY_EXISTS)   { 
    ::CloseHandle(hMap); 
    hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,sec_name);
  }
  if ( hMap )  {
    add[0] = (int)::MapViewOfFile(hMap,FILE_MAP_ALL_ACCESS,0,0,0);
    if ( add[0] != 0 )  {
      add[1] = (int)hMap;
      return 1;
    }
  }
  return 0;
#endif
}

/// Delete named global section
int lib_rtl_delete_section(const char *sec_name)
{
  return 1;
}

/// Map global section a a specific address
int lib_rtl_map_section(const char* sec_name, void* address)   {
  int* add = (int*)address;
#ifdef VMS
  DESCRIPTOR(name,sec_name);
  int inadd[2]={0,0};
  int flags   = SEC$M_EXPREG+SEC$M_WRT+SEC$M_SYSGBL;
  str$upcase (&name, &name);
  int status = sys$mgblsc (inadd,add,PSL$C_USER,flags,&name,0,0);
  return status;
#elif linux
  int fd = 0;
  int sysprot  = PROT_READ+PROT_WRITE;
  int sysflags = MAP_SHARED+MAP_ANONYMOUS;
# ifdef MAP_ANONYMOUS
  sysflags |= MAP_ANONYMOUS;
# else
  fd = ::open("/dev/zero",O_RDWR|O_BINARY);
  // Must have a real file or /dev/zero opened in initialise().
  ASSERT (fd != IOFD_INVALID);
# endif
  int length = 1024, pos=0;
  add[1] = fd;
  add[0] = (int)mmap (0, length, sysprot, sysflags, fd, pos);
#elif _WIN32
  HANDLE hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,sec_name);
  if ( hMap )  {
    add[0] = (int)::MapViewOfFile(hMap,FILE_MAP_ALL_ACCESS,0,0,0);
    if ( add[0] != 0 )  {
      add[1] = (int)hMap;
      return 1;
    }
  }
  return 0;
#endif
}

/// Unmap global section: address is quadword: void*[2]
int lib_rtl_unmap_section(void* address)   {
  int* inadd = (int*)address;
#ifdef VMS
  int radd[2];
  int status = sys$deltva (inadd, radd, PSL$C_USER);
  return sys$purgws (inadd);
#elif _WIN32
  return (::UnmapViewOfFile((void*)inadd[0]) == 0) ? 0 : 1;
#endif
}

/// Flush global section to disk file
int lib_rtl_flush_section(void* address)   {
#if _WIN32
  int* inadd = (int*)address;
  DWORD sc = ::FlushViewOfFile((void*)inadd[0],0);
#endif
  return 1;
}

