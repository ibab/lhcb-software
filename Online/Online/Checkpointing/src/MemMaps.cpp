#include "Checkpointing/MemMaps.h"
#include "Checkpointing/SysCalls.h"
#include "Checkpointing/Static.h"
#include "Checkpointing.h"
#include "Restore.h"

#include <cstdio>
#include <cerrno>
#include <cstring>
#include <climits>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

using namespace Checkpointing;

#ifndef __STATIC__
DefineMarker(MEMMAP_BEGIN_MARKER,"MMAP");
DefineMarker(MEMMAP_END_MARKER,  "mmap");
#endif

template <class T> int default_map(const AreaHandler* ,const Area& a, const unsigned char* data, int data_len) {
  return T::do_map(a, data, data_len);
}

template <class T> int default_handle(const AreaHandler* p,int which, const Area& a) {
  return ((T*)p)->handle(which,a);
}


AreaBaseHandler::AreaBaseHandler() : m_bytes(0), m_count(0)   {
  // Memarea size without individual areas
  m_space = 2*sizeof(Marker)+sizeof(int);
  f_map     = default_map<AreaBaseHandler>;
  f_handle  = default_handle<AreaBaseHandler>;
}

int AreaBaseHandler::updateCounts(const Area& a)   {
  AreaBaseHandler* p = const_cast<AreaBaseHandler*>(this);
  long pg = chkpt_sys.pageSize;
  long sz = pg - 1;
  p->m_bytes += size_t((a.size+sz)/pg)*pg;
  p->m_space += a.length();
  ++p->m_count;
  return 1;
}

int AreaBaseHandler::mapArea(const Area& a, const unsigned char* in, int data_len)  {
  return checkpointing_area_map(a,0,in,data_len);
}

int AreaBaseHandler::handle(int, const Area& a)    {
  return updateCounts(a);
}

AreaPrintHandler::AreaPrintHandler() : AreaBaseHandler()  {
  f_handle = default_handle<AreaPrintHandler>;
}

int AreaPrintHandler::handle(int, const Area& a)    {
  a.print();
  return updateCounts(a);
}

AreaInfoHandler::AreaInfoHandler() {
  stack[0]      = stack[1]      = 0;
  vdso[0]       = vdso[1]       = 0;
  vsyscall[0]   = vsyscall[1]   = 0;
  imageAddr[0]  = ULONG_MAX;
  imageAddr[1]  = 0;
  checkpointAddr[0] = ULONG_MAX;
  checkpointAddr[1] = 0;
  highAddr          = 0;
  image[0]          = 0;
  m_prev            = false;
  f_handle = default_handle<AreaInfoHandler>;
}

int AreaInfoHandler::handle(int, const Area& a)  {
  //if ( m_prev && a.name[0] != 0 ) m_prev = false;
  if ( a.name[0]=='[' ) {
    if ( *(int*)a.name == *(int*)"[stack]" ) {
      stack[0] = a.low;
      stack[1] = a.high;
    }
    else if ( *(int*)a.name == *(int*)"[vdso]" ) {
      vdso[0] = a.low;
      vdso[1] = a.high;
    }
    else if ( *(int*)a.name == *(int*)"[vsyscall]" ) {
      vsyscall[0] = a.low;
      vsyscall[1] = a.high;
    }
  }
  else if ( a.name[0] == '/' ) {
    if ( strstr(a.name,chkpt_sys.checkpointFile) ) {
      if ( checkpointAddr[0] > a.low )  checkpointAddr[0] = a.low;
      if ( checkpointAddr[1] < a.high ) checkpointAddr[1] = a.high;
    }
    else if ( strstr(a.name,"libCheckpointing.so") ) {
      m_prev = true;
      if ( !image[0] ) m_memcpy(image,a.name,sizeof(image));
      if ( imageAddr[0] > a.low )  imageAddr[0] = a.low;
      if ( imageAddr[1] < a.high ) imageAddr[1] = a.high;
    }
    else if ( strstr(a.name,"libChkpt.so") ) {
      m_prev = true;
      if ( !image[0] ) m_memcpy(image,a.name,sizeof(image));
      if ( imageAddr[0] > a.low )  imageAddr[0] = a.low;
      if ( imageAddr[1] < a.high ) imageAddr[1] = a.high;
    }
  }
  else if ( m_prev ) {
    m_prev = false;
    if ( imageAddr[0] > a.low )  imageAddr[0] = a.low;
    if ( imageAddr[1] < a.high ) imageAddr[1] = a.high;
  }
  if ( a.high > highAddr ) highAddr = a.high;
  return updateCounts(a);
}

