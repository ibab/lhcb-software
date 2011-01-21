#define CHECKPOINTING_STATIC_CPP
#include "Checkpointing/Namespace.h"
#include "Checkpointing/Static.h"
#include "Checkpointing/MemMaps.h"
#include "Checkpointing/SysCalls.h"
#include "Checkpointing.h"
#include "Restore.h"
#include "Save.h"

#include <cstdio>
#include <unistd.h>
#include <sys/resource.h>

using namespace Checkpointing;

namespace CHECKPOINTING_NAMESPACE {
  SysInfo chkpt_sys;
}

#ifndef __STATIC__
DefineMarker(SYS_BEGIN_MARKER,    "PSYS");
DefineMarker(SYS_END_MARKER,      "psys");
#endif

#define PAGE_SIZE 4096

SysInfo::SysInfo() {
  struct rlimit lim;

  ::getrlimit(RLIMIT_NOFILE, &lim);
  startRestore       = checkpointing_sys_restore_start;
  finishRestore      = 0;

  stackLimitCurr     = 0;
  stackLimitHard     = 0;
  saved_break        = 0;
  addrSize           = 0;
  addrStart          = 0;
  addrEnd            = 0;
  chkptStart         = 0;
  chkptSize          = 0;
  sysInfo            = 0;
  pageSize           = ::sysconf(_SC_PAGESIZE);
  checkpointFD       = lim.rlim_max-1;
  checkpointFile[0]  = 0;
  checkpointImage[0] = 0;
  motherofall        = 0;
  motherPID          = 0;
  chkptPID           = 0;
  restart_type       = 0;
  restart_flags      = 0;
}

void SysInfo::setCheckpointFile(int fd) {
  char text[64], nam[PATH_MAX];
  ::sprintf(text,"/proc/self/fd/%d",fd);
  int name_len = (int)::readlink(text,nam,sizeof(nam)-1);
  nam[name_len] = 0;
  m_memcpy(checkpointFile,nam,sizeof(checkpointFile));
  checkpointFD = fd;
}

void SysInfo::aquire() {
  struct rlimit lim;

  // Get Stack limit
  ::getrlimit(RLIMIT_STACK, &lim);
  stackLimitCurr = lim.rlim_cur;
  stackLimitHard = lim.rlim_max;

  // Check heap
  void* next_break = mtcp_sys_brk(saved_break);
  if ( next_break == (void*)-1 ) {
    mtcp_output(MTCP_FATAL,"restore: sbrk(%p): errno: %d (bad heap)\n",saved_break,mtcp_sys_errno);
  }
  if ( 0 == addrSize || 0 == chkptSize || !checkpointImage[0] ) {
    AreaInfoHandler hdlr;
    MemMaps mm;
    if ( 1 == mm.scan(hdlr) ) {
      size_t sz = hdlr.imageAddr[1]-hdlr.imageAddr[0];
      sz = (sz+PAGE_SIZE-1) & -PAGE_SIZE;
      m_memcpy(checkpointImage,hdlr.image,sizeof(checkpointImage));
      chkptStart = hdlr.checkpointAddr[0];
      chkptSize  = hdlr.checkpointAddr[1]-hdlr.checkpointAddr[0];
      addrSize   = sz;//high-low;
      addrStart  = hdlr.imageAddr[0];
      addrEnd    = hdlr.imageAddr[0]+addrSize;
      return;
    }
    mtcp_output(MTCP_FATAL,"restore: Failed to access checkpointing image.\n");
  }
}

/// Print data content
void SysInfo::print() {
  checkpointing_sys_print(*this);
}

long SysInfo::write(int fd) {
  return checkpointing_sys_fwrite(fd, this);
}
