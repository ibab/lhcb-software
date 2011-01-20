#include "CheckPointing/Static.h"
#include "CheckPointing/Process.h"
#include "CheckPointing/MemMaps.h"
#include "CheckPointing/FileMap.h"
#include "CheckPointing.h"
#include "Restore.h"

using namespace CheckPointing;

#ifndef __STATIC__
DefineMarker(PROCESS_BEGIN_MARKER,"PROC");
DefineMarker(PROCESS_END_MARKER,  "proc");
#endif

/// Read full process information from checkpoint file
int Process::read(const void* addr) {
  return checkpointing_process_read(this,addr);
}

/// Read header information from checkpoint file
int Process::readHeader(const void* addr)   {
  return checkpointing_process_read_header(this,addr);
}

/// Read trailer information from checkpoint file
int Process::readTrailer(const void* addr)  {
  return checkpointing_process_read_trailer(this,addr);
}

/// Read system information from checkpoint file
int Process::readSysInfo(const void* addr)   {
  return checkpointing_process_skip_sys(this,addr);
}

/// Read the file descriptor information to checkpoint file
int Process::readFiles(const void* addr)   {
  return checkpointing_process_read_files(this,addr);
}

/// Read the memory areas to checkpoint file
int Process::readMemory(const void* addr)   {
  return checkpointing_process_read_memory(this,addr);
}

/// Finalize restore process
int Process::restoreFinish() {
  if ( m_flag == PROCESS_RESTORE )   {
    checkpointing_sys_restore_finish();
    return 1;
  }
  return 0;
}

/// Write full process information to checkpoint file
int Process::write(int fd)  const {
  long rc = 0, tot = 0;
  if ( fd != 0 ) {
    if ( (rc=writeHeader(fd)) ) {
      tot += rc;
      mtcp_output(MTCP_INFO,"checkpoint: Wrote %ld [Total:%ld] bytes of process header.\n",rc,tot);
      if ( (rc=writeSysInfo(fd)) ) {
	tot += rc;
	mtcp_output(MTCP_INFO,"checkpoint: Wrote %ld [Total:%ld] bytes of System information.\n",rc,tot);
	if ( (rc=writeFiles(fd)) ) {
	  tot += rc;
	  mtcp_output(MTCP_INFO,"checkpoint: Wrote %ld [Total:%ld] bytes of file information.\n",rc,tot);
	  if ( (rc=writeMemory(fd)) ) {
	    tot += rc;
	    mtcp_output(MTCP_INFO,"checkpoint: Wrote %ld [Total:%ld] bytes of process memory.\n",rc,tot);
	    if ( (rc=writeTrailer(fd)) ) {
	      tot += rc;
	      mtcp_output(MTCP_INFO,"checkpoint: Wrote %ld [Total:%ld] bytes of process trailer.\n",rc,tot);
	      return tot;
	    }
	  }
	}
      }
    }
  }
  return 0;
}

/// Write header information to checkpoint file
int Process::writeHeader(int fd)  const {
  int rc = writeMarker(fd,PROCESS_BEGIN_MARKER);
  return rc;
}

/// Write trailer information to checkpoint file
int Process::writeTrailer(int fd)  const {
  size_t sz = writeMarker(fd,PROCESS_END_MARKER);
  return sz;
}

/// Write system information to checkpoint file
int Process::writeSysInfo(int fd)  const {
  SysInfo& sys = chkpt_sys;
  sys.aquire();
  sys.print();
  long len = sys.write(fd);
  if ( len ) {
    return len;
  }
  mtcp_output(MTCP_ERROR,"Failed to write System information!\n");
  return 0;
}

/// Write the file descriptor information to checkpoint file
int Process::writeFiles(int fd)  const {
  FileMap f;
  FileWriteHandler wr(fd);
  long len = f.scan(wr.start());
  wr.stop();
  if ( len ) {
    return wr.bytes();
  }
  mtcp_output(MTCP_ERROR,"Failed to write file information!\n");
  return 0;
}

/// Write the memory areas to checkpoint file
int Process::writeMemory(int fd)  const {
  MemMaps m;
  long len = m.write(fd);
  if ( len ) {
    return len;
  }
  mtcp_output(MTCP_ERROR,"Failed to write memory dump!\n");
  return 0;
}