AreaWriteHandler::AreaWriteHandler(int fd) 
  : m_fd(fd), m_bytes(0) 
{
  f_handle = default_handle<AreaWriteHandler>;
}

int AreaWriteHandler::handle(int, const Area& a)    {
  // Skip if the memory region is where we actually write!
  if ( a.high == chkpt_sys.addrEnd ) {
    checkpointing_area_print(&a,MTCP_INFO,"*** WARNING: SKIP IMAGE area:");
    return 0;
  }  
  else if ( a.low >= chkpt_sys.addrStart && a.high <= chkpt_sys.addrEnd ) {
    checkpointing_area_print(&a,MTCP_INFO,"*** WARNING: SKIP IMAGE area:");
    return 0;
  }  
  else if( *(int*)a.name == *(int*)"[vdso]" && chkpt_sys.vsyscallStart == 0 ) {
    checkpointing_area_print(&a,MTCP_INFO,"*** WARNING: SKIP [vdso] area:");
    return 0;
  }
  else if( *(long*)a.name == *(long*)"[vsyscall]" ) {
    checkpointing_area_print(&a,MTCP_INFO,"*** WARNING: SKIP [vsyscall] area:");
    return 0;
  }
  else if ( m_strcmp(a.name,"/usr/lib/locale/locale-archive") == 0 ) {
    checkpointing_area_print(&a,MTCP_INFO,"*** WARNING: LOCALE ARCHIVE:");
    return 0;
  }
  else if ( m_strcmp(a.name,"/usr/lib64/gconv/gconv-modules.cache") == 0 ) {
    checkpointing_area_print(&a,MTCP_INFO,"*** WARNING: LOCALE ARCHIVE:");
    return 0;
  }

  long rc = a.write(m_fd);
  if ( rc > 0 )  {
    updateCounts(a);
    m_bytes += rc;
  }
  return rc;
}

AreaChkptWriteHandler::AreaChkptWriteHandler(int fd) : AreaWriteHandler(fd) {
  f_handle = default_handle<AreaChkptWriteHandler>;
  m_prev = false;
}

int AreaChkptWriteHandler::handle(int, const Area& a)    {
  if ( m_prev || m_strcmp(a.name,chkpt_sys.checkpointImage) == 0 ) {
    long rc = 0;
    if ( a.prot[0]=='r' ) {
      rc = m_writemem(m_fd,(void*)a.low,a.size);
    }
    else {
      rc = m_writeset(m_fd,0,a.size);
    }
    if ( rc > 0 ) m_bytes += rc;
    a.print("Write raw image:");
    m_prev = m_prev ? false : true;
    return rc;
  }
  m_prev = false;
  return 0;
}

AreaMapper::AreaMapper()   {
  f_map    = default_map<AreaMapper>;
  f_handle = default_handle<AreaMapper>;
}

/// Number of memory mapped areas/files
int MemMaps::numLines()  const {
  int fd = mtcp_sys_open("/proc/self/maps",O_RDONLY,0);
  if ( fd>0 ) {
    char p;
    int num = 0;
    for(int n=1; n>0;)   {
      n = mtcp_sys_read(fd,&p,1);
      if      ( n == 1 && p!='\n' ) continue;
      else if ( n == 1 ) ++num;
      else if ( n == 0 ) break;
    }
    mtcp_sys_close(fd);
    return num;
  }
  return -1;
}

