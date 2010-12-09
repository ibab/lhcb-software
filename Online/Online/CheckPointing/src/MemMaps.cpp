#include "CheckPointing/MemMaps.h"
#include "CheckPointing/SysCalls.h"
#include "CheckPointing.h"

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

using namespace CheckPointing;

#define MEMMAP_BEGIN  "<MemMap>"
#define MEMMAP_END    "</MemMap>"

void AreaCollector::handle(int which, const Area& a) const {
  if ( 0 == m_maps->m_areas ) m_maps->m_areas = new Area[m_maps->m_numArea];
  m_maps->m_areas[which] = a;
}

void AreaMapper::handle(int which, const Area& a)  const {
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
int MemMaps::collect() {
  if ( m_areas ) delete [] m_areas;
  m_areas = new Area[m_numArea=numLines()];
  int fd = mtcp_sys_open("/proc/self/maps",O_RDONLY,0);
  if ( fd>0 ) {
    int count = 0;
    char text[256+PATH_MAX] = "  ", *p=text, *low, *high, *prot, *inode, *dev, *off, *fn;
    const char* file;
    for(int n=1; n>0 && (p-text)<int(sizeof(text)); ++p)   {
      n = mtcp_sys_read(fd,p,1);
      if ( n == 1 && *p != '\n' ) continue;
      *p = 0;
      if ( n == 1 ) {
	//mtcp_printf("%s\n",text);
	low  = text;
	high = strchr(low,'-');
	*high++  = 0;
	prot     = strchr(high,' ');
	*prot++  = 0;
	off      = strchr(prot,' ');
	*off++   = 0;
	dev      = strchr(off,' ');
	*dev++   = 0;
	inode    = strchr(dev,' ');
	*inode++ = 0;
	fn       = strchr(inode,' ');
	*fn++    = 0;
	file     = strchr(fn,'/');
	if ( !file ) file = strchr(fn,'[');
	if ( !file ) file = "";
	//mtcp_printf("%17s-%-16s p:%s o:%-12s d:%5s i:%-12s f:%s\n",low,high,prot,off,dev,inode,file);
	Area& a = m_areas[count];
	::sscanf(low,"%lx",&a.low);
	::sscanf(high,"%lx",&a.high);
	::sscanf(off,"%lx",&a.offset);
	a.size = a.high - a.low;
	*(int*)a.prot = *(int*)prot;
	a.name_len = m_strcpy(a.name,file);
	count++;
      }
      if ( n == 0 ) break;
      p = text;
      *p = ' ';
    }
    text[sizeof(text)-1]=0;
    return 1;
  }
  return 0;
}

/// Write descriptor information about the memory mappings to file
int MemMaps::writeDescriptors(int fd) {
  if ( fd > 0 && m_areas ) {
    const char* s = "<AreaDescriptors>";
    int rc = ::write(fd,s,strlen(s));
    for(int i=0; i<m_numArea; ++i)
      rc += m_areas[i].write(fd);
    s = "</AreaDescriptors>";
    rc += ::write(fd,s,strlen(s));
    return rc;
  }
  return -1;
}

/// Dump descriptor information about the memory mappings
void MemMaps::dump() {
  size_t siz = 0;
  long pg = sysconf(_SC_PAGESIZE);
  long sz = pg - 1;
  if ( !m_areas ) collect();
  for(int i=0; i<m_numArea; ++i) {
    const Area& a = m_areas[i];
    a.print();
    siz += size_t((a.size+sz)/pg)*pg;
  }
  mtcp_printf("--------------- Total memory size:%ld Bytes\n",siz);
}

/// Write descriptor information and data from the memory mappings to file
long MemMaps::write(int fd) {
  if ( fd > 0 && m_areas ) {
    long rc = ::write(fd,&m_numArea,sizeof(m_numArea));
    const char* s = MEMMAP_BEGIN;
    rc += ::write(fd,MEMMAP_BEGIN,strlen(MEMMAP_BEGIN));
    for(int i=0; i<m_numArea; ++i)
      rc += m_areas[i].write(fd);
    s = "</MemMap>";
    rc += ::write(fd,MEMMAP_END,strlen(MEMMAP_END));
    mtcp_printf("Write %ld bytes of mmap area data\n",rc);
    return rc;
  }
  return -1;
}

/// Read descriptor information and data from the memory mappings from file
long MemMaps::read(int fd) {
  return read(fd, AreaCollector(this));
}

/// Read descriptor information and data from the memory mappings from file
long MemMaps::read(int fd, const AreaHandler& handler) {
  if ( fd > 0 ) {
    char text[256];
    long rc = ::read(fd,&m_numArea,sizeof(m_numArea));
    rc += ::read(fd,text,strlen(MEMMAP_BEGIN));
    Area a;
    for(int i=0; i<m_numArea; ++i) {
      rc += a.read(fd);
      handler.handle(i,a);
    }
    rc += ::read(fd,text,strlen(MEMMAP_END));
    mtcp_printf("Read %ld bytes of mmap area data\n",rc);
    return rc;
  }
  return -1;
}

int test_MemMaps_read() {
  MemMaps m;
  int fd = ::open("test.dat",O_RDONLY);
  int rc = m.read(fd);
  ::close(fd);
  return rc;
}

int test_MemMaps_write() {
  MemMaps m;
  m.collect();
  m.dump();
  int fd = ::open("test.dat",O_CREAT|O_TRUNC|O_WRONLY,S_IWUSR|S_IRUSR);
  int rc = m.write(fd);
  ::close(fd);
  return rc;
}


#include "CheckPointing/Static.h"

#define PAGE_SIZE 4096
#include <sys/resource.h>



int test_MemMaps_sharable() {
  mtcp_set_debug_level(0);
  void* curr_break = mtcp_sys_brk(0);
  void* new_break = mtcp_sys_brk (mtcp_saved_break);

  mtcp_output(MTCP_INFO,"restore: curr break %p, saved break %p, shareable: begin %p, end %p\n", 
	      curr_break, mtcp_saved_break, mtcp_shareable_begin, mtcp_shareable_end);

  if (((VA)curr_break > (VA)mtcp_shareable_begin) && ((VA)mtcp_saved_break < (VA)mtcp_shareable_end)) {
    mtcp_output(MTCP_ERROR,"restore: current_brk %p, mtcp_saved_break %p, mtcp_shareable_begin %p, mtcp_shareable_end %p\n", 
                  curr_break, mtcp_saved_break, mtcp_shareable_begin, mtcp_shareable_end);
    mtcp_abort ();
  }

  if (new_break == (void*)-1) {
    mtcp_output(MTCP_FATAL,"restore: sbrk(%p): errno: %d (bad heap)\n",mtcp_saved_break,mtcp_sys_errno);
  }

  /// Stack limit
  struct rlimit stack_rlimit;
  ::getrlimit(RLIMIT_STACK, &stack_rlimit);
  mtcp_output(MTCP_INFO,"checkpoint: Stack resource limit: soft:%p, hard:%p\n", 
	      stack_rlimit.rlim_cur, stack_rlimit.rlim_max);

  /// Sharable image adress space
  restore_addrspace_begin = (((VA)mtcp_shareable_begin) & -PAGE_SIZE);
  restore_addrspace_size  = ((VA)mtcp_shareable_end - restore_addrspace_begin + PAGE_SIZE - 1) & -PAGE_SIZE;
  restore_addrspace_end   = restore_addrspace_begin + restore_addrspace_size;
  mtcp_output(MTCP_INFO,"checkpoint: image of size %X at %p -> %p\n",
	      restore_addrspace_size, restore_addrspace_begin, restore_addrspace_end);

  // The finishRestore function pointer:
  mtcp_output(MTCP_INFO,"checkpoint: finishRestore: %p\n",restore_finish_function_pointer);

  return 1;
}
