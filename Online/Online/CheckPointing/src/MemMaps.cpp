#include "CheckPointing/MemMaps.h"
#include "CheckPointing/SysCalls.h"
#include "CheckPointing/Static.h"
#include "CheckPointing/MMap.h"
#include "CheckPointing.h"

#include <cstdio>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

using namespace CheckPointing;

static const int MEMMAP_BEGIN_MARKER = *(Marker*)"MMAP";
static const int MEMMAP_END_MARKER   = *(Marker*)"mmap";

int AreaMapper::handle(int , const Area& )   {
  return 1;
}

AreaBaseHandler::AreaBaseHandler() : m_bytes(0), m_count(0)
{
  // Memarea size without individual areas
  m_space = 2*sizeof(Marker)+sizeof(int);
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

int AreaBaseHandler::handle(int, const Area& a)    {
  return updateCounts(a);
}

int AreaPrintHandler::handle(int, const Area& a)    {
  a.print();
  return updateCounts(a);
}

AreaInfoHandler::AreaInfoHandler() {
  stack[0]    = stack[1]    = 0;
  vdso[0]     = vdso[1]     = 0;
  vsyscall[0] = vsyscall[1] = 0;
  highAddr    = 0;
}

int AreaInfoHandler::handle(int, const Area& a)  {
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
  if ( a.high > highAddr ) highAddr = a.high;
  return updateCounts(a);
}

int AreaWriteHandler::handle(int, const Area& a)    {
  // Skip if the memory region is where we actually write!
  if ( m_addr >= (void*)a.low && m_addr <= (void*)a.high ) {
    a.print("SKIP  OUTPUT area:");
    return 0; // This is the output segment
  }
  else   {
    updateCounts(a);
    long rc = a.write(m_ptr);
    if ( rc > 0 ) m_ptr += rc;
    return addr_diff(m_ptr,m_addr);
  }
  return 0;
}

/// Default constructor
MemMaps::MemMaps() : m_numArea(0), m_areas(0) {
}

/// Default destructor
MemMaps::~MemMaps()   {
  if ( m_areas ) delete [] m_areas;
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
	  unsigned long brk = (unsigned long)mtcp_sys_brk(NULL);
	  if ( brk >= a.low && brk <= a.high ) file = "[heap]";
	}
	a.name_len = m_strcpy(a.name,file);
	long sc = handler.handle(count,a);
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
long MemMaps::write(void* address) {
  Pointer num, out = (Pointer)address;
  if ( out ) {
    out += saveMarker(out,MEMMAP_BEGIN_MARKER);
    num = out;
    out += saveInt(out,0);
    AreaWriteHandler h(out);
    if ( 1 == scan(h) ) {
      out += h.bytesWritten();
      out += saveMarker(out,MEMMAP_END_MARKER);
      saveInt(num,h.count()); // Update counter
      mtcp_output(MTCP_INFO,"Write %ld bytes of mmap area data\n",addr_diff(out,address));
      long len = addr_diff(out,address);
      return len;
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
    in += checkMarker(in,MEMMAP_BEGIN_MARKER);
    in += getInt(in,&m_numArea);
    Area a;
    for(int i=0; i<m_numArea; ++i) {
      in += a.read(in);
      handler.handle(i,a);
    }
    in += checkMarker(in,MEMMAP_END_MARKER);
    mtcp_output(MTCP_INFO,"Read %ld bytes of mmap area data\n",addr_diff(in,address));
    return addr_diff(in,address);
  }
  return -1;
}


/// Write descriptor information and data from the memory mappings to file
long MemMaps::write(const char* file_name) {
  AreaBaseHandler b;
  if ( scan(b) > 0 ) {
    long len = b.space();
    MMap m;
    if ( m.create(file_name,len) ) {
      void* mem = m.address();
      mtcp_output(MTCP_INFO,"Got memory area:%p -> %p [%d bytes]\n",m.low(),m.high(),len);
      long true_len = this->write(mem);
      mtcp_output(MTCP_INFO,"Wrote %ld bytes of %ld space.\n",true_len,len);
      m.commit(true_len);
      return true_len;
    }
    mtcp_output(MTCP_ERROR,"Failed to open file %s: %s\n",file_name,strerror(errno));
    return 0;
  }
  mtcp_output(MTCP_ERROR,"Failed to estimate mmap size for %s: %s\n",file_name,strerror(errno));
  return 0;
}

/// Read descriptor information and data from the memory mappings from file
long MemMaps::read(const char* file_name) {
  MMap m;
  if ( m.open(file_name) ) {
    void* mem = m.address();
    AreaMapper mapper;
    if ( this->read(mem,mapper) ) {
      m.close();
      return 1;
    }
    mtcp_output(MTCP_ERROR,"Failed to read memory maps from file %s.\n",file_name);
    return 0;
  }
  mtcp_output(MTCP_ERROR,"Failed to access file %s: %s\n",file_name,strerror(errno));
  return 0;
}
