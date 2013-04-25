#include "Checkpointing/Static.h"
#include "Checkpointing/SysCalls.h"
#include "Checkpointing/FileMap.h"
#include "Checkpointing/MemMaps.h"
#include "Checkpointing/Process.h"
#include "Checkpointing/Thread.h"
#include "Checkpointing.h"
#include "Restore.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

using namespace Checkpointing;

DefineMarker(FILE_BEGIN_MARKER,   "FILE");
DefineMarker(FILE_END_MARKER,     "file");
DefineMarker(PROCESS_BEGIN_MARKER,"PROC");
DefineMarker(PROCESS_END_MARKER,  "proc");

DefineMarker(FILEMAP_BEGIN_MARKER,"FMAP");
DefineMarker(FILEMAP_END_MARKER,  "fmap");
DefineMarker(MEMAREA_BEGIN_MARKER,"AREA");
DefineMarker(MEMAREA_END_MARKER,  "area");
DefineMarker(MEMMAP_BEGIN_MARKER, "MMAP");
DefineMarker(MEMMAP_END_MARKER,   "mmap");

DefineMarker(LIBS_BEGIN_MARKER,   "LIBS");
DefineMarker(LIBS_END_MARKER,     "libs");
// These 2 MUST be defined in Save.cpp!
//DefineMarker(LIBRARY_BEGIN_MARKER,"XLIB");
//DefineMarker(LIBRARY_END_MARKER,  "xlib");

/// Print content to stack saved in context
STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_print_stack(const char* comment) {
  int* ptr = (int*)chkpt_sys.savedContext->SAVEDSP;
  mtcp_output(MTCP_ERROR,"Saved stack pointer   %p\n",(void*)chkpt_sys.savedContext->SAVEDSP);
  mtcp_output(MTCP_ERROR,"Saved base  pointer   %p\n",(void*)chkpt_sys.savedContext->SAVEDBP);
  for(size_t i=0; i<32; ++i)   {
    mtcp_output(MTCP_ERROR,"Stack(%s): %p -> %X  %X %X %X\n",comment,ptr,ptr[0],ptr[1],ptr[2],ptr[3]);
    ptr += 4;
  }
}

/// Print file descriptor information
STATIC(void) CHECKPOINTING_NAMESPACE::checkpoint_file_print(int lvl, const FileDesc* d) {
  mtcp_output(lvl,
	      "FileHandler: /proc/self/fd/%d -> %s\n\t\tSiz:%d Dir:%s Reg:%s "
	      "Sock:%s Fifo:%s Block:%s Tmp:%s Del:%s\n",
	      d->fd, d->name, d->statbuf.st_size,
	      S_ISDIR(d->statbuf.st_mode)  ? "YES" : "NO", 
	      S_ISREG(d->statbuf.st_mode)  ? "YES" : "NO", 
	      S_ISSOCK(d->statbuf.st_mode) ? "YES" : "NO", 
	      S_ISFIFO(d->statbuf.st_mode) ? "YES" : "NO",
	      S_ISBLK(d->statbuf.st_mode)  ? "YES" : "NO",
	      d->istmp ? "YES" : "NO",
	      d->isdel ? "YES" : "NO"
	      );
}

