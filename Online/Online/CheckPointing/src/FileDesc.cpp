#include "CheckPointing/Static.h"
#include "CheckPointing/FileMap.h"
#include "CheckPointing/SysCalls.h"
#include "CheckPointing.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

using namespace CheckPointing;

static const unsigned int FILE_START_MARKER = *(unsigned int*)"FILE";
static const unsigned int FILE_END_MARKER   = *(unsigned int*)"file";
  

int FileDesc::setup(int fdnum) {
  char procfdname[PATH_MAX];
  struct stat f_link;

  fd = -1;
  name[0] = 0;
  isdel   = 0;
  istmp   = 0;
  hasData = 0;
  _pad    = 0;
  // Read the symbolic link so we get the filename that's open on the fd
  ::sprintf(procfdname, "/proc/self/fd/%d",fdnum);
  name_len = (int)::readlink(procfdname,name,sizeof(name)-1);
  if (name_len < 0) {
    mtcp_output(MTCP_ERROR,"FileHandler: error reading %s: %s\n",procfdname,strerror(errno));
    return 0;
  }
  if ((name_len >= 0) || (errno != ENOENT)) { // probably was the proc/self/fd directory itself
    int rc;
    fd = fdnum;
    name[name_len] = 0;

    // Read about the link itself so we know read/write open flags
    rc = lstat (procfdname, &f_link);
    if (rc < 0) {
      mtcp_output(MTCP_ERROR,"FileHandler: error stat %s [%d] -> %s: %s\n",
		  procfdname, fd, name, strerror(-rc));
      return 0;
    }
    // Read about the actual file open on the fd
    rc = fstat(fd, &statbuf);
    // This does not take into account regulare open files, which are written!
    if (rc < 0) {
      mtcp_output(MTCP_WARNING,"FileHandler: error stat %s [%d] -> %s: %s\n",
		  procfdname,fd,name,strerror(-rc));
      return 0;
    }
    istmp = (*(int*)name == *(int*)"/tmp/") == 0 ? 0 : 'y';
    isdel = m_strfind(name,"(deleted)") == 0 ? 0 : 'y';
    hasData = (istmp || isdel) ? 'y' : 0;
    // Replace file's permissions with current access flags so restore will know how to open it
    offset = 0;
    if (S_ISREG (statbuf.st_mode)) offset = mtcp_sys_lseek(fdnum, 0, SEEK_CUR);
    statbuf.st_mode = (statbuf.st_mode & ~0777) | (f_link.st_mode & 0777);
    /// Set file name of restore file
    if ( fd == chkpt_sys.checkpointFD ) {
      m_strcpy(chkpt_sys.checkpointFile,name);
    }

    mtcp_output(MTCP_DEBUG,
		"FileHandler: fstat %s [%d] Dir:%s Reg:%s Sock:%s Fifo:%s Block:%s Tmp:%s Del:%s Data:%s\n",
		procfdname, fd,
		S_ISDIR(statbuf.st_mode)  ? "YES" : "NO", 
		S_ISREG(statbuf.st_mode)  ? "YES" : "NO", 
		S_ISSOCK(statbuf.st_mode) ? "YES" : "NO", 
		S_ISFIFO(statbuf.st_mode) ? "YES" : "NO",
		S_ISBLK(statbuf.st_mode)  ? "YES" : "NO",
		istmp   ? "YES" : "NO",
		isdel   ? "YES" : "NO",
		hasData ? "YES" : "NO"
		);
    mtcp_output(MTCP_DEBUG,"\t->\t%s [%d]\n",name,name_len);
    return 1;
  }
  return 0;
}

