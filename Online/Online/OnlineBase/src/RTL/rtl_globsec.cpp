#define RTL_IMPLEMENTATION
#include <map>
#include <memory>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "rtl_internal.h"
#include "fcntl.h"

#if defined(__linux)
#include "unistd.h"
#include <sys/mman.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

using namespace RTL;
static std::auto_ptr<lib_rtl_gbl_map_t> s_map(new lib_rtl_gbl_map_t);
lib_rtl_gbl_map_t& RTL::allSections() {
  return *s_map.get();
}

extern "C" int lib_rtl_gbl_exithandler() {
  if ( s_map.get() )  {
    lib_rtl_gbl_map_t m = allSections();
    lib_rtl_gbl_map_t::iterator i = m.begin();
    for( ; i != m.end(); ++i ) {
      lib_rtl_delete_section((*i).second);
    }
    delete s_map.release();
  }
  return 1;
}

/// Create named global section
int lib_rtl_create_section(const char* sec_name, size_t size, lib_rtl_gbl_t* address, bool keep) {
  long siz  = (size/4096)*4096 + (((size%4096)==0) ? 0 : 4096);  //  multiple of page size
  std::auto_ptr<lib_rtl_gbl_desc> h(new lib_rtl_gbl_desc);
  ::snprintf(h->name,sizeof(h->name),"/%s",sec_name);
  h->addaux = h.get();
  h->size   = siz;
  *address  = 0;
  //::lib_rtl_output(LIB_RTL_DEBUG,"Create global section %s of size:%d\n",h->name, h->size);
#if defined(__linux)
  int sysprot  = PROT_READ+PROT_WRITE;
  int sysflags = MAP_SHARED;
  h->fd = ::shm_open(h->name,O_RDWR|O_CREAT|O_EXCL,0666);
  if ( h->fd != -1 ) {
    char path[1024];
    ::snprintf(path,sizeof(path),"/dev/shm%s",h->name);
    ::chmod(path,0666);
    ::ftruncate(h->fd, h->size);
    h->address = ::mmap (0, h->size, sysprot, sysflags, h->fd, 0);
    if ( h->address != MAP_FAILED && h->address != 0 )  {
      if ( !keep ) {
        allSections().insert(std::make_pair(h->name,h.get()));
      }
      *address = h.release();
      return 1;
    }
    ::close(h->fd);
    h->fd = 0;
  }
  // lib_rtl_output(LIB_RTL_DEBUG,"lib_rtl_create_section> Unlink section:%s\n",sec_name ? sec_name : "Unknown");
  // ::shm_unlink(h->name);
#elif defined(_WIN32)
  // Setup inherited security attributes (FIXME: merge somewhere else)
  SECURITY_ATTRIBUTES   sa = {sizeof(SECURITY_ATTRIBUTES), 0, true};
  h->addaux = ::CreateFileMapping(INVALID_HANDLE_VALUE,&sa,PAGE_READWRITE,0,siz,h->name);
  if (h->addaux != 0 && ::GetLastError() == ERROR_ALREADY_EXISTS)   { 
    ::CloseHandle(h->addaux); 
    h->addaux = ::OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,h->name);
  }
  if ( h->addaux )  {
    h->address = ::MapViewOfFile(h->addaux,FILE_MAP_ALL_ACCESS,0,0,siz);
    if ( h->address != 0 )  {
      h->fd = 0;
      *address = h.release();
      return 1;
    }
  }
#endif
  int err = lib_rtl_get_error();
  ::lib_rtl_output(LIB_RTL_ERROR,"Error creating section [%s]. Status %d [%s]\n",h->name,err,RTL::errorString(err));
  return 0;
}