/// Reopen file descriptor for process restoration
STATIC(int) CHECKPOINTING_NAMESPACE::checkpoint_file_reopen(FileDesc* d) {
  // Open the file on a temp fd
  int flags = O_RDWR, mode = 0;
  if (!(d->statbuf.st_mode & S_IWUSR)) flags = O_RDONLY;
  else if (!(d->statbuf.st_mode & S_IRUSR)) flags = O_WRONLY;
  if ( d->hasData ) {
    if ( d->istmp ) { // Generate artificial temporary name
      char* p = m_chrfind(d->name,' ');
      if ( !p ) p = d->name + d->name_len;
      if ( p ) {
	pid_t pid = mtcp_sys_getpid();
	*p++ = '_';
	if ( pid>10000 ) *p++ =  (pid/10000)+'0';
	if ( pid>1000  ) *p++ = ((pid%10000)/1000)+'0';
	if ( pid>100   ) *p++ = ((pid%1000)/100)+'0';
	if ( pid>10    ) *p++ = ((pid%100)/10)+'0';
	*p++ = (pid%10)+'0';
	*p++ = 0;
      }
      //::tmpnam(d->name);
    }
    flags |= O_RDWR | O_CREAT;
    mode = 0777;
  }
  int fdnum = mtcp_sys_open(d->name, flags, mode);
  if (fdnum < 0)    {
    mtcp_output(MTCP_ERROR,"FileDesc: error %d opening %s flags %o Skip file\n", 
		mtcp_sys_errno, d->name, flags);
    if (mtcp_sys_errno == EACCES) mtcp_output(MTCP_ERROR,"  Permission denied.\n");
    return 0;
  }
  // Move it to the original fd if it didn't coincidentally open there
  if (d->fd != fdnum) {
    if (mtcp_sys_dup2(fdnum,d->fd) < 0) {
      mtcp_output(MTCP_ERROR,"FileDesc: error %d [%s] restore %s fd %d to %d SKIP file.\n", 
		  mtcp_sys_errno, strerror(mtcp_sys_errno), d->name, fdnum, d->fd);
      mtcp_sys_close (fdnum);
      return 0;
    }
    mtcp_sys_close(fdnum);
  }
  return 1;
}

/// Read file descriptor information from memory block
STATIC(int) CHECKPOINTING_NAMESPACE::checkpoint_file_read(FileDesc* d, const void* addr, bool restore) {
  unsigned char* in = (unsigned char*)addr;
  if ( in ) {
    int  is_sock = 0, is_pipe = 0;
    long len = sizeof(FileDesc)-sizeof(d->name);
    in += checkMarker(in,FILE_BEGIN_MARKER);
    in += m_memcpy(d,in,len);
    in += m_memcpy(d->name,in,d->name_len+1);
    is_sock = S_ISSOCK(d->statbuf.st_mode);
    is_pipe = m_strncmp(d->name,"pipe:[",6)==0;
    restore = !is_sock && !is_pipe;
    if ( d->fd == chkpt_sys.checkpointFD ) restore = false;
    restore = restore ? (checkpoint_file_reopen(d) == 1) : restore;
    if ( d->hasData ) {
      if ( restore ) {
	::write(d->fd,in,d->statbuf.st_size);
      }
      in += d->statbuf.st_size;
    }
    if ( restore ) {
      // Position the file to its same spot it was at when checkpointed
      if (S_ISREG(d->statbuf.st_mode) && (mtcp_sys_lseek (d->fd, d->offset, SEEK_SET) != d->offset)) {
	mtcp_output(MTCP_ERROR,"FileDesc: error %d [%s] positioning %s to %ld\n", 
		    mtcp_sys_errno, strerror(mtcp_sys_errno), d->name, (long)d->offset);
	return -1;
      }
      if ( d->isdel ) mtcp_sys_unlink(d->name);
    }
    in += checkMarker(in,FILE_END_MARKER);
    return addr_diff(in,addr);
  }
  return -1;
}

