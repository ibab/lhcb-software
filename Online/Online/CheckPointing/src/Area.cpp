#include "CheckPointing/Static.h"
#include "CheckPointing/MemMaps.h"
#include "CheckPointing/SysCalls.h"
#include "CheckPointing.h"

#include <cerrno>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/mman.h>

using namespace CheckPointing;

static const Marker MEMAREA_BEGIN_MARKER = *(Marker*)"AREA";
static const Marker MEMAREA_END_MARKER   = *(Marker*)"area";

/// Read memory area descriptor and data from file given by the file handle
int Area::read(const void* ptr) {
  const unsigned char* in = (const unsigned char*)ptr;
  int l = 0;
  int len = sizeof(Area)-sizeof(name);
  in += checkMarker(in,MEMAREA_BEGIN_MARKER);
  in += m_memcpy(this,in,len);
  in += m_memcpy(name,in,name_len+1);
  in += m_memcpy(&l,in,sizeof(l));
  if ( l ) in += l;
  in += checkMarker(in,MEMAREA_END_MARKER);
  print("Read memory area:");
  return addr_diff(in,ptr);
}

/// Write memory area descriptor and data to file given by the file handle
int Area::write(void* ptr)    const {
  unsigned char* out = (unsigned char*)ptr;
  if ( out!=0 && strcmp(name,chkpt_sys.checkpointFile)!=0 ) {
    int flg = mapFlags();
    size_t len = sizeof(Area)-sizeof(name)+name_len+1;
    out += saveMarker(out,MEMAREA_BEGIN_MARKER);
    out += m_memcpy(out,this,len);
    // This may be optimized:
    // We normally only need:
    // if (flg & MAP_ANONYMOUS || flg & MAP_SHARED)
    
    if ( strncmp(name,"[stack]",7)==0 ) {
      // Only write NULL-size marker
      out += saveInt(out,0);
    }
    //else if ( strcmp(name,chkpt_sys.checkpointFile)==0 ) {
    //  // Only write NULL-size marker
    //  out += saveInt(out,0);
    //}
    else if ( prot[0]=='r' && (name_len == 0 || name[0]=='[') ) {
      out += saveInt(out,size);
      if ( flg & MAP_SHARED ) {
	// invalidate shared memory pages so that the next read to it 
	// (when we are writing them to ckpt file) will cause them 
	// to be reloaded from the disk 
	if ( mtcp_sys_msync(low,size,MS_INVALIDATE) < 0 ){
	  mtcp_output(MTCP_FATAL,"sync_shared_memory: error %d Invalidating %X at %p from %s + %X",
		      mtcp_sys_errno,size,low,name,offset);
	}
      }
      out += m_memcpy(out,(void*)low,size);
    }
    else {
      // Only write NULL-size marker
      out += saveInt(out,0);
    }
    out += saveMarker(out,MEMAREA_END_MARKER);
    print("Wrote memory area:");
    return addr_diff(out,ptr);
  }
  return 0;
}

/// returns the full spze requirement to save this memory area
int Area::length() const {
  int len = 2*sizeof(Marker) + sizeof(Area) - (sizeof(name) - name_len - 1) + sizeof(int);
  if ( prot[0]=='r' && (name_len == 0 || name[0]=='[') )
    len += size;
  return len;
}

/// Print area descriptor to standard output
void Area::print(const char* opt) const {
  mtcp_output(MTCP_INFO,"%s%016x-%016x %8d %c%c%c%c o:%012x %s\n",
	      opt,low,high,size,prot[0],prot[1],prot[2],prot[3],offset,name);
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

