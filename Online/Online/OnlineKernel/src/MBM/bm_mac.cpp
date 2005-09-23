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

int _mbm_create_section(const char* sec_name,int size, void* address) {
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

int _mbm_delete_section(const char *sec_name)
{
  return 1;
}

int _mbm_map_section(const char* sec_name, void* address)   {
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
  add[0] = fd;
  add[1] = (int)mmap (0, length, sysprot, sysflags, fd, pos);
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
int _mbm_unmap_section(void* address)   {
  int* inadd = (int*)address;
#ifdef VMS
  int radd[2];
  int status = sys$deltva (inadd, radd, PSL$C_USER);
  return sys$purgws (inadd);
#elif _WIN32
  return (::UnmapViewOfFile((void*)inadd[0]) == 0) ? 0 : 1;
#endif
}

int _mbm_flush_sections(BMDESCRIPT* bm)   {
#ifdef VMS
#elif _WIN32
  DWORD sc;
  sc=::FlushViewOfFile(bm->ctrl_add[0],0);
  sc=::FlushViewOfFile(bm->event_add[0],0);
  sc=::FlushViewOfFile(bm->user_add[0],0);
  sc=::FlushViewOfFile(bm->bitm_add[0],0);
  sc=::FlushViewOfFile(bm->buff_add[0],0);
  return 1;
#endif
}