/// Read file descriptor information from file
STATIC(int) CHECKPOINTING_NAMESPACE::checkpoint_file_fread(FileDesc* d, int fd, bool restore) {
  int is_pipe=0, is_sock=0, in = 0;
  if ( fd > 0 ) {
    long len = sizeof(FileDesc)-sizeof(d->name);
    in += readMarker(fd,FILE_BEGIN_MARKER);
    in += m_fread(fd,d,len);
    in += m_fread(fd,d->name,d->name_len+1);
    is_sock = S_ISSOCK(d->statbuf.st_mode);
    is_pipe = m_strncmp(d->name,"pipe:[",6)==0;
    //Debug: checkpoint_file_print(MTCP_WARNING,this);
    restore = !is_sock && !is_pipe;
    if ( d->fd == chkpt_sys.checkpointFD ) restore = false;
    restore = restore ? (checkpoint_file_reopen(d) == 1) : restore;
    if ( d->hasData ) {
      if ( restore ) {
	m_fcopy(d->fd,fd,d->statbuf.st_size);
      }
      in += d->statbuf.st_size;
    }
    if ( restore )   {
      // Position the file to its same spot it was at when checkpointed
      if (S_ISREG(d->statbuf.st_mode) && (mtcp_sys_lseek(d->fd, d->offset, SEEK_SET) != d->offset)) {
	mtcp_output(MTCP_ERROR,"FileDesc: error %d [%s] positioning %s to %ld\n", 
		    mtcp_sys_errno, strerror(mtcp_sys_errno), d->name, (long)d->offset);
	return -1;
      }
      if ( d->isdel ) mtcp_sys_unlink(d->name);
    }
    in += readMarker(fd,FILE_END_MARKER);
    return in;
  }
  return -1;
}

/// Read process descriptor header from memory
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_read_header(Process*, const void* addr) {
  const_Pointer ptr = (const_Pointer)addr;
  if ( ptr ) {
    ptr += checkMarker(ptr,PROCESS_BEGIN_MARKER);
  }
  return addr_diff(ptr,addr);
}

/// Read process descriptor trailer from memory
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_read_trailer(Process*, const void* addr) {
  size_t sz = checkMarker(addr,PROCESS_END_MARKER);
  return sz;
}

/// Read process descriptor header from file
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_fread_header(Process*,int fd) {
  int sz = readMarker(fd,PROCESS_BEGIN_MARKER);
  return sz;
}


/// Read process descriptor trailer from file
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_fread_trailer(Process*,int fd) {
  int sz = readMarker(fd,PROCESS_END_MARKER);
  return sz;
}

/// Read full file descriptor information from memory
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_read_files(Process*,const void* addr)  {
  const_Pointer in = (const_Pointer)addr;
  if ( in ) {
    //#ifdef __STATIC__
    FileDesc dsc;
    in  += checkMarker(in,FILEMAP_BEGIN_MARKER);
    int count = *(int*)in;
    in += sizeof(int);
    mtcp_output(MTCP_INFO,"restore: restoring %d pending file descriptors\n",count);
    long cnt=0;
    do {
      in += checkpoint_file_read(&dsc,in,true);
      mtcp_output(MTCP_DEBUG,"restore[addr:%p]: ",in);
      checkpoint_file_print(MTCP_DEBUG|MTCP_NO_HEADER,&dsc);
      ++cnt;
    } while (cnt<count);
    in += checkMarker(in,FILEMAP_END_MARKER);
    return addr_diff(in,addr);
    //#endif
  }
  mtcp_output(MTCP_ERROR,"Failed to read file information!\n");
  return 0;
}

STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_map_memory(const AreaHandler* ,const Area& a, const unsigned char* data, int data_len) {
  return checkpointing_area_map(a,0,data,data_len);
}

STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_read_memory(Process*, const void* addr) {
  mtcp_output(MTCP_INFO,"checkpointing_process_read_memory\n");
  MemMaps m;
  const_Pointer in = (const_Pointer)addr;
  if ( in ) {
    int numArea = 0;
    in += checkMarker(in,MEMMAP_BEGIN_MARKER);
    in += getInt(in,&numArea);
    Area a;
    mtcp_output(MTCP_INFO,"restore: Need to restore %d memory areas\n",numArea);
    for(int i=0; i<numArea; ++i) {
      int rc = checkpointing_area_read(&a,in,checkpointing_process_map_memory,(const AreaHandler*)0);
      if ( rc<0 ) {
	mtcp_output(MTCP_FATAL,"restore: Failed to restored area [%d] %s\n",i,a.name);
      }
      in += rc;
    }
    in += checkMarker(in,MEMMAP_END_MARKER);
    mtcp_output(MTCP_INFO,"Read %ld bytes of mmap area data for %d areas\n",
		addr_diff(in,addr),numArea);
    return addr_diff(in,addr);
  }
  mtcp_output(MTCP_ERROR,"Failed to read memory dump!\n");
  return 0;
}

STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_read(Process* p, const void* addr) {
  long rc = 0;
  if ( addr != 0 ) {
    const_Pointer in = (const_Pointer)addr;
    mtcp_output(MTCP_INFO,"Start reading at address:%p\n",in);
    if ( (rc=checkpointing_process_read_header(p,in)) ) {
      mtcp_output(MTCP_INFO,"Read %ld bytes of process header.\n",rc);
      if ( (rc=checkpointing_process_skip_sys(p, in+=rc)) ) {
	mtcp_output(MTCP_INFO,"Read %ld bytes of process sysinfo.\n",rc);
	if ( (rc=checkpointing_process_read_files(p,in+=rc)) ) {
	  mtcp_output(MTCP_INFO,"Read %ld bytes of process file info.\n",rc);
	  if ( (rc=checkpointing_process_read_memory(p,in+=rc)) ) {
	    mtcp_output(MTCP_INFO,"Read %ld bytes of process memory.\n",rc);
	    if ( (rc=checkpointing_process_read_trailer(p,in+=rc)) ) {
	      mtcp_output(MTCP_INFO,"Read %ld bytes of process trailer.\n",rc);
	      in+= rc;
	      checkpointing_sys_restore_finish();
	      return addr_diff(in,addr);
	    }
	  }
	}
      }
    }
  }
  return 0;
}

/// Read full file descriptor information from file
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_fread_files(Process*,int fd)  {
  int in = 0;
  if ( fd>0 ) {
#ifdef __STATIC__
    int count = 0;
    FileDesc dsc;
    in += readMarker(fd,FILEMAP_BEGIN_MARKER);
    in += readInt(fd,&count);
    mtcp_output(MTCP_INFO,"restore[fd:%d]: restoring %d pending file descriptors.\n",fd,count);
    long cnt=0;
    do {
      in += checkpoint_file_fread(&dsc,fd,true);
      mtcp_output(MTCP_DEBUG,"restore[fd:%d]: ",fd);
      checkpoint_file_print(MTCP_DEBUG|MTCP_NO_HEADER,&dsc);
      ++cnt;
    } while (cnt<count);
    in += readMarker(fd,FILEMAP_END_MARKER);
    return in;
#endif
  }
  mtcp_output(MTCP_ERROR,"Failed to read file information!\n");
  return in;
}

STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_fmap_memory(const AreaHandler* ,const Area& a, int fd, int data_len) {
  return checkpointing_area_map(a,fd,0,data_len);
}

STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_fread_memory(Process*,int fd) {
  MemMaps m;
  if ( fd>0 ) {
    int in = 0;
    int numArea = 0;
    in += readMarker(fd,MEMMAP_BEGIN_MARKER);
    in += readInt(fd,&numArea);
    Area a;
    mtcp_output(MTCP_INFO,"fd:%d, restore: Need to restore %d memory areas\n",fd,numArea);
    for(int i=0; i<numArea; ++i) {
      int rc = checkpointing_area_fread(&a,fd,checkpointing_process_fmap_memory,(const AreaHandler*)0);
      if ( rc<0 ) {
	mtcp_output(MTCP_FATAL,"restore: Failed to restored area [%d] %s\n",i,a.name);
      }
      in += rc;
    }
    in += readMarker(fd,MEMMAP_END_MARKER);
    mtcp_output(MTCP_INFO,"fd:%d, restore: Read %ld bytes of mmap area data for %d areas\n",fd,in,numArea);
    return in;
  }
  mtcp_output(MTCP_ERROR,"fd:%d, restore: Failed to read memory dump!\n",fd);
  return 0;
}

/// Skip library section. We got it already....
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_fskip_libs(Process*, int fd) {
  int num_libs;
  long offset, len = 0;
  int in = readMarker(fd,LIBS_BEGIN_MARKER);
  in += readLong(fd,&len);
  in += readInt(fd,&num_libs);
  if ( len > 0 ) {
    offset = mtcp_sys_lseek(fd,0,SEEK_CUR);
    mtcp_sys_lseek(fd,offset+len,SEEK_SET);
  }
  in += readMarker(fd,LIBS_END_MARKER);
  return in;
}

