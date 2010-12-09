#include "CheckPointing/SysCalls.h"
#include "CheckPointing/ForkProcess.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

int ForkProcess::getFileDescriptors (int fd, vector<FileDescriptor>& descrs)    {
  char dbuf[BUFSIZ], linkbuf[PATH_MAX], *p, procfdname[64];
  int doff, dsiz, fddir, fdnum, linklen, rc;
  off_t offset;
  struct dirent *dent;
  struct stat lstatbuf, statbuf;

  mtcp_output(MTCP_INFO,"mtcp saveFileDescriptors: Start scan.\n");
  
  // Open /proc/self/fd directory - it contains a list of files I have open
  char proc_fd[64] = "/proc/self/fd";
  fddir = mtcp_sys_open (proc_fd, O_RDONLY, 0);
  if (fddir < 0) {
    mtcp_output(MTCP_ERROR,"mtcp saveFileDescriptors: error opening directory /proc/self/fd: %s\n",
		strerror(errno));
    return 0;
  }
  int num_entries = 0;
  // Check each entry
  while (1) {
    dsiz = -1;
    if (sizeof dent->d_ino == 4) dsiz = mtcp_sys_getdents  (fddir, dbuf, sizeof dbuf);
    if (sizeof dent->d_ino == 8) dsiz = mtcp_sys_getdents64(fddir, dbuf, sizeof dbuf);
    if (dsiz <= 0) break;
    for (doff = 0; doff < dsiz; doff += dent -> d_reclen) {
      dent = (dirent*)(dbuf + doff);
      ++num_entries;

      // The filename should just be a decimal number = the fd it represents
      // Also, skip the entry for the checkpoint and directory files as we don't want the restore to know about them
      fdnum = strtol(dent -> d_name, &p, 10);
      if ( *p == 0 && fdnum > 2 && fdnum != fd && fdnum != fddir ) {
        // Read the symbolic link so we get the filename that's open on the fd
        sprintf(procfdname, "%s/%d", proc_fd, fdnum);
        linklen = readlink(procfdname, linkbuf, sizeof linkbuf - 1);
        if ((linklen >= 0) || (errno != ENOENT)) { // probably was the proc/self/fd directory itself

	  FileDescriptor dsc;
	  dsc.fdnum = fdnum;
	  dsc.tmpbuf = 0;
	  dsc.tmpdel = 0;

          if (linklen < 0) {
            mtcp_output(MTCP_ERROR,"mtcp saveFileDescriptors: error reading %s: %s\n", procfdname, strerror (errno));
	    return 0;
          }
          linkbuf[linklen] = 0;

          // Read about the link itself so we know read/write open flags
          rc = lstat (procfdname, &lstatbuf);
          if (rc < 0) {
            mtcp_output(MTCP_ERROR,"mtcp saveFileDescriptors: error lstat *%s [%d] -> %s: %s\n",
			 procfdname, fdnum, linkbuf, strerror (-rc));
	    return 0;
          }

	  ::strcpy(dsc.linkbuf, linkbuf);
          // Read about the actual file open on the fd
          rc = stat (linkbuf, &statbuf);
	  bool reread = false;

	  // This does not take into account regulare open files, which are written!
          if (rc < 0) {
            mtcp_output(MTCP_WARNING,"mtcp saveFileDescriptors: error stat %s [%d] -> *%s: %s\n",
			 procfdname, fdnum, linkbuf, strerror (-rc));
	    rc = fstat(fdnum,&statbuf);
	    if ( rc == 0 ) {
	      mtcp_output(MTCP_DEBUG,
			  "mtcp saveFileDescriptors: fstat %s [%d] -> *%s: Dir:%s Reg:%s Sock:%s Fifo:%s Block:%s\n",
			   procfdname, fdnum, linkbuf, 
			   S_ISDIR(statbuf.st_mode)  ? "YES" : "NO", 
			   S_ISREG(statbuf.st_mode)  ? "YES" : "NO", 
			   S_ISSOCK(statbuf.st_mode) ? "YES" : "NO", 
			   S_ISFIFO(statbuf.st_mode) ? "YES" : "NO",
			   S_ISBLK(statbuf.st_mode)  ? "YES" : "NO"
			   );
	      if ( S_ISREG(statbuf.st_mode) ) {
		off_t offs = mtcp_sys_lseek(fdnum, 0, SEEK_CUR);
		if ( 0 != fd ) {
		  rc = mtcp_sys_lseek(fdnum, 0, SEEK_SET);
		  void* buff = malloc(statbuf.st_size);
		  rc = ::pread(fdnum,buff,statbuf.st_size,0);
		  if ( rc == statbuf.st_size )  {
		    dsc.tmpbuf = buff;
		    rc = 0;
		  }
		}
		else {
		  dsc.tmpbuf = 0;
		  reread = true;
		  rc = 0;
		}
		dsc.tmpdel = ::strstr(linkbuf,"(deleted)") != 0;
		mtcp_output(MTCP_DEBUG,"mtcp saveFileDescriptors: %s: %d bytes at %X RD:%d.\n",
			    procfdname, statbuf.st_size, offs, rc);
	      } 
	      else {
		rc = -1;
	      }
	    }
          }
          if ( rc == 0 )  {
	    // Write state information to checkpoint file
	    // Replace file's permissions with current access flags so restore will know how to open it
            offset = 0;
            if (S_ISREG (statbuf.st_mode)) offset = mtcp_sys_lseek (fdnum, 0, SEEK_CUR);
            statbuf.st_mode = (statbuf.st_mode & ~0777) | (lstatbuf.st_mode & 0777);
	    
            mtcp_output(MTCP_DEBUG,"mtcp saveFileDescriptors: %s -> %s\n", procfdname, linkbuf);

	    dsc.statbuf = statbuf;
	    dsc.offset  = offset;
	    if ( fd == 0 ) {
	      descrs.push_back(dsc);
	    }
	    else {
	      int has_data = reread ? 1 : 0;
	      ::write(fd,"<File>",6);
	      ::write(fd,&dsc,sizeof(dsc));
	      ::write(fd,&has_data,sizeof(int));
	      if ( reread ) {
		char c;
		mtcp_sys_lseek(fdnum, 0, SEEK_SET);
		for(int i=0; i<statbuf.st_size; ++i) {
		  ::pread(fdnum,&c,1,i);
		  ::write(fd,&c,1);
		}
	      }
	      ::write(fd,"</File>",7);
	    }
            //writefile (fd, &fdnum, sizeof fdnum);
            //writefile (fd, &statbuf, sizeof statbuf);
            //writefile (fd, &offset,  sizeof offset);
            //writefile (fd, &linklen, sizeof linklen);
            //writefile (fd, linkbuf,  linklen);
          }
        }
      }
    }
  }
  if (dsiz < 0) {
    mtcp_output(MTCP_ERROR,"mtcp saveFileDescriptors: error reading /proc/self/fd: %s\n",
		strerror (mtcp_sys_errno));
    return 0;
  }
  mtcp_sys_close (fddir);
  mtcp_output(MTCP_INFO,"mtcp saveFileDescriptors: Scanned %d entries.\n",num_entries);
  return 1;
}

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
		fdnum, linkbuf, d.tmpbuf ? "YES" : "NO", d.tmpdel ? "(deleted)" : "");

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
    if ( d.tmpdel ) ::remove(linkbuf);
  }
  return 1;
}
