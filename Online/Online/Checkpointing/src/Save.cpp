#include "Checkpointing/Static.h"
#include "Checkpointing/SysCalls.h"
#include "Checkpointing/MemMaps.h"
#include "Checkpointing/FileMap.h"
#include "Checkpointing/Thread.h"
#include "Checkpointing.h"
#include "Restore.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

using namespace Checkpointing;

#ifndef __STATIC__
DefineMarker(PROCESS_BEGIN_MARKER,"PROC");
DefineMarker(PROCESS_END_MARKER,  "proc");
DefineMarker(MEMMAP_BEGIN_MARKER, "MMAP");
DefineMarker(MEMMAP_END_MARKER,   "mmap");
DefineMarker(FILE_BEGIN_MARKER,   "FILE");
DefineMarker(FILE_END_MARKER,     "file");
DefineMarker(LIBS_BEGIN_MARKER,   "LIBS");
DefineMarker(LIBS_END_MARKER,     "libs");
#endif
DefineMarker(LIBRARY_BEGIN_MARKER,"XLIB");
DefineMarker(LIBRARY_END_MARKER,  "xlib");

/// Write single library content to file identified by fileno fd_out
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_library_fwrite(int fd, const Area* a)    {
  if ( fd <= 0 ) return 0;
  else if ( m_strcmp(a->name,chkpt_sys.checkpointFile)  == 0 ) return 0;
  else if ( m_strcmp(a->name,chkpt_sys.checkpointImage) == 0 ) return 0;
  else if ( a->name[0] && a->name[0]=='/' ) {
    int    bytes = 0;
    bytes += writeMarker(fd,LIBRARY_BEGIN_MARKER);
    int lib_fd = mtcp_sys_open(a->name,O_RDONLY,0);
    if ( lib_fd > 0 ) {
      struct stat lib_stat;
      const char *p0, *nam;
      for(p0=a->name, nam=a->name; *p0; ++p0) if (*p0=='/') nam=p0;
      nam = nam + 1;
      if ( 0 == mtcp_sys_fstat(lib_fd,&lib_stat) ) {
	size_t nam_len = m_strlen(nam);
	bytes += writeInt(fd,nam_len+1);
	bytes += m_writemem(fd,nam,nam_len+1);
	bytes += writeInt(fd,lib_stat.st_size);
	bytes += m_fcopy(fd, lib_fd, lib_stat.st_size);
	bytes += writeMarker(fd,LIBRARY_END_MARKER);
	mtcp_sys_close(lib_fd);
	mtcp_output(MTCP_INFO,"checkpoint_lib_fwrite: Image: %s Wrote %d bytes\n",nam,bytes);
	return bytes;
      }
      else {
	mtcp_sys_close(lib_fd);
	writeInt(fd,2*sizeof(Marker));
	mtcp_output(MTCP_ERROR,"checkpoint_lib_fwrite: Image:%s failed to stat:%d [%s]\n",
		    a->name, mtcp_sys_errno, ::strerror(mtcp_sys_errno));
      }
    }
    else {
      mtcp_output(MTCP_ERROR,"checkpoint_lib_fwrite: Image:%s failed to open file:%d [%s]\n",
		  a->name, mtcp_sys_errno, ::strerror(mtcp_sys_errno));
    }
    return -1;
  }
  return 0;
}