/// Read entire process information from checkpoint file. Skip irrelevant enteties
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_fread(Process* p, int fd) {
  long rc = 0;
  if ( fd > 0 ) {
    mtcp_output(MTCP_DEBUG,"fd:%d, Start reading at position:%p\n",fd,rc);
    if ( (rc=checkpointing_process_fread_header(p,fd)) ) {
      mtcp_output(MTCP_INFO,"fd:%d, Read %ld bytes of process header.\n",fd,rc);

      /// Skip libraries
      if ( (rc=checkpointing_process_fskip_libs(p,fd)) ) {
	mtcp_output(MTCP_INFO,"fd:%d, Read %ld bytes of process sysinfo.\n",fd,rc);
      }
      else {
	mtcp_output(MTCP_FATAL,"fd:%d Failed to skip library section.\n",fd,rc);
	return 0;
      }
      /// Skip SysInfo (we got it already...)
      if ( (rc=checkpointing_process_fskip_sys(p,fd)) ) {
	mtcp_output(MTCP_INFO,"fd:%d, Read %ld bytes of process sysinfo.\n",fd,rc);
      }
      else {
	mtcp_output(MTCP_FATAL,"fd:%d Failed to skip SysInfo section.\n",fd,rc);
	return 0;
      }
      /// Read and save file sections
      if ( (rc=checkpointing_process_fread_files(p,fd)) ) {
	mtcp_output(MTCP_INFO,"fd:%d, Read %ld bytes of process file info.\n",fd,rc);
      }
      else {
	mtcp_output(MTCP_FATAL,"fd:%d Failed to process file sections.\n",fd,rc);
	return 0;
      }
      /// Read and remap memory sections
      if ( (rc=checkpointing_process_fread_memory(p,fd)) ) {
	mtcp_output(MTCP_INFO,"fd:%d, Read %ld bytes of process memory.\n",fd,rc);
      }
      else {
	mtcp_output(MTCP_FATAL,"fd:%d Failed to process memory sections.\n",fd,rc);
	return 0;
      }
      /// Read process trailer
      if ( (rc=checkpointing_process_fread_trailer(p,fd)) ) {
	mtcp_output(MTCP_INFO,"fd:%d, Read %ld bytes of process trailer.\n",fd,rc);
	//checkpointing_sys_restore_finish();
	return 1;
      }
    }
  }
  return 0;
}

STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_area_datalength(const Area* a) {
  if ( *(int*)a->name == *(int*)"[stack]" ) {
    // Only write NULL-size marker
    return a->size;
  }
  else if ( a->prot[0]=='r' && (a->name_len == 0 || a->name[0]=='[') ) {
    return a->size;
  }
  else if ( a->prot[0]=='r' && a->prot[1]=='w' ) {
    return a->size;
  }
  else if ( 0 == m_strcmp(a->name,chkpt_sys.checkpointImage) ) {
    return a->size;
  }
  else if ( a->prot[0]=='r' && a->name[0]=='/' ) {
    return a->size;
  }
  return 0;
}

STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_area_print(const Area* a,int lvl,const char* opt) {
  int len = checkpointing_area_datalength(a);
  mtcp_output(lvl,"%s%p-%p [%8d,%8d] %c%c%c%c o:%p %s\n",
	      opt,a->low,a->high,a->size,len,
	      a->prot[0],a->prot[1],a->prot[2],a->prot[3],a->offset,a->name);
}

STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_area_read(Area* a, const void* addr, int (*map)(const AreaHandler*, const Area& a,const unsigned char*,int), const AreaHandler* handler) 
{
  const unsigned char* in = (const unsigned char*)addr;
  int l = 0;
  int len = sizeof(Area)-sizeof(a->name);
  in += checkMarker(in,MEMAREA_BEGIN_MARKER);
  in += m_memcpy(a,in,len);
  in += m_memcpy(a->name,in,a->name_len+1);
  in += m_memcpy(&l,in,sizeof(l));
  in += map ? (*map)(handler,*a,in,l) : l;
  in += checkMarker(in,MEMAREA_END_MARKER);
  checkpointing_area_print(a,MTCP_DEBUG,"Read memory area:");
  return addr_diff(in,addr);  
}

STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_area_fread(Area* a,int fd, int (*map)(const AreaHandler*, const Area& a,int fd,int), const AreaHandler* handler) {
  int in = 0;
  int l = 0;
  int len = sizeof(Area)-sizeof(a->name);
  in += readMarker(fd,MEMAREA_BEGIN_MARKER);
  in += m_fread(fd,a,len);
  in += m_fread(fd,a->name,a->name_len+1);
  in += readInt(fd,&l);
  if ( a->name[0] && a->name[0] == '/' && 
       chkpt_sys.save_flags&MTCP_SAVE_LIBS &&
       chkpt_sys.checkpointLibs[0] ) {
    char tmp[PATH_MAX];
    const char *p0, *p1;
    m_strcpy(tmp,chkpt_sys.checkpointLibs);
    for(p0=a->name, p1=a->name; *p0; ++p0) if (*p0=='/') p1=p0;
    m_strcpy(tmp+m_strlen(tmp),p1);
    m_strcpy(a->name,tmp);
    a->name_len = m_strlen(a->name);
  }
  in += map ? (*map)(handler,*a,fd,l) : l;
  in += readMarker(fd,MEMAREA_END_MARKER);
  checkpointing_area_print(a,MTCP_DEBUG,"Read memory area:");
  return in;
}

/// Access protection flags for this memory area
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_area_protection(const Area* a) {
  int p = 0;
  if ( a->prot[0]=='r' ) p |= PROT_READ;
  if ( a->prot[1]=='w' ) p |= PROT_WRITE;
  if ( a->prot[2]=='x' ) p |= PROT_EXEC;
  return p;
}

/// Access mmap flags for this memory area
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_area_mapFlags(const Area* a) {
  int f = MAP_FIXED;
  if ( a->prot[3]  =='s' ) f |= MAP_SHARED;
  if ( a->prot[3]  =='p' ) f |= MAP_PRIVATE;
  if ( a->name_len == 0  ) f |= MAP_ANONYMOUS;
  if ((f& MAP_PRIVATE)) f |= MAP_ANONYMOUS;
  return f;
}

STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_area_map(const Area& a,int fd_in,const unsigned char* in,int data_len) {
  //mtcp_output(MTCP_DEBUG,"*** Mapping area \"%s\"  %p->%p.\n",a.name,a.low,a.high);
  int flags = checkpointing_area_mapFlags(&a);
  int prot  = checkpointing_area_protection(&a);
  const char* nam = a.name;
  void* addr = (void*)a.low;
  size_t size = a.size;
  off_t  offset = a.offset;

  if( *(int*)nam == *(int*)"[vdso]"  && chkpt_sys.vsyscallStart == 0 ) {
    offset = a.offset;
    checkpointing_area_print(&a,MTCP_INFO,"*** WARNING: SKIP [vdso] area:");
    if ( fd_in>0 && data_len>0 ) m_fskip(fd_in,data_len);
    return data_len;
  }
  if( *(long*)nam == *(long*)"[vsyscall]" ) {
    offset = a.offset;
    checkpointing_area_print(&a,MTCP_INFO,"*** WARNING: SKIP [vsyscall] area:");
    if ( fd_in>0 && data_len>0 ) m_fskip(fd_in,data_len);
    return data_len;
  }
  if ( a.high == chkpt_sys.addrEnd ) {
    checkpointing_area_print(&a,MTCP_INFO,"*** WARNING: SKIP IMAGE area:");
    m_fskip(fd_in,size);
    return data_len;
  }  
  if ( 0 == m_strcmp(nam,"/usr/lib/locale/locale-archive") ) {
    checkpointing_area_print(&a,MTCP_INFO,"*** WARNING: SKIP LOCALE-ARCHIVE:");
    m_fskip(fd_in,size);
    return data_len;    
  }
  if ( 0 == m_strcmp(nam,"/var/db/nscd/hosts") ) {  // new since slc6
    checkpointing_area_print(&a,MTCP_INFO,"*** WARNING: SKIP image:");
    m_fskip(fd_in,size);
    return data_len;    
  }
  if ((flags & MAP_ANONYMOUS) && (flags & MAP_SHARED)) {
    mtcp_output(MTCP_INFO,"*** WARNING: area %s specifies MAP_ANONYMOUS and MAP_SHARED.\n"
		"*** Turning off MAP_ANONYMOUS and hoping for best.\n",nam);
  }

  int  fd = 0;
  int  map_prot = prot|MAP_FIXED;
  bool copy_data = true;
  if (flags & MAP_ANONYMOUS) {
    /* CASE MAP_ANONYMOUS (usually implies MAP_PRIVATE):
     * For anonymous areas, the checkpoint file contains the memory contents 
     * directly. So mmap an anonymous area and read the file into it.
     * If file exists, turn off MAP_ANONYMOUS: standard private map
     *
     * If there is a filename there, though, pretend like we're mapping    
     * to it so a new /proc/self/maps will show a filename there like with 
     * original process.  We only need read-only access because we don't   
     * want to ever write the file.					     
     */
    if ( nam[0] == '/' ) { /* If not null string, not [stack] or [vdso] */
      fd = mtcp_sys_open(nam,O_RDONLY,0);
      if (fd < 0) fd = 0;
      else flags ^= MAP_ANONYMOUS;
    }
    // We though need write access to the memory in order to fill it.
    map_prot |= PROT_WRITE;
  }
  else {  // Case of shared memory: NOT MAP_ANONYMOUS:
    int file_flags = 0;            // see how to open it based on the access required
    if ( (prot & (PROT_READ|PROT_EXEC) ) != 0            )  file_flags = O_RDONLY;
    if ( (prot & (PROT_WRITE|PROT_READ)) == (PROT_WRITE) )  file_flags = O_WRONLY;
    if ( (prot & (PROT_WRITE|PROT_READ)) == (PROT_WRITE|PROT_READ) ) file_flags = O_RDWR;

    fd = mtcp_sys_open(nam,file_flags,0);  // open it
    if ( fd < 0 )  {    // Fatal: backing file doesn't exist:
      mtcp_output(MTCP_FATAL,"restore: error %d opening mmap file %s\n",mtcp_sys_errno, nam);
    }
    copy_data = (file_flags == O_WRONLY || file_flags == O_RDWR) && (prot & PROT_WRITE);
  }

  mtcp_output(MTCP_DEBUG,"restore %sanon.%p-%p [%d,%d] %c%c%c%c o:%p %s [%s] %s\n",
	      (flags & MAP_ANONYMOUS) ? "non-" : "    ",addr,a.high,size,data_len,
	      a.prot[0],a.prot[1],a.prot[2],a.prot[3],fd==0 ? 0 : offset,nam,
	      data_len>0 ? "Move data" : "Skip data",
	      flags & MAP_SHARED ? "Shared" : "");
  // Create the memory area. This mmap automatically unmaps old memory.
  void* data = mtcp_sys_mmap(addr,size,map_prot,flags,fd,offset);
  if ( data == MAP_FAILED || data != addr ) {
    int err = mtcp_sys_errno;
    mtcp_output(MTCP_ERROR,"restore: \"%s\"\n",nam);
    mtcp_output(MTCP_FATAL,"restore: error %d mapping 0x%X bytes offset%d at %p ->%p\n",
		err,size,offset,addr,data);
  }
  // This mmapfile after prev. mmap is okay; use same args again. Posix says prev. map will be munmapped.
  if ( data_len > 0 )   {
    if ( copy_data ) {
      (fd_in > 0) ? m_fread(fd_in,addr,size) : m_memcpy(addr,in,size),in+=size;
      if ( fd < 0 ) checkpointing_print_stack("from_file"); // ie. never!
    }
    else if ( fd_in > 0 ) {
      m_fskip(fd_in,size);
    }
  }
  if ( !(prot & PROT_WRITE) && (map_prot&PROT_WRITE) ) {
    if (mtcp_sys_mprotect (addr,size,prot) < 0) {
      int err = mtcp_sys_errno;
      mtcp_output(MTCP_FATAL,"restore: \"%s\" error %d write-protecting 0x%X bytes at %p\n",
		  nam,err,size,addr);
    }
  }
  // Close image file (fd only gets in the way) */
  if ( fd != 0 ) mtcp_sys_close(fd);
  
  void* brk = mtcp_sys_brk(0);
  if( *(int*)nam == *(int*)"[heap]" && brk != nam + size) {
    //mtcp_output(MTCP_WARNING,"WARNING: break (%p) not equal to end of heap (%p)\n",brk,nam+size);
  }
  return data_len;
}

