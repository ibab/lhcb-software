#include "Checkpointing/MMap.h"
#include "Checkpointing/Static.h"
#include "Checkpointing/SysCalls.h"
#include "Checkpointing.h"

#include <cstdio>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace Checkpointing;

int MMap::create(const char* file_name, long len) {
  int fd = ::open(file_name,O_CREAT|O_TRUNC|O_WRONLY,S_IWUSR|S_IRUSR);
  if ( fd != -1 ) {
    if ( len < 0 )  {
      m_fd = fd;
      m_addr = 0;
      m_length = 0;
      return 1;
    }
    if ( 0 == ::ftruncate(fd,len) ) {
      ::close(fd);
      fd = ::open(file_name,O_RDWR);
      if ( fd != chkpt_sys.checkpointFD ) {
	if (mtcp_sys_dup2 (fd, chkpt_sys.checkpointFD) < 0) {
	  mtcp_output(MTCP_ERROR,"FileDesc: cannot mmap file %s to fid:%d. %d:%s\n",
		      file_name, chkpt_sys.checkpointFD, mtcp_sys_errno, strerror(mtcp_sys_errno));
	  ::close (fd);
	  return 0;
	}
	::close (fd);
	fd = chkpt_sys.checkpointFD;
      }
      long ll = len;
      void* mem = ::mmap(0,ll,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
      if ( mem != MAP_FAILED ) {
	mtcp_output(MTCP_INFO,"Mapped file %s: with memory %p -> %p [%d bytes]\n",
		    file_name,mem,((char*)mem)+ll,ll);
	m_fd = fd;
	m_addr = (char*)mem;
	m_length = len;
	return 1;
      }
      ::close(fd);
      mtcp_output(MTCP_ERROR,"Failed to mmap file %s: %s\n",file_name,strerror(errno));
      return 0;
    }
    ::close(fd);
    mtcp_output(MTCP_ERROR,"Failed to extend file %s by %ld: %s\n",file_name,len,strerror(errno));
    return 0;
  }
  mtcp_output(MTCP_ERROR,"Failed to open file %s: %s\n",file_name,strerror(errno));
  return 0;
}

#if 0
int MMap::commit(long len) {
  if ( -1 != m_fd ) {
    if ( len < 0 ) {
      errno = EINVAL;
      return 0;
    }
    if ( len < m_length ) {
      if ( 0 != ::msync(m_addr,m_length,MS_SYNC) ) {
	mtcp_output(MTCP_ERROR,"Failed to sync memory mapping:%s\n",strerror(errno));
      }
      if ( 0 != ::munmap(m_addr,m_length) ) {
	mtcp_output(MTCP_ERROR,"Failed to unmap memory mapping:%s\n",strerror(errno));
      }
      ::ftruncate(m_fd,len);
      ::close(m_fd);
      m_length = len;
      m_addr = 0;
      m_fd = 0;
      return 1;
    }
  }
  errno = EBADF;
  return 0;
}
#endif

int MMap::open(const char* file_name) {
  struct stat buf;
  if ( 0 == ::stat(file_name,&buf) ) {
    long len = buf.st_size;
    int fd = ::open(file_name,O_RDONLY);
    if ( fd != -1 ) {
      if ( fd != chkpt_sys.checkpointFD ) {
	if (mtcp_sys_dup2 (fd, chkpt_sys.checkpointFD) < 0) {
	  mtcp_output(MTCP_ERROR,"FileDesc: cannot mmap file %s to fid:%d. %d:%s\n",
		      file_name, chkpt_sys.checkpointFD, mtcp_sys_errno, strerror(mtcp_sys_errno));
	  ::close (fd);
	  return 0;
	}
	::close (fd);
	fd = chkpt_sys.checkpointFD;
      }
      void* mem = ::mmap(0,len,PROT_READ,MAP_PRIVATE,fd,0);
      if ( mem != MAP_FAILED ) {
	m_fd = fd;
	m_addr = (char*)mem;
	m_length = len;
	mtcp_output(MTCP_INFO,"Mapped file %s: with %d bytes READ\n",file_name,len);
	return 1;
      }
      ::close(fd);
      mtcp_output(MTCP_ERROR,"Failed to mmap file %s: %s\n",file_name,strerror(errno));
      return 0;
    }
    mtcp_output(MTCP_ERROR,"Failed to open file %s: %s\n",file_name,strerror(errno));
    return 0;
  }
  mtcp_output(MTCP_ERROR,"Failed to access file %s: %s\n",file_name,strerror(errno));
  return 0;
}

int MMap::close() {
  if ( m_addr ) {
    ::munmap(m_addr,m_length);
    m_length = 0;
    m_addr = 0;
  }
  if ( -1 != m_fd ) {
    ::close(m_fd);
    m_fd = 0;
  }
  return 1;
}