/// Collect in process information about the memory mappings
int MemMaps::scan(AreaHandler& handler) {
  int fd = mtcp_sys_open("/proc/self/maps",O_RDONLY,0);
  if ( fd>0 ) {
    int count = 0;
    char text[256+PATH_MAX] = "  ", *p=text, *low, *high, *prot, *inode, *dev, *off, *fn;
    const char *file;
    for(int n=1; n>0 && (p-text)<int(sizeof(text)); ++p)   {
      n = mtcp_sys_read(fd,p,1);
      if ( n == 1 && *p != '\n' ) continue;
      *p = 0;
      if ( n == 1 ) {
	low  = text;
	high = m_chrfind(low,'-');
	*high++  = 0;
	prot     = m_chrfind(high,' ');
	*prot++  = 0;
	off      = m_chrfind(prot,' ');
	*off++   = 0;
	dev      = m_chrfind(off,' ');
	*dev++   = 0;
	inode    = m_chrfind(dev,' ');
	*inode++ = 0;
	fn       = m_chrfind(inode,' ');
	*fn++    = 0;
	file     = m_chrfind(fn,'/');
	if ( !file ) file = m_chrfind(fn,'[');
	if ( !file ) file = "";
	Area a;
	::sscanf(low,"%lx",&a.low);
	::sscanf(high,"%lx",&a.high);
	::sscanf(off,"%lx",&a.offset);
	a.size = a.high - a.low;
	*(int*)a.prot = *(int*)prot;
	if ( !file[0] ) {
	  unsigned long brk = (unsigned long)mtcp_sys_brk(0);
	  if ( brk >= a.low && brk <= a.high ) file = "[heap]";
	}
	a.name_len = m_strcpy(a.name,file);
	long sc = (*handler.f_handle)(&handler,count,a);
	if ( sc > 0 ) count++;
      }
      if ( n == 0 ) break;
      p = text;
      *p = ' ';
    }
    text[sizeof(text)-1]=0;
    close(fd);
    return 1;
  }
  return 0;
}

/// Dump descriptor information about the memory mappings
void MemMaps::dump() {
  AreaPrintHandler h;
  scan(h);
  mtcp_output(MTCP_INFO,"--------------- Total memory size:%ld Bytes\n",h.bytes());
}

/// Write descriptor information and data from the memory mappings to file
long MemMaps::write(int fd) {
  if ( fd > 0 ) {
    int bytes   = writeMarker(fd,MEMMAP_BEGIN_MARKER);
    long offset = ::lseek(fd,0,SEEK_CUR);
    bytes += writeInt(fd,0);
    AreaWriteHandler h(fd);
    if ( 1 == scan(h) ) {
      bytes += h.bytesWritten();
      bytes += writeMarker(fd,MEMMAP_END_MARKER);
      long off = ::lseek(fd,0,SEEK_CUR);
      ::lseek(fd,offset,SEEK_SET);
      writeInt(fd,h.count()); // Update counter
      ::lseek(fd,off,SEEK_SET);
      return bytes;
    }
    mtcp_output(MTCP_ERROR,"Failed to scan memory sections!\n");
    return -1;
  }
  return -1;
}

/// Read descriptor information and data from the memory mappings from file
long MemMaps::read(const void* address, AreaHandler& handler) {
  const_Pointer in = (const_Pointer)address;
  if ( in ) {
    int numArea = 0;
    in += checkMarker(in,MEMMAP_BEGIN_MARKER);
    in += getInt(in,&numArea);
    Area a;
    for(int i=0; i<numArea; ++i) {
      in += a.read(in,handler);
      handler.f_handle(&handler,i,a);
    }
    in += checkMarker(in,MEMMAP_END_MARKER);
    mtcp_output(MTCP_INFO,"Read %ld bytes of mmap area data\n",addr_diff(in,address));
    return addr_diff(in,address);
  }
  return -1;
}
