#include "Checkpointing/Static.h"
#include "Checkpointing/SysCalls.h"
#include "Checkpointing/FileMap.h"
#include "Checkpointing/MemMaps.h"
#include "Checkpointing/Process.h"
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
DefineMarker(SYS_BEGIN_MARKER,    "PSYS");
DefineMarker(SYS_END_MARKER,      "psys");


#define STACKSIZE 4096      // size of temporary stack (in quadwords)
/* temp stack used internally by restore so we don't go outside the
 *   libmtcp.so address range for anything;
 * including "+ 1" since will set %esp/%rsp to tempstack+STACKSIZE
 */
STATIC(long long int) mtcp_internal_tempstack[STACKSIZE + 1];


STATIC(int) CHECKPOINTING_NAMESPACE::checkpoint_file_reopen(FileDesc* d) {
  // Open the file on a temp fd
  int flags = O_RDWR, mode = 0;
  if (!(d->statbuf.st_mode & S_IWUSR)) flags = O_RDONLY;
  else if (!(d->statbuf.st_mode & S_IRUSR)) flags = O_WRONLY;

  if ( d->hasData ) {
    if ( d->istmp ) { // Generate artificial temporary name
      char* p = m_chrfind(d->name,' ');
      if ( p ) {
	p[0] = '_';
	p[1] = 0;
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

STATIC(int) CHECKPOINTING_NAMESPACE::checkpoint_file_read(FileDesc* d, const void* addr, bool restore) {
  unsigned char* in = (unsigned char*)addr;
  if ( in > 0 ) {
    long len = sizeof(FileDesc)-sizeof(d->name);
    in += checkMarker(in,FILE_BEGIN_MARKER);
    in += m_memcpy(d,in,len);
    in += m_memcpy(d->name,in,d->name_len+1);
    restore = !S_ISSOCK(d->statbuf.st_mode);
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

STATIC(int) CHECKPOINTING_NAMESPACE::checkpoint_file_fread(FileDesc* d, int fd, bool restore) {
  int is_sock=0, in = 0;
  if ( fd > 0 ) {
    long len = sizeof(FileDesc)-sizeof(d->name);
    in += readMarker(fd,FILE_BEGIN_MARKER);
    in += m_fread(fd,d,len);
    in += m_fread(fd,d->name,d->name_len+1);
    is_sock = S_ISSOCK(d->statbuf.st_mode);
    restore = !is_sock;
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

STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_read_header(Process*, const void* addr) {
  const_Pointer ptr = (const_Pointer)addr;
  if ( ptr ) {
    ptr += checkMarker(ptr,PROCESS_BEGIN_MARKER);
  }
  return addr_diff(ptr,addr);
}

STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_read_trailer(Process*, const void* addr) {
  size_t sz = checkMarker(addr,PROCESS_END_MARKER);
  return sz;
}

STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_fread_header(Process*,int fd) {
  int sz = readMarker(fd,PROCESS_BEGIN_MARKER);
  return sz;
}


STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_fread_trailer(Process*,int fd) {
  int sz = readMarker(fd,PROCESS_END_MARKER);
  return sz;
}

/// Read system information from checkpoint file
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_skip_sys(Process*,const void* addr)  {
  int siz;
  const_Pointer in = (const_Pointer)addr;
  in += checkMarker(in,SYS_BEGIN_MARKER);
  in += getInt(in,&siz);
  in += siz;
  in += getInt(in,&siz);
  in += siz;
  in += checkMarker(in,SYS_END_MARKER);
  return addr_diff(in,addr);
}

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
      mtcp_output(MTCP_DEBUG,"restore: file: %d -> %s offset:%d\n",dsc.fd,dsc.name,dsc.offset);
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

/// Read system information from checkpoint file
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_fskip_sys(Process*,int fd)  {
  int siz, in = 0;
  in += readMarker(fd,SYS_BEGIN_MARKER);
  in += readInt(fd,&siz);
  in += m_fskip(fd,siz);
  in += readInt(fd,&siz);
  in += m_fskip(fd,siz);
  in += readMarker(fd,SYS_END_MARKER);
  return in;
}

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
      mtcp_output(MTCP_DEBUG,"restore[fd:%d]: file: %d -> %s offset:%d\n",fd,dsc.fd,dsc.name,dsc.offset);
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

STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_process_fread(Process* p, int fd) {
  long rc = 0;
  if ( fd > 0 ) {
    mtcp_output(MTCP_DEBUG,"fd:%d, Start reading at position:%p\n",fd,rc);
    if ( (rc=checkpointing_process_fread_header(p,fd)) ) {
      mtcp_output(MTCP_INFO,"fd:%d, Read %ld bytes of process header.\n",fd,rc);
      if ( (rc=checkpointing_process_fskip_sys(p,fd)) ) {
	mtcp_output(MTCP_INFO,"fd:%d, Read %ld bytes of process sysinfo.\n",fd,rc);
	if ( (rc=checkpointing_process_fread_files(p,fd)) ) {
	  mtcp_output(MTCP_INFO,"fd:%d, Read %ld bytes of process file info.\n",fd,rc);
	  if ( (rc=checkpointing_process_fread_memory(p,fd)) ) {
	    mtcp_output(MTCP_INFO,"fd:%d, Read %ld bytes of process memory.\n",fd,rc);
	    if ( (rc=checkpointing_process_fread_trailer(p,fd)) ) {
	      mtcp_output(MTCP_INFO,"fd:%d, Read %ld bytes of process trailer.\n",fd,rc);
	      checkpointing_sys_restore_finish();
	      return 1;
	    }
	  }
	}
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

  if( *(int*)nam == *(int*)"[vdso]" ) {
    offset = a.offset;
    checkpointing_area_print(&a,MTCP_INFO,"*** WARNING: SKIP VDSO area:");
    if ( fd_in>0 && data_len>0 ) m_fskip(fd_in,data_len);
    return data_len;
  }
  if ( a.high == chkpt_sys.addrEnd ) {
    checkpointing_area_print(&a,MTCP_INFO,"*** WARNING: SKIP IMAGE area:");
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
    mtcp_output(MTCP_FATAL,"restore: error %d mapping 0x%X bytes offset%d at %p ->%p\n",mtcp_sys_errno,size,offset,addr,data);
  }
  // This mmapfile after prev. mmap is okay; use same args again. Posix says prev. map will be munmapped.
  if ( data_len > 0 )   {
    if ( copy_data ) {
      (fd_in > 0) ? m_fread(fd_in,addr,size) : m_memcpy(addr,in,size),in+=size;
    }
    else if ( fd_in > 0 ) {
      m_fskip(fd_in,size);
    }
  }
  if ( !(prot & PROT_WRITE) && (map_prot&PROT_WRITE) ) {
    if (mtcp_sys_mprotect (addr,size,prot) < 0) {
      mtcp_output(MTCP_FATAL,"restore: error %d write-protecting 0x%X bytes at %p\n",mtcp_sys_errno,size,addr);
    }
  }
  // Close image file (fd only gets in the way) */
  if ( fd != 0 ) mtcp_sys_close(fd);
  
  void* brk = mtcp_sys_brk(NULL);
  if( *(int*)nam == *(int*)"[heap]" && brk != nam + size) {
    //mtcp_output(MTCP_WARNING,"WARNING: break (%p) not equal to end of heap (%p)\n",brk,nam+size);
  }

  return data_len;
}

/// Print data content
STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_sys_print(const SysInfo& s) {
  void* curr_break = mtcp_sys_brk(0);
  // Checkpoint image
  mtcp_output(MTCP_INFO,"checkpoint: SysInfo:       %p -> %p Size:%d\n",&s,s.sysInfo,sizeof(SysInfo));
  mtcp_output(MTCP_INFO,"checkpoint: Checkpoint[%d]:%s \n",s.checkpointFD,s.checkpointFile);
  mtcp_output(MTCP_INFO,"checkpoint: dto.   begin:  %p end    %p [%X bytes]\n",
	      s.chkptStart,s.chkptStart+s.chkptSize,s.chkptSize);

  // Restore image
  mtcp_output(MTCP_INFO,"checkpoint: Page   size:   %d [%X]\n",s.pageSize,s.pageSize);
  mtcp_output(MTCP_INFO,"checkpoint: Image  name:  '%s'\n",s.checkpointImage);
  mtcp_output(MTCP_INFO,"checkpoint: Image  begin:  %p end    %p [%X bytes]\n",
	      s.addrStart,s.addrEnd,s.addrSize);

  // Heap & Stack
  mtcp_output(MTCP_INFO,"checkpoint: Heap   curr:   %p saved  %p\n",curr_break,s.saved_break);
  mtcp_output(MTCP_INFO,"checkpoint: Stack lim soft:%p hard:  %p\n",s.stackLimitCurr,s.stackLimitHard);

  // The finishRestore function pointer:
  mtcp_output(MTCP_INFO,"checkpoint: Restore start: %p finish:%p\n",s.startRestore,s.finishRestore);
  mtcp_output(MTCP_INFO,"checkpoint: Mother of all: %p \n",s.motherofall);
}

STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_sys_restore_start(SysInfo* sys, int print_level) {
  /* If we just replace extendedStack by (tempstack+STACKSIZE) in "asm"
   * below, the optimizer generates non-PIC code if it's not -O0 - Gene
   */
  long long* extendedStack = mtcp_internal_tempstack + STACKSIZE;

  mtcp_set_debug_level(print_level);
  /* Switch to a stack area that's part of the shareable's memory address range
   * and thus not used by the checkpointed program
   */
  if ( sys ) {
    mtcp_output(MTCP_INFO,"restore_start....First check input system information:%p\n",sys);
    checkpointing_sys_print(*sys);
  }
  checkpointing_sys_print(chkpt_sys);

  // Now we move the process to the temporary stack allocated in this image
  // in order to execute the process restore. After this, we move back
  // to the regular stack pointer.
  asm volatile (CLEAN_FOR_64_BIT(mov %0,%%esp\n\t)
                /* This next assembly language confuses gdb,
		   but seems to work fine anyway */
                CLEAN_FOR_64_BIT(xor %%ebp,%%ebp\n\t)
                : : "g" (extendedStack) : "memory");
#if 0  /* Same as above.... */
  asm volatile (CLEAN_FOR_64_BIT(mov %0,%%esp\n\t)
                : : "g" (extendedStack) : "memory");
  //asm("mov $0,%rbp");
  asm volatile ("xor %rbp,%rbp");
#endif
  // Stack is wacked. Need to call a new routinne, which should never return.
  checkpointing_restore_process();
  asm volatile ("hlt");
}

STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_restore_process() {
  struct stat sb;
  const char* nam = chkpt_sys.checkpointFile;
  int fdnum = chkpt_sys.checkpointFD;
  int fd = mtcp_sys_open(nam,O_RDONLY,0);
  int rc = mtcp_sys_lstat(nam,&sb);
  if ( rc < 0 ) {
    mtcp_output(MTCP_FATAL,"restore: Failed [%d] to stat checkpoint file:%s\n",mtcp_sys_errno,nam);
  }
  if ( fd != fdnum ) {  // Move file to the same descriptor used for writing the checkpoint!
    if ( mtcp_sys_dup2(fd,fdnum) < 0) {
      mtcp_output(MTCP_ERROR,"FileDesc: error %d [%s] restore %s fd %d to %d.\n", 
		  mtcp_sys_errno, strerror(mtcp_sys_errno), nam, fd, fdnum);
      mtcp_sys_close (fd);
      return;
    }
    mtcp_sys_close(fd);
  }
  Process p(Process::PROCESS_RESTORE);
  if ( checkpointing_process_fread(&p,fdnum) ) {
    mtcp_output(MTCP_INFO,"Finished to restore files and memory areas from %s.\n",nam);
  }
  mtcp_output(MTCP_FATAL,"Failed to read process information from file %s.\n",nam);
}

STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_sys_restore_finish() {
  SysInfo& s = chkpt_sys;
  mtcp_output(MTCP_INFO,"restore: restore complete, resuming. Jump to:%p\n",s.finishRestore);
  // Jump to finishrestore in original program's libmtcp.so image
  (*(s.finishRestore))();
}
