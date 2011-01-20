#include "Checkpoining/Static.h"
#include "Checkpoining/SysCalls.h"
#include "Checkpoining/MemMaps.h"
#include "Checkpoining.h"
#include "Save.h"
#include <unistd.h>

using namespace Checkpoining;

#ifndef __STATIC__
DefineMarker(SYS_BEGIN_MARKER,    "PSYS");
DefineMarker(SYS_END_MARKER,      "psys");
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