/// Delete named global section
int lib_rtl_delete_section(lib_rtl_gbl_t h)    {
  std::auto_ptr<lib_rtl_gbl_desc> dsc((lib_rtl_gbl_desc*)h);
  int sc = 0;
  if ( dsc.get() )  {
#ifdef __linux
    //::lib_rtl_output(LIB_RTL_DEBUG,"lib_rtl_delete_section> Unlink section:%s\n",dsc->name ? dsc->name : "Unknown");
    sc = ::shm_unlink(dsc->name)==0 ? 1 : 0;
    lib_rtl_gbl_map_t& m = allSections();
    lib_rtl_gbl_map_t::iterator i=m.find(dsc->name);
    if ( i != m.end() ) {
      m.erase(i);
    }
    if ( dsc->fd ) ::close(dsc->fd);
    if ( sc == 0 ) sc = 1;
#else
    sc = lib_rtl_unmap_section(h);
#endif
  }
  return sc;
}

/// Map global section a a specific address
int lib_rtl_map_section(const char* sec_name, size_t size, lib_rtl_gbl_t* address)   {
  std::auto_ptr<lib_rtl_gbl_desc> h(new lib_rtl_gbl_desc);
  ::snprintf(h->name,sizeof(h->name),"/%s",sec_name);
  h->addaux = h.get();
  *address = 0;
  //::lib_rtl_output(LIB_RTL_DEBUG,"Map global section %s of size:%d\n",h->name, h->size);
#if defined(__linux)
  int sysprot  = PROT_READ+PROT_WRITE;
  int sysflags = MAP_SHARED;
  h->fd = ::shm_open(h->name,O_RDWR,0666);
  h->size = (int((size+4095)/4096))*4096;  //  multiple of page size
  if ( -1 == h->fd )  {
    ::shm_unlink(h->name);
    return 0;
  }
  if ( 0 == size ) {   // Map whole section
    struct stat st_buf;
    ::fstat(h->fd,&st_buf); // Must succeed, since file could be opened
    h->size = st_buf.st_size;
  }
  h->address = ::mmap (0, h->size, sysprot, sysflags, h->fd, 0);
  if ( h->address != MAP_FAILED && h->address != 0 )  {
    *address = h.release();
    return 1;
  }
  // File was already successfully opened
  ::close(h->fd);
  return 0;
#elif defined(_WIN32)
  h->size   = (int((size+4095)/4096))*4096;
  h->addaux = ::OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,h->name);
  if ( h->addaux )  {
    h->address = ::MapViewOfFile(h->addaux,FILE_MAP_ALL_ACCESS,0,0,size==0 ? 0 : h->size);
    if ( h->address != 0 )  {
      h->fd = 0;
      *address = h.release();
      return 1;
    }
  }
  int err = lib_rtl_get_error();
  ::lib_rtl_output(LIB_RTL_ERROR,"Error mapping section [%s]. Status %d [%s]\n",h->name,err,RTL::errorString(err));
  return 0;
#endif
}

/// Unmap global section: address is quadword: void*[2]
int lib_rtl_unmap_section(lib_rtl_gbl_t handle)   {
  if ( handle )  {
    std::auto_ptr<lib_rtl_gbl_desc> h((lib_rtl_gbl_desc*)handle);
    //lib_rtl_gbl_desc* h = (lib_rtl_gbl_desc*)handle;
#if defined(__linux)
    int sc = ::munmap(h->address,h->size)==0 ? 1 : 0;
    if ( h->fd >= 0 ) {
      //static int cnt = 0;
      /* int ret = */ ::close(h->fd);
      /* ::lib_rtl_output(LIB_RTL_DEBUG,"%d Close FD:%d err=%d\n",++cnt,h->fd,ret); */
    }
    h->fd = -1;
#elif defined(_WIN32)
    int sc = (::UnmapViewOfFile(h->address) == 0) ? 0 : 1;
    if ( 0 != sc ) ::CloseHandle(h->addaux);
#endif
    return sc;
  }
  return 0;
}

/// Flush global section to disk file
int lib_rtl_flush_section(lib_rtl_gbl_t handle)   {
  if ( handle )  {
    lib_rtl_gbl_desc* h = (lib_rtl_gbl_desc*)handle;
#if defined(_WIN32)
    DWORD sc = ::FlushViewOfFile(h->addaux,h->size);
    if ( sc == 0 )  {
      return 0;
    }
#elif defined(__linux)
    ::msync(h->address, h->size, MS_INVALIDATE|MS_SYNC);
#endif
  }
  return 1;
}
