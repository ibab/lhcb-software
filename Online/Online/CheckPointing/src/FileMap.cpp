#include "CheckPointing/FileMap.h"
#include "CheckPointing/SysCalls.h"
#include "CheckPointing.h"

#include <unistd.h>

using namespace CheckPointing;


static const unsigned int FILEMAP_START_MARKER = *(unsigned int*)"FMAP";
static const unsigned int FILEMAP_END_MARKER   = *(unsigned int*)"fmap";

/// Handle single file entry
void FilePrintHandler::handle(int fdnum)  const {
  FileDesc dsc;
  if ( 1 == dsc.setup(fdnum) ) {
    dsc.print();
  }
}

/// Start the write cycle
int FileWriteHandler::start()  {
  m_count = 0;
  m_offset = mtcp_sys_lseek (m_fd, 0, SEEK_CUR);
  if ( m_offset < 0 ) {
    mtcp_output(MTCP_ERROR,"FileWriteHandler: Cannot get output position [%d].\n",mtcp_sys_errno);
    return 0;
  }
  m_bytes += writeMarker(m_fd,FILEMAP_START_MARKER);
  m_bytes += ::write(m_fd,&m_count,sizeof(int));
  return 1;
}

/// Stop and finish the write cycle
int FileWriteHandler::stop()  {
  long rc = writeMarker(m_fd,FILEMAP_END_MARKER);
  m_bytes += rc;
  long off = mtcp_sys_lseek (m_fd, 0, SEEK_CUR);
  if ( off < 0 ) {
    mtcp_output(MTCP_ERROR,"FileWriteHandler: Cannot get output position [%d].\n",mtcp_sys_errno);
    return 0;
  }
  if ( mtcp_sys_lseek (m_fd, m_offset, SEEK_SET) < 0 ) {
    mtcp_output(MTCP_ERROR,"FileWriteHandler: Cannot get output position [%d].\n",mtcp_sys_errno);
    return 0;
  }
  rc += ::write(m_fd,&m_count,sizeof(int));
  if ( mtcp_sys_lseek (m_fd, off, SEEK_SET) < 0 ) {
    mtcp_output(MTCP_ERROR,"FileWriteHandler: Cannot get output position [%d].\n",mtcp_sys_errno);
    return 0;
  }
  return rc>0 ? 1 : 0;
}

/// Handle single file entry
void FileWriteHandler::handle(int fdnum) const {
  FileDesc dsc;
  if ( 1 == dsc.setup(fdnum) ) {
    long n = dsc.write(m_fd);
    if ( n > 0 ) {
      ++m_count;
      m_bytes += n;
      return;
    }
    mtcp_output(MTCP_ERROR,"FileMap: error saving file information for FD:%d\n",fdnum);
  }
  mtcp_output(MTCP_ERROR,"FileMap: accessing file information for FD:%d\n",fdnum);
}


/// Start the read cycle
int FileReadHandler::start()  {
  m_offset = mtcp_sys_lseek (m_fd, 0, SEEK_CUR);
  if ( m_offset < 0 ) {
    mtcp_output(MTCP_ERROR,"FileReadHandler: Cannot get output position [%d].\n",mtcp_sys_errno);
    return 0;
  }
  m_bytes += checkMarker(m_fd,FILEMAP_START_MARKER);
  return 1;
}

/// Stop and finish the read cycle
int FileReadHandler::stop()  {
  long rc = checkMarker(m_fd,FILEMAP_END_MARKER);
  m_bytes += rc;
  return rc>0 ? 1 : 0;
}


/// Handle single file entry
void FileReadHandler::handle(int fdnum) const {
  FileDesc dsc;
  if ( 1 == dsc.setup(fdnum) ) {
    dsc.read(m_fd, false);
  }
}



#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>



FileMap::FileMap() : m_numFiles(0) {
}

int FileMap::scan(const FileHandler& handler)    {
  // Open /proc/self/fd directory - it contains a list of files I have open
  int dsiz = -1, fddir = mtcp_sys_open("/proc/self/fd",O_RDONLY,0);
  if (fddir >= 0) {
    char* p;
    int count = 0;
    char dbuf[BUFSIZ];
    struct dirent *dent;
    while (1) {        // Check each entry
      dsiz = -1;
      if (sizeof(dent->d_ino) == 4) dsiz = mtcp_sys_getdents  (fddir,dbuf,sizeof(dbuf));
      if (sizeof(dent->d_ino) == 8) dsiz = mtcp_sys_getdents64(fddir,dbuf,sizeof(dbuf));
      if (dsiz <= 0) break;
      for (int doff = 0; doff < dsiz; doff += dent->d_reclen) {
	dent = (dirent*)(dbuf + doff);
	// The filename should just be a decimal number = the fd it represents
	// Also, skip the entry for the checkpoint and directory files as we 
	// don't want the restore to know about them
	int fdnum = ::strtol(dent->d_name, &p, 10);
	if ( *p == 0 && fdnum > 2 && fdnum != fddir ) {
	  ++count;
	  handler.handle(fdnum);
	}
      }
    }
    mtcp_sys_close(fddir);
    if (dsiz < 0) {
      mtcp_output(MTCP_ERROR,"FileMap: error reading /proc/self/fd: %s\n",strerror(mtcp_sys_errno));
      return 0;
    }
    mtcp_output(MTCP_INFO,"FileMap: Scanned %d entries.\n",count);
    return 1;
  }
  mtcp_output(MTCP_ERROR,"FileMap: error opening directory /proc/self/fd: %s\n",strerror(errno));
  return 0;
}