/// Write all mapped libraries to file identified by fileno fd_out
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_libs_fwrite(int fd) {
  if ( fd > 0 ) {
    int  bytes  = writeMarker(fd,LIBS_BEGIN_MARKER);
    long off, offset = ::lseek(fd,0,SEEK_CUR);
    bytes += writeLong(fd,0);
    bytes += writeInt(fd,0);
    if ( chkpt_sys.save_flags&MTCP_SAVE_LIBS )   {
      AreaLibHandler h(fd);
      if ( 1 == checkpointing_memory_scan(&h) ) {
	bytes += h.bytes();
	bytes += writeMarker(fd,LIBS_END_MARKER);
	off = mtcp_sys_lseek(fd,0,SEEK_CUR);
	mtcp_sys_lseek(fd,offset,SEEK_SET);
	writeLong(fd,h.bytes());   // Update counter
	writeInt(fd,h.numLibs());  // Update counter
	mtcp_sys_lseek(fd,off,SEEK_SET);
	mtcp_output(MTCP_INFO,"Saving %d image files to checkpoint!\n",h.numLibs());
	h.release();
	return bytes;
      }
      h.release();
      bytes += writeMarker(fd,LIBS_END_MARKER);
      mtcp_output(MTCP_ERROR,"Failed to scan memory sections!\n");
      return bytes;
    }
    else {
      mtcp_output(MTCP_INFO,"Saving image files not requested!\n");
      bytes += writeMarker(fd,LIBS_END_MARKER);
      return bytes;
    }
  }
  return 0;
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

/// Write full process information to checkpoint file
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_fwrite(int fd)   {
  long rc = 0, tot = 0, offset = 0;
  if ( fd != 0 ) {
    checkpointing_sys_aquire(&chkpt_sys);
    checkpointing_sys_print(&chkpt_sys);
    if ( (rc=checkpointing_process_write_header(fd)) ) {
      tot += rc;
      mtcp_output(MTCP_INFO,"checkpoint: Wrote %ld [Total:%ld] bytes of process header.\n",rc,tot);
      if ( (rc=checkpointing_libs_fwrite(fd)) ) {
	tot += rc;
	mtcp_output(MTCP_INFO,"checkpoint: Wrote %ld [Total:%ld] bytes of library data.\n",rc,tot);
      }
      else   {
	mtcp_output(MTCP_FATAL,"checkpoint: Failed to write library data.\n");
      }
      offset = mtcp_sys_lseek(fd,0,SEEK_CUR);
      if ( (rc=checkpointing_sys_fwrite(fd,&chkpt_sys)) ) {
	tot += rc;
	mtcp_output(MTCP_INFO,"checkpoint: Wrote %ld [Total:%ld] bytes of System information.\n",rc,tot);
	if ( (rc=checkpointing_files_write(fd)) ) {
	  tot += rc;
	  mtcp_output(MTCP_INFO,"checkpoint: Wrote %ld [Total:%ld] bytes of file information.\n",rc,tot);
	  if ( (rc=checkpointing_memory_write(fd)) ) {
	    if ( chkpt_sys.restart_type == SysInfo::RESTART_CHECKPOINT ) {
	      mtcp_output(MTCP_INFO,"checkpoint: Restarting HURRA. Restart_type=%d [Should be:%d]\n",
			  chkpt_sys.restart_type, SysInfo::RESTART_CHECKPOINT);
	      return 1;
	    }
	    tot += rc;
	    mtcp_output(MTCP_INFO,"checkpoint: Wrote %ld [Total:%ld] bytes of process memory.\n",rc,tot);
	    if ( (rc=checkpointing_process_write_trailer(fd)) ) {
	      tot += rc;
	      mtcp_output(MTCP_INFO,"checkpoint: Wrote %ld [Total:%ld] bytes of process trailer.\n",rc,tot);
	      mtcp_sys_lseek(fd,offset,SEEK_SET);
	      mtcp_output(MTCP_INFO,"checkpoint: Restarting -- BAD Location. Restart_type=%d [Should be:%d]\n",
			  chkpt_sys.restart_type, SysInfo::RESTART_CHECKPOINT);
	      checkpointing_sys_fwrite(fd,&chkpt_sys);
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
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_write_header(int fd)   {
  return writeMarker(fd,PROCESS_BEGIN_MARKER);
}

/// Write trailer information to checkpoint file
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_write_trailer(int fd)   {
  return writeMarker(fd,PROCESS_END_MARKER);
}

/// Write the file descriptor information to checkpoint file
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_files_write(int fd)   {
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

/// Collect in process information about the memory mappings
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_memory_scan(AreaHandler* handler) {
  int fd = mtcp_sys_open("/proc/self/maps",O_RDONLY,0);
  if ( fd>0 ) {
    int count = 0;
    char text[256+PATH_MAX] = "  ", *p=text, *low, *high, *prot, *inode, *dev, *off, *fn;
    const char *file;
    for(int n=1; n>0 && (p-text)<int(sizeof(text)); ++p)   {
      n = mtcp_sys_read(fd,p,1);
      if ( n == 1 && *p != '\n' ) continue;
      *p = 0;
      if ( n == 1 ) {
	low  = text;
	high = m_chrfind(low,'-');
	*high++  = 0;
	prot     = m_chrfind(high,' ');
	*prot++  = 0;
	off      = m_chrfind(prot,' ');
	*off++   = 0;
	dev      = m_chrfind(off,' ');
	*dev++   = 0;
	inode    = m_chrfind(dev,' ');
	*inode++ = 0;
	fn       = m_chrfind(inode,' ');
	*fn++    = 0;
	file     = m_chrfind(fn,'/');
	if ( !file ) file = m_chrfind(fn,'[');
	if ( !file ) file = "";
	Area a;
	::sscanf(low,"%lx",&a.low);
	::sscanf(high,"%lx",&a.high);
	::sscanf(off,"%lx",&a.offset);
	a.size = a.high - a.low;
	*(int*)a.prot = *(int*)prot;
	if ( !file[0] ) {
	  unsigned long brk = (unsigned long)mtcp_sys_brk(0);
	  if ( brk >= a.low && brk <= a.high ) file = "[heap]";
	}
	a.name_len = m_strcpy(a.name,file);
	long sc = (*handler->f_handle)(handler,count,a);
	if ( sc > 0 ) count++;
      }
      if ( n == 0 ) break;
      p = text;
      *p = ' ';
    }
    text[sizeof(text)-1]=0;
    mtcp_sys_close(fd);
    return 1;
  }
  return 0;
}

/// Write the memory areas to checkpoint file
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_memory_write(int fd)   {
  if ( fd > 0 ) {
    int  bytes  = writeMarker(fd,MEMMAP_BEGIN_MARKER);
    long off, offset = ::lseek(fd,0,SEEK_CUR);
    bytes += writeInt(fd,0);
    AreaWriteHandler h(fd);
    if ( 1 == checkpointing_memory_scan(&h) ) {
      bytes += h.bytesWritten();
      bytes += writeMarker(fd,MEMMAP_END_MARKER);
      off = ::lseek(fd,0,SEEK_CUR);
      ::lseek(fd,offset,SEEK_SET);
      writeInt(fd,h.count()); // Update counter
      ::lseek(fd,off,SEEK_SET);
      return bytes;
    }
    mtcp_output(MTCP_ERROR,"Failed to scan memory sections!\n");
    return -1;
  }
  return -1;
}