/// Print the data to standard output
void FileDesc::print() {
  mtcp_output(MTCP_INFO,
	      "FileHandler: /proc/self/fd/%d -> %s\n\t\tSiz:%d Dir:%s Reg:%s "
	      "Sock:%s Fifo:%s Block:%s Tmp:%s Del:%s\n",
	      fd, name, statbuf.st_size,
	      S_ISDIR(statbuf.st_mode)  ? "YES" : "NO", 
	      S_ISREG(statbuf.st_mode)  ? "YES" : "NO", 
	      S_ISSOCK(statbuf.st_mode) ? "YES" : "NO", 
	      S_ISFIFO(statbuf.st_mode) ? "YES" : "NO",
	      S_ISBLK(statbuf.st_mode)  ? "YES" : "NO",
	      istmp ? "YES" : "NO",
	      isdel ? "YES" : "NO"
	      );
}

/// Reopen file and attach it to the proper file descriptor
int FileDesc::reopen() {
  // Open the file on a temp fd
  int flags = O_RDWR, mode = 0;

  if (!(statbuf.st_mode & S_IWUSR)) flags = O_RDONLY;
  else if (!(statbuf.st_mode & S_IRUSR)) flags = O_WRONLY;

  if ( hasData ) {
    ::tmpnam(name);
    flags |= O_RDWR | O_CREAT;
    mode = 0777;
  }
  int fdnum = mtcp_sys_open(name, flags, mode);
  if (fdnum < 0)    {
    mtcp_output(MTCP_ERROR,"FileDesc: error %d opening %s flags %o Skip file\n", 
		mtcp_sys_errno, name, flags);
    if (mtcp_sys_errno == EACCES) mtcp_output(MTCP_ERROR,"  Permission denied.\n");
    return 0;
  }
  // Move it to the original fd if it didn't coincidentally open there
  if (fd != fdnum) {
    if (mtcp_sys_dup2 (fd, fdnum) < 0) {
      mtcp_output(MTCP_ERROR,"FileDesc: error %d [%s] restore %s fd %d to %d SKIP file.\n", 
		  mtcp_sys_errno, strerror(mtcp_sys_errno), name, fdnum, fd);
      mtcp_sys_close (fdnum);
      return 0;
    }
    mtcp_sys_close (fdnum);
  }
  return 1;
}

/// Write descriptor and possibly data to file identified by fileno
int FileDesc::write(void* address)   const {
  unsigned char* out = (unsigned char*)address;
  if ( out != 0 ) {
    long len = sizeof(FileDesc)-sizeof(name)+name_len+1;
    out += saveMarker(out,FILE_START_MARKER);
    out += m_memcpy(out,this,len);
    if ( hasData ) {
      out += ::pread(fd,out,statbuf.st_size,0);
    }
    out += saveMarker(out,FILE_END_MARKER);
    return addr_diff(out,address);
  }
  return -1;
}

/// Read descriptor and possibly data from file identified by fileno
int FileDesc::read(const void* addr, bool restore) {
  unsigned char* in = (unsigned char*)addr;
  if ( in > 0 ) {
    long len = sizeof(FileDesc)-sizeof(name);
    in += checkMarker(in,FILE_START_MARKER);
    in += m_memcpy(this,in,len);
    in += m_memcpy(name,in,name_len+1);
    restore = !S_ISSOCK(statbuf.st_mode);
    if ( fd == chkpt_sys.checkpointFD ) restore = false;
    restore = restore ? (reopen() == 1) : restore;
    if ( hasData ) {
      if ( restore ) {
	::write(fd,in,statbuf.st_size);
      }
      in += statbuf.st_size;
    }
    if ( restore ) {
      // Position the file to its same spot it was at when checkpointed
      if (S_ISREG(statbuf.st_mode) && (mtcp_sys_lseek (fd, offset, SEEK_SET) != offset)) {
	mtcp_output(MTCP_ERROR,"FileDesc: error %d [%s] positioning %s to %ld\n", 
		    mtcp_sys_errno, strerror(mtcp_sys_errno), name, (long)offset);
	return -1;
      }
      if ( isdel ) ::remove(name);
    }
    in += checkMarker(in,FILE_END_MARKER);
    return addr_diff(in,addr);
  }
  return -1;
}
