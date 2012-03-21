#include "Checkpointing/Namespace.h"
#include "Checkpointing/Static.h"
#include "Checkpointing/MemMaps.h"
#include "Checkpointing/SysCalls.h"
#include "Checkpointing.h"
#include "Restore.h"

#include <cerrno>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/mman.h>
#include <ucontext.h>

using namespace Checkpointing;

#ifndef __STATIC__
DefineMarker(MEMAREA_BEGIN_MARKER,"AREA");
DefineMarker(MEMAREA_END_MARKER,  "area");
#endif

/// Read memory area descriptor and data from file given by the file handle
int Area::read(const void* ptr, const AreaHandler& handler) {
  int rc = checkpointing_area_read(this,ptr,handler.f_map,&handler);
  return rc;
}

/// Write memory area descriptor and data to file given by the file handle
int Area::streamOut(void* ptr, bool write_nulls)    const {
  unsigned char* out = (unsigned char*)ptr;
  if ( 0 == out ) return 0;
  else if ( m_strcmp(name,chkpt_sys.checkpointFile)  == 0 ) return 0;
  else if ( m_strcmp(name,chkpt_sys.checkpointImage) == 0 ) return 0;
  else {
    int flg = mapFlags();
    size_t len = sizeof(Area)-sizeof(name)+name_len+1;
    out += saveMarker(out,MEMAREA_BEGIN_MARKER);
    out += m_memcpy(out,this,len);
    // This may be optimized:
    // We normally only need:
    len = dataLength();
    if ( !write_nulls && 0 == (len=dataLength()) ) {
      // Only streamOut NULL-size marker
      out += saveInt(out,0);
    }
    else  {
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
      if ( prot[0] != 'r' ) {
	out += m_memset(out,0,size);
	print(MTCP_DEBUG,"NULL  memory area:");
      }
      else {
	out += m_memcpy(out,(void*)low,size);
	print(MTCP_DEBUG,"Wrote memory area:");
      }
    }
    out += saveMarker(out,MEMAREA_END_MARKER);
    print(MTCP_DEBUG,"Wrote memory area:");
    return addr_diff(out,ptr);
  }
}

/// Write memory area descriptor and data to file given by the file handle
int Area::write(int fd, bool write_nulls)    const {
  if ( fd <= 0 ) return 0;
  else if ( m_strcmp(name,chkpt_sys.checkpointFile)  == 0 ) return 0;
  else if ( m_strcmp(name,chkpt_sys.checkpointImage) == 0 ) return 0;
  else {
    int    bytes = 0;
    int    flg = mapFlags();
    size_t len = sizeof(Area)-sizeof(name)+name_len+1;

    bytes += writeMarker(fd,MEMAREA_BEGIN_MARKER);
    bytes += m_writemem(fd,this,len);
    // This may be optimized:
    // We normally only need:
    // if (flg & MAP_ANONYMOUS || flg & MAP_SHARED)
    len = dataLength();
    if ( !write_nulls && 0 == (len=dataLength()) ) {
      // Only write NULL-size marker
      bytes += writeInt(fd,0);
      print(MTCP_DEBUG,"EMPTY memory area:");
    }
    else   {
      bytes += writeInt(fd,size);
      if ( flg & MAP_SHARED ) {
	// invalidate shared memory pages so that the next read to it 
	// (when we are writing them to ckpt file) will cause them 
	// to be reloaded from the disk 
	if ( mtcp_sys_msync(low,size,MS_INVALIDATE) < 0 ){
	  mtcp_output(MTCP_FATAL,"sync_shared_memory: error %d Invalidating %X at %p from %s + %X",
		      mtcp_sys_errno,size,low,name,offset);
	}
      }
      if ( prot[0] != 'r' ) {
	bytes += m_writeset(fd,0,size);
	print(MTCP_DEBUG,"NULL  memory area:");
      }
      else {
	long* ptr, *start, *sp;
	if ( m_strcmp(name,"[stack]") == 0 ) {
	  sp = (long*)low;
	  start = ptr = (long*)malloc(((size/sizeof(long))+2)*sizeof(long));
	}
	if ( m_strcmp(name,"[stack]") == 0 ) {
#if 0
#endif
	  for(;sp<(long*)high;++ptr,++sp) *ptr = *sp;
	  bytes += m_writemem(fd,start,size);
	  ::free(start);
	  if ( ::getcontext (chkpt_sys.savedContext) < 0 )   {
	    mtcp_output(MTCP_FATAL,"checkpointing_sys_get_context: failed to retrieve context of motherofall.\n");
	  }
	  if ( chkpt_sys.restart_type == SysInfo::RESTART_CHECKPOINT ) {
	    return 1;
	  }
	  //bytes += m_writemem(fd,(void*)low,size);
	}
	else 
	{
	  bytes += m_writemem(fd,(void*)low,size);
	}
	print(MTCP_DEBUG,"Wrote memory area:");
      }
    }
    bytes += writeMarker(fd,MEMAREA_END_MARKER);
    return bytes;
  }
}

/// Calculate the size of the data to be written
int Area::dataLength()  const {
  return checkpointing_area_datalength(this);
}

/// returns the full spze requirement to save this memory area
int Area::length() const {
  int len = 2*sizeof(Marker) + sizeof(Area) - (sizeof(name) - name_len - 1) + sizeof(int);
  len += dataLength();
  return len;
}

/// Print area descriptor to standard output
void Area::print(const char* opt) const {
  checkpointing_area_print(this,MTCP_INFO,opt);
}

/// Print area descriptor to standard output
void Area::print(int lvl,const char* opt) const {
  checkpointing_area_print(this,lvl,opt);
}

/// Access protection flags for this memory area
int Area::protection() const {
  return checkpointing_area_protection(this);
}

/// Access mmap flags for this memory area
int Area::mapFlags() const {
  return checkpointing_area_mapFlags(this);
}

/// Simple check if the memory area is mapped to a file
int Area::isFile() const {
  return name[0] == '/';
}