int FileMap::restore(int in, const FileHandler& handler) {
  if ( in >= 0 ) {
    long rc = 0;
    rc += checkMarker(in,FILEMAP_START_MARKER);
    
    rc += checkMarker(in,FILEMAP_END_MARKER);
    return rc;
  }
  return -1;
}

#if 0

/**  Read file descriptor info from checkpoint file and re-open and re-position files on the same descriptors 
 *  Move the checkpoint file to a different fd if needed
 */
int ForkProcess::setFileDescriptors(const vector<FileDescriptor>& descrs)    {
  for(vector<FileDescriptor>::const_iterator i=descrs.begin(); i!=descrs.end();++i)  {
    const FileDescriptor& d = *i;
    int          fdnum   = d.fdnum;
    const struct stat& statbuf(d.statbuf);
    /// Close all regular files
    if ( S_ISREG(statbuf.st_mode) || S_ISSOCK(statbuf.st_mode) )  {
      if ( fdnum != 0 ) ::close(fdnum);
    }
  }
  for(vector<FileDescriptor>::const_iterator i=descrs.begin(); i!=descrs.end();++i)  {
    const FileDescriptor& d = *i;
    int          fdnum   = d.fdnum;
    off_t        offset  = d.offset;
    const struct stat& statbuf(d.statbuf);
    char         linkbuf[PATH_MAX];

    ::memcpy(linkbuf,d.linkbuf,sizeof(d.linkbuf));
    mtcp_output(MTCP_DEBUG,"mtcp restoreFileDescriptors*: restoring %d -> %s %s %s\n", 
		fdnum, linkbuf, d.tmpbuf ? "YES" : "NO", d.isdel ? "(deleted)" : "");

    // Open the file on a temp fd
    int flags = O_RDWR;
    if (!(statbuf.st_mode & S_IWUSR)) flags = O_RDONLY;
    else if (!(statbuf.st_mode & S_IRUSR)) flags = O_WRONLY;

    int mode = 0;
    if ( d.tmpbuf ) {
      ::tmpnam(linkbuf);
      flags |= O_RDWR | O_CREAT;
      mode = 0777;
    }
    int fd = mtcp_sys_open(linkbuf, flags, mode);
    if (fd < 0)    {
      mtcp_output(MTCP_ERROR,"mtcp restoreFileDescriptors: error %d opening %s flags %o\n", 
		   mtcp_sys_errno, linkbuf, flags);
      if (mtcp_sys_errno == EACCES) mtcp_output(MTCP_ERROR,"  Permission denied.\n");
      continue;
    }

    // Move it to the original fd if it didn't coincidentally open there
    if (fd != fdnum) {
      if (mtcp_sys_dup2 (fd, fdnum) < 0) {
        mtcp_output(MTCP_ERROR,"mtcp restoreFileDescriptors: error %d duping %s from %d to %d\n", 
		    mtcp_sys_errno, linkbuf, fd, fdnum);
        return 0;
      }
      mtcp_sys_close (fd);
    }

    if ( d.tmpbuf ) {
      ::write(fdnum,d.tmpbuf,statbuf.st_size);
    }

    /* Position the file to its same spot it was at when checkpointed */
    if (S_ISREG (statbuf.st_mode) && (mtcp_sys_lseek (fdnum, offset, SEEK_SET) != offset)) {
      mtcp_output(MTCP_ERROR,"mtcp restoreFileDescriptors: error %d positioning %s to %ld\n", 
		  mtcp_sys_errno, linkbuf, (long)offset);
      return 0;
    }
    if ( d.isdel ) ::remove(linkbuf);
  }
  return 1;
}
#endif

int test_FileMap_scan() {
  mtcp_set_debug_level(0);
  FileMap m;
  int fd=::open("/home/frankm/.bashrc",O_RDONLY);
  m.scan(FilePrintHandler());
  ::close(fd);
  fd=::open("/home/frankm/.bashrc",O_RDONLY|O_APPEND);
  m.scan(FilePrintHandler());
  ::close(fd);
  return 1;
}

int test_FileMap_write() {
  mtcp_set_debug_level(0);
  FileMap m;
  int fd1=::open("/home/frankm/.bashrc",O_RDONLY);
  int fd2=::open("test.dat",O_WRONLY|O_CREAT|O_TRUNC,0777);
  m.scan(FileWriteHandler(fd2));
  ::close(fd1);
  ::close(fd2);
  return 1;
}

int test_FileMap_read() {
  mtcp_set_debug_level(0);
  FileMap m;
  ::close(0);
  int fd1=::open("test.dat",O_RDONLY);
  m.scan(FileReadHandler(fd1));
  ::close(fd1);
  return 1;
}