/// Secondary restore routine. Execution starts once we jumped to the local stack.
STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_sys_restore_process() {
  mtcp_output(MTCP_INFO,"Start to restore files and memory areas from %s.\n",chkpt_sys.checkpointFile);
  if ( checkpointing_process_fread(0,chkpt_sys.checkpointFD) ) {
    mtcp_output(MTCP_INFO,"Finished to restore files and memory areas from %s.\n",chkpt_sys.checkpointFile);
    checkpointing_sys_restore_finish();
  }
  mtcp_output(MTCP_FATAL,"Failed to read process information from file %s.\n",chkpt_sys.checkpointFile);
}

/// Final restart routine. Execution starts once we are back on the stack of the restored process.
STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_sys_restore_finish() {
  mtcp_output(MTCP_INFO,"sys_restore_finish: restore complete, resuming. Jump to:%p\n",chkpt_sys.finishRestore);
  // Jump to finishrestore in original program's libmtcp.so image
  (*(chkpt_sys.finishRestore))();
}

/// Handle input file to set environment etc.
STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_sys_process_file(int fd, void* par, checkpointing_string_handler_t handler) {
  int rc = 0;
  char c, line[4096], *p=line;
  bool eq=false, com=false, ign=false, nrec=false;
  do {
    if ( (rc=mtcp_sys_read(fd,&c,1)) == 1 ) {
      if ( long(p-line) > long(sizeof(line)-1) && !(c=='\n' || c=='\0' || c==';') ) continue;
      if ( (com = c=='#' ? true : com) ) { if ( c!='\n')*p++ = c; continue; }
      switch(c) {
      case '\n':
	com = false;
      case ';':
      case '\0':
	if ( m_strncmp(line,"then",4)  == 0 && (m_isspace(line[4]) || m_isspace(c))) ign = true;
	else if ( m_strncmp(line,"fi",2) == 0 && (m_isspace(line[2]) || m_isspace(c) || c==';')) ign = true;
	if ( p > line ) {
	  const char* opt = ign ? "Ignore: " : nrec ? "Not recognized: " : com ? "Comment: " : "";
	  *p++ = 0;
	  if ( *opt == 0 )  {
	    if ( handler ) handler(par,line);
	  }
	}
	eq = ign = nrec = false;
	p  = line;
	break;
      case ' ':
	if ( p==line ) break;
	*p++ = c;
	if ( !eq )  {
	  if      ( m_strncmp(line,"if ",   3) == 0 ) ign = true;
	  else if ( m_strncmp(line,"export",6) == 0 ) ign = true;
	  else if ( m_strncmp(line,"unset", 5) == 0 ) ign = true;
	  else if ( m_strncmp(line,"LS_COLORS",9) == 0 ) com = true;
	  else if ( line[0] ) nrec = true;
	}
	break;
      case '=':
	eq = true;	// Fall through!
      default:
	*p++ = c;
	break;
      }
    }
  }
  while(rc==1);
}

