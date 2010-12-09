#include "CheckPointing/MemMaps.h"
#include "CheckPointing/SysCalls.h"
#include "CheckPointing.h"

#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <sys/mman.h>

using namespace CheckPointing;

#define AREA_BEGIN "<Area>"
#define AREA_END   "</Area>"

/// Read memory area descriptor and data from file given by the file handle
int Area::read(int fd) {
  int rc = 0, l = 0;
  char text[128];
  int len = sizeof(Area)-sizeof(name);
  rc += ::read(fd,text,strlen(AREA_BEGIN));
  rc += ::read(fd,this,len);
  rc += ::read(fd,name,name_len);
  rc += ::read(fd,&l,sizeof(l));
  if ( l ) {
    for(;l>0;--l) rc += ::read(fd,text,1);
  }
  rc += ::read(fd,text,strlen(AREA_END));
  checkMarker(fd);
  mtcp_printf("Read memory area:");
  print();
  return rc;
}

/// Write memory area descriptor and data to file given by the file handle
int Area::write(int fd) {
  int flg = mapFlags();
  int rc     = ::write(fd,AREA_BEGIN,strlen(AREA_BEGIN));
  size_t len = sizeof(Area)-sizeof(name)+name_len;
  rc  += ::write(fd,this,len);
  // This may be optimized:
  // We normally only need:
  // if (flg & MAP_ANONYMOUS || flg & MAP_SHARED)

  if ( name_len == 0 || (name[0]=='[' && prot[0]=='r')) {
    int   i = (int)size;
    rc     += ::write(fd,&i,sizeof(int));
    char* p = (char*)low;

    if ( flg & MAP_SHARED ) {
      // invalidate shared memory pages so that the next read to it 
      // (when we are writing them to ckpt file) will cause them 
      // to be reloaded from the disk 
      if ( mtcp_sys_msync(low,size,MS_INVALIDATE) < 0 ){
        mtcp_output(MTCP_FATAL,"sync_shared_memory: error %d Invalidating %X at %p from %s + %X",
		    mtcp_sys_errno,size,low,name,offset);
      }
    }
    for(i=(int)size;i>0;) {
      int n = ::write(fd,p,i);
      if (n>=0) {
	i -= n;
	p += n;
      }
      else {
	mtcp_printf("ERROR Writing memory area:%s ->",::strerror(errno));
	this->print();
	return -1;
      }
    }
    rc += size;
  }
  else {
    // Only write NULL-size marker
    int l = 0;
    rc += ::write(fd,&l,sizeof(int));
  }
  rc += ::write(fd,AREA_END,strlen(AREA_END));
  rc += writeMarker(fd);
  mtcp_printf("Wrote memory area:");
  print();
  return rc;
}

/// Print area descriptor to standard output
void Area::print() const {
  mtcp_printf("%016x-%016x %8d %c%c%c%c o:%012x %s\n",
	      low,high,size,prot[0],prot[1],prot[2],prot[3],offset,name);
}

/// Access protection flags for this memory area
int Area::protection() const {
  int p = 0;
  if ( prot[0]=='r' ) p |= PROT_READ;
  if ( prot[1]=='w' ) p |= PROT_WRITE;
  if ( prot[2]=='x' ) p |= PROT_EXEC;
  return p;
}

/// Access mmap flags for this memory area
int Area::mapFlags() const {
  int f = MAP_FIXED;
  if ( prot[3]  =='s' ) f |= MAP_SHARED;
  if ( prot[3]  =='p' ) f |= MAP_PRIVATE;
  if ( name_len == 0  ) f |= MAP_ANONYMOUS;
  if ((f& MAP_PRIVATE)) f |= MAP_ANONYMOUS;
  return f;
}

/// Simple check if the memory area is mapped to a file
int Area::isFile() const {
  return name[0] == '/';
}

