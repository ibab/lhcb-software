#include "Checkpointing/Static.h"
#include "Checkpointing/SysCalls.h"
#include "Checkpointing/MemMaps.h"
#include "Checkpointing/FileMap.h"
#include "Checkpointing.h"
#include "Save.h"
#include <unistd.h>

using namespace Checkpointing;

#ifndef __STATIC__
DefineMarker(SYS_BEGIN_MARKER,    "PSYS");
DefineMarker(SYS_END_MARKER,      "psys");
DefineMarker(FILE_BEGIN_MARKER,   "FILE");
DefineMarker(FILE_END_MARKER,     "file");
#endif

STATIC(long) CHECKPOINTING_NAMESPACE::checkpointing_sys_fwrite(int fd, const SysInfo* s) {
  MemMaps mm;
  long bytes = writeMarker(fd,SYS_BEGIN_MARKER);
  bytes += writeInt(fd,sizeof(SysInfo));
  bytes += m_writemem(fd,s,sizeof(SysInfo));
  long offset = mtcp_sys_lseek(fd,0,SEEK_CUR);
  bytes += writeInt(fd,0);
  AreaChkptWriteHandler hdlr(fd);
  if ( 1 != mm.scan(hdlr) ) {
    mtcp_output(MTCP_FATAL,"checkpoint: Failed to write checkpointing image.\n");
  }
  int val = hdlr.bytesWritten();
  long off = mtcp_sys_lseek(fd,0,SEEK_CUR);
  mtcp_sys_lseek(fd,offset,SEEK_SET);
  writeInt(fd,val);
  mtcp_sys_lseek(fd,off,SEEK_SET);
  bytes += writeMarker(fd,SYS_END_MARKER);
  mtcp_output(MTCP_INFO,"checkpoint: Wrote %d [%d] bytes of restore image.\n",val,s->addrSize);
  return bytes;
}

/// Write descriptor and possibly data to file identified by fileno fd_out
STATIC(int) CHECKPOINTING_NAMESPACE::checkpoint_file_fwrite(const FileDesc* d,int fd_out) {
  if ( fd_out > 0 ) {
    long len   = sizeof(FileDesc)-sizeof(d->name)+d->name_len+1;
    long bytes = writeMarker(fd_out,FILE_BEGIN_MARKER);
    bytes += m_writemem(fd_out,d,len);
    if ( d->hasData ) {
      char c;
      for(long i=0;i<d->statbuf.st_size;++i) {
	::pread(d->fd,&c,1,i);
	::write(fd_out,&c,1);
      }
      bytes += d->statbuf.st_size;
    }
    bytes += writeMarker(fd_out,FILE_END_MARKER);
    return bytes;
  }
  return -1;
}

/// Write descriptor and possibly data to memory block
STATIC(int) CHECKPOINTING_NAMESPACE::checkpoint_file_write(const FileDesc* d,void* address) {
  unsigned char* out = (unsigned char*)address;
  if ( out != 0 ) {
    long len = sizeof(FileDesc)-sizeof(d->name)+d->name_len+1;
    out += saveMarker(out,FILE_BEGIN_MARKER);
    out += m_memcpy(out,d,len);
    if ( d->hasData ) {
      out += ::pread(d->fd,out,d->statbuf.st_size,0);
    }
    out += saveMarker(out,FILE_END_MARKER);
    return addr_diff(out,address);
  }
  return -1;
}

