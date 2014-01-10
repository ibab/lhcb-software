#include "Checkpointing/Static.h"
#include "Checkpointing/FileMap.h"
#include "Checkpointing/SysCalls.h"
#include "Checkpointing.h"
#include "Restore.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

using namespace Checkpointing;

WEAK(int) FileDesc::setup(int fdnum) {
  char procfdname[PATH_MAX];
  struct stat f_link;
  int is_sock = 0, is_pipe = 0;
  fd = -1;
  name[0] = 0;
  isdel   = 0;
  istmp   = 0;
  hasData = 0;
  _pad    = 0;
  // Read the symbolic link so we get the filename that's open on the fd
  ::sprintf(procfdname, "/proc/self/fd/%d",fdnum);
  name_len = (int)mtcp_sys_readlink(procfdname,name,sizeof(name)-1);
  if (name_len < 0) {
    mtcp_output(MTCP_ERROR,"FileHandler: error reading %s: %s\n",procfdname,strerror(errno));
    return 0;
  }
  if ((name_len >= 0) || (errno != ENOENT)) { // probably was the proc/self/fd directory itself
    int rc;
    fd = fdnum;
    name[name_len] = 0;

    // Read about the link itself so we know read/write open flags
    rc = mtcp_sys_lstat (procfdname, &f_link);
    if (rc < 0) {
      mtcp_output(MTCP_ERROR,"FileHandler: error stat %s [%d] -> %s: %s\n",
                  procfdname, fd, name, strerror(-rc));
      return 0;
    }
    // Read about the actual file open on the fd
    rc = mtcp_sys_fstat(fd, &statbuf);
    // This does not take into account regulare open files, which are written!
    if (rc < 0) {
      mtcp_output(MTCP_WARNING,"FileHandler: error stat %s [%d] -> %s: %s\n",
                  procfdname,fd,name,strerror(-rc));
      return 0;
    }
    istmp = m_intcheck(name,"/tmp/") == 0 ? 0 : 'y';
    isdel = m_strfind(name,"(deleted)") == 0 ? 0 : 'y';
    is_sock = S_ISFIFO(statbuf.st_mode);
    is_pipe = m_strncmp(name,"pipe:[",6)==0;

    hasData = ((istmp || isdel) && !is_sock && !is_pipe) ? 'y' : 0;
    // Replace file's permissions with current access flags so restore will know how to open it
    offset = 0;
    if (S_ISREG (statbuf.st_mode)) offset = mtcp_sys_lseek(fdnum, 0, SEEK_CUR);
    statbuf.st_mode = (statbuf.st_mode & ~0777) | (f_link.st_mode & 0777);
    /// Set file name of restore file
    if ( fd == chkpt_sys.checkpointFD ) {
      m_strcpy(chkpt_sys.checkpointFile,name);
    }
    checkpoint_file_print(MTCP_DEBUG,this);
    return 1;
  }
  return 0;
}

/// Print the data to standard output
WEAK(void) FileDesc::print(int lvl) const  {
  checkpoint_file_print(lvl,this);
}

/// Reopen file and attach it to the proper file descriptor
WEAK(int) FileDesc::reopen() {
  return checkpoint_file_reopen(this);
}

/// Write descriptor and possibly data to memory
WEAK(int) FileDesc::streamOut(void* address)   const {
  //print(MTCP_WARNING);
  if ( name[0] && isdel && chkpt_sys.numTmpFiles<int(sizeof(chkpt_sys.tmpFiles)/sizeof(chkpt_sys.tmpFiles[0])) ) {
    SysInfo::TmpFile& tmp = chkpt_sys.tmpFiles[chkpt_sys.numTmpFiles];
    tmp.fd = fd;
    m_strcpy(tmp.name,name);
    char* p = m_chrfind(tmp.name,' ');
    if ( p ) *p = 0;
    ++chkpt_sys.numTmpFiles;
    mtcp_output(MTCP_DEBUG,"FileHandler: added temp file %d: %s\n",fd,tmp.name);
  }
  else if ( name[0] && isdel )  {
    mtcp_output(MTCP_ERROR,"FileHandler: Cannot add temp file %d: %s -- increase space in sys.tmpFiles.\n",fd,name);
  }
  return checkpoint_file_write(this,address);
}

/// Write descriptor and possibly data to file identified by fileno fd_out
WEAK(int) FileDesc::write(int fd_out)   const {
  //print(MTCP_WARNING);
  if ( name[0] && isdel && chkpt_sys.numTmpFiles<int(sizeof(chkpt_sys.tmpFiles)/sizeof(chkpt_sys.tmpFiles[0])) ) {
    SysInfo::TmpFile& tmp = chkpt_sys.tmpFiles[chkpt_sys.numTmpFiles];
    tmp.fd = fd;
    m_strcpy(tmp.name,name);
    char* p = m_chrfind(tmp.name,' ');
    if ( p ) *p = 0;
    ++chkpt_sys.numTmpFiles;
    mtcp_output(MTCP_DEBUG,"FileHandler: added temp file %d: %s\n",fd,tmp.name);
  }
  else if ( name[0] && isdel )  {
    mtcp_output(MTCP_ERROR,"FileHandler: Cannot add temp file %d: %s -- increase space in sys.tmpFiles.\n",fd,name);
  }
  return checkpoint_file_fwrite(this,fd_out);
}

/// Read descriptor and possibly data from file identified by fileno
WEAK(int) FileDesc::read(const void* addr, bool restore) {
  return checkpoint_file_read(this, addr, restore);
}
