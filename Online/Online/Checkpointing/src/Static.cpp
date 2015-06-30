#define CHECKPOINTING_STATIC_CPP
#include "Checkpointing/Namespace.h"
#include "Checkpointing/Static.h"
#include "Checkpointing/MemMaps.h"
#include "Checkpointing/SysCalls.h"
#include "Checkpointing.h"
#include "Restore.h"

#include <cstdio>
#include <unistd.h>
#include <sys/resource.h>

using namespace Checkpointing;

namespace CHECKPOINTING_NAMESPACE {
  SysInfo chkpt_sys;
}

typedef SysInfo::mem_address_t mem_address_t;
#define PAGE_SIZE 4096
DefineMarker(SYS_BEGIN_MARKER,    "PSYS");
DefineMarker(SYS_END_MARKER,      "psys");

//#define STACKSIZE 8192      // size of temporary stack (in quadwords)
//#define STACKSIZE 4096      // size of temporary stack (in quadwords)
#define STACKSIZE 2048      // size of temporary stack (in quadwords)
//#define STACKSIZE 1024      // size of temporary stack (in quadwords)
/* temp stack used internally by restore so we don't go outside the
 *   libmtcp.so address range for anything;
 * including "+ 1" since will set %esp/%rsp to tempstack+STACKSIZE
 */
STATIC(long long int) mtcp_internal_tempstack[STACKSIZE + 1];

/// Initialize SysInfo structure
STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_sys_initialize(SysInfo* sys) {
  struct rlimit lim;
  ::getrlimit(RLIMIT_NOFILE, &lim);
  sys->startRestore       = checkpointing_sys_restore_start;
  sys->finishRestore      = 0;

  sys->stackLimitCurr     = 0;
  sys->stackLimitHard     = 0;
  sys->saved_break        = 0;
  sys->addrSize           = 0;
  sys->addrStart          = 0;
  sys->addrEnd            = 0;
  sys->chkptStart         = 0;
  sys->chkptSize          = 0;
  sys->vsyscallStart      = 0;
  sys->vsyscallEnd        = 0;
  sys->sysInfo            = 0;
  sys->pageSize           = ::sysconf(_SC_PAGESIZE);
  sys->checkpointFD       = lim.rlim_max-1;
  sys->checkpointLibs[0]  = 0;
  sys->checkpointFile[0]  = 0;
  sys->checkpointImage[0] = 0;
  sys->motherofall        = 0;
  sys->motherPID          = 0;
  sys->chkptPID           = 0;
  sys->restart_type       = 0;
  sys->restart_flags      = 0;
  sys->save_flags         = 0;
  sys->utgid              = 0;
  sys->utgidLen           = 0;
  sys->argv               = 0;
  sys->argc               = 0;
  sys->envp               = 0;
  sys->arg0               = 0;
  sys->arg0String[0]      = 0;
  sys->restore_argc       = 0;
  sys->restore_arg0       = 0;
  sys->restore_arglen     = 0;
  sys->restore_utgid      = 0;
  sys->restore_utgidLen   = 0;
  sys->numTmpFiles        = 0;
  for(size_t i=0; i<sizeof(sys->tmpFiles)/sizeof(sys->tmpFiles[0]);++i) {
    sys->tmpFiles[i].name[0] = 0;
    sys->tmpFiles[i].fd = -1;
  }
}

/// Set directory name for libraries to be restored.
STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_sys_set_lib_directory(SysInfo* sys, const char* dir_name)   {
  if ( dir_name ) {
    m_memcpy(sys->checkpointLibs,dir_name,m_strlen(dir_name));
  }
  else {
    sys->checkpointLibs[0] = 0;
  }
}

/// Set save flags
STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_sys_set_save_flags(SysInfo* sys, int flags) {
  sys->save_flags = flags;
}

/// Set restart flags
STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_sys_set_restart_flags(SysInfo* sys, int flags) {
  sys->restart_flags = flags;
}

/// Main restart routine in checkpointing image
STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_sys_restore_start(int argc, char** argv, char** env, const char* libs_dir,int print_level,int flags) {
  /* If we just replace extendedStack by (tempstack+STACKSIZE) in "asm"
   * below, the optimizer generates non-PIC code if it's not -O0 - Gene
   */
  long long* extendedStack = mtcp_internal_tempstack + STACKSIZE;
  mtcp_set_debug_level(print_level);
  mtcp_output(MTCP_INFO,"restore: Assume that checkpoint file is already opened with fd:%d\n",chkpt_sys.checkpointFD);
  if ( mtcp_sys_lseek(chkpt_sys.checkpointFD,0,SEEK_SET) < 0 ) {// Position checkpoint file to start
    mtcp_output(MTCP_FATAL,"restore: Failed [%d] to seek back to beginning of the checkpoint file\n",mtcp_sys_errno);
  }
  chkpt_sys.restart_flags = flags;
  mtcp_output(MTCP_INFO,"restore: restore stack.\n");
  checkpointing_sys_init_restore_stack(&chkpt_sys,argc,argv,env);
  if ( libs_dir )
    m_strcpy(chkpt_sys.checkpointLibs,libs_dir);
  else
    chkpt_sys.checkpointLibs[0] = 0;
  mtcp_output(MTCP_INFO,"restore: restore stack.....done\n");
  checkpointing_sys_print(&chkpt_sys);

  // Now we move the process to the temporary stack allocated in this image
  // in order to execute the process restore. After this, we move back
  // to the regular stack pointer.
  __asm__ volatile (CLEAN_FOR_64_BIT(mov %0,%%esp\n\t)
                    /* This next assembly language confuses gdb,
                       but seems to work fine anyway */
                    CLEAN_FOR_64_BIT(xor %%ebp,%%ebp\n\t)
                    : : "g" (extendedStack) : "memory");
#if 0  /* Same as above.... */
  __asm__ volatile (CLEAN_FOR_64_BIT(mov %0,%%esp\n\t)
                    : : "g" (extendedStack) : "memory");
  //__asm__("mov $0,%rbp");
  __asm__ volatile ("xor %rbp,%rbp");
#endif
  // Stack is wacked. Need to call a new routinne, which should never return.
  checkpointing_sys_restore_process();
  __asm__ volatile ("hlt");
}

/// Set the file descriptor for the checkpointing file
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_sys_set_checkpoint_file(SysInfo* sys, int fd) {
  char text[64], nam[PATH_MAX];
  ::sprintf(text,"/proc/self/fd/%d",fd);
  int name_len = (int)mtcp_sys_readlink(text,nam,sizeof(nam)-1);
  nam[name_len] = 0;
  m_memcpy(sys->checkpointFile,nam,sizeof(sys->checkpointFile));
  sys->checkpointFD = fd;
  return 1;
}

STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_sys_aquire(SysInfo* sys) {
  struct rlimit lim;

  // Get Stack limit
  ::getrlimit(RLIMIT_STACK, &lim);
  sys->stackLimitCurr = lim.rlim_cur;
  sys->stackLimitHard = lim.rlim_max;
  sys->envp = environ;

  // Check heap
  void* next_break = mtcp_sys_brk(sys->saved_break);
  if ( next_break == (void*)-1 ) {
    mtcp_output(MTCP_FATAL,"restore: sbrk(%p): errno: %d (bad heap)\n",sys->saved_break,mtcp_sys_errno);
  }
  if ( 0 == sys->addrSize || 0 == sys->chkptSize || !sys->checkpointImage[0] ) {
    AreaInfoHandler hdlr;
    MemMaps mm;
    if ( 1 == mm.scan(hdlr) ) {
      mem_address_t sz = hdlr.imageAddr[1]-hdlr.imageAddr[0];
      //sz = ((sz+PAGE_SIZE-1) & (-PAGE_SIZE)); // icc does not like this...
      sz = ((sz+PAGE_SIZE-1)/PAGE_SIZE)*PAGE_SIZE;
      m_memcpy(sys->checkpointImage,hdlr.image,sizeof(sys->checkpointImage));
      sys->chkptStart    = hdlr.checkpointAddr[0];
      sys->chkptSize     = hdlr.checkpointAddr[1]-hdlr.checkpointAddr[0];
      sys->addrSize      = sz;//high-low;
      sys->addrStart     = hdlr.imageAddr[0];
      sys->addrEnd       = hdlr.imageAddr[0]+sys->addrSize;
      sys->vsyscallStart = hdlr.vsyscall[0];
      sys->vsyscallEnd   = hdlr.vsyscall[1];
      return;
    }
    mtcp_output(MTCP_FATAL,"restore: Failed to access checkpointing image.\n");
  }
}

/// Print data content of SysInfo structure
STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_sys_print(const SysInfo* s) {
  void* curr_break = mtcp_sys_brk(0);
  // Checkpoint image
  mtcp_output(MTCP_INFO,"checkpoint: SysInfo:       %p -> %p Size:%d\n",&s,s->sysInfo,int(sizeof(SysInfo)));
  mtcp_output(MTCP_INFO,"checkpoint: Checkpoint[%d]:%s \n",s->checkpointFD,s->checkpointFile);
  mtcp_output(MTCP_INFO,"checkpoint: dto.   begin:  %p end    %p [%X bytes]\n",
              s->chkptStart,s->chkptStart+s->chkptSize,s->chkptSize);

  // Restore image
  mtcp_output(MTCP_INFO,"checkpoint: Page   size:   %d [%X]\n",s->pageSize,s->pageSize);
  mtcp_output(MTCP_INFO,"checkpoint: Image  name:  '%s'\n",s->checkpointImage);
  mtcp_output(MTCP_INFO,"checkpoint: Image  begin:  %p end    %p [%X bytes]\n",
              s->addrStart,s->addrEnd,s->addrSize);

  // Library temporary directory
  mtcp_output(MTCP_INFO,"checkpoint: Tmp.lib. dir: '%s'\n",s->checkpointLibs);

  // Heap & Stack
  mtcp_output(MTCP_INFO,"checkpoint: Heap   curr:   %p saved  %p\n",curr_break,s->saved_break);
  mtcp_output(MTCP_INFO,"checkpoint: Stack lim soft:%p hard:  %p\n",s->stackLimitCurr,s->stackLimitHard);
  mtcp_output(MTCP_INFO,"checkpoint: tmpstack start:%p finish:%p\n",
              mtcp_internal_tempstack,mtcp_internal_tempstack+STACKSIZE+1);
  mtcp_output(MTCP_INFO,"checkpoint: argc          :%d          \n",s->argc);

  // The finishRestore function pointer:
  mtcp_output(MTCP_INFO,"checkpoint: Restore start: %p finish:%p\n",s->startRestore,s->finishRestore);
  mtcp_output(MTCP_INFO,"checkpoint: Mother of all: %p \n",s->motherofall);
  mtcp_output(MTCP_INFO,"checkpoint: Restore flags: %X \n",s->restart_flags);
#if 0
  for(size_t i=0; i<sizeof(s->tmpFiles)/sizeof(s->tmpFiles[0]);++i) {
    const SysInfo::TmpFile& tmp = s->tmpFiles[i];
    mtcp_output(MTCP_INFO,"checkpoint: tmp File:  %d  %s\n",tmp.fd,tmp.name);
  }
#endif
}
#if 0
/// Get program context
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_sys_get_context()  {
  if ( ::getcontext (&chkpt_sys.motherofall->m_savctx) < 0 )   {
    mtcp_output(MTCP_FATAL,"checkpointing_sys_get_context: failed to retrieve context of motherofall.\n");
  }
  return 1;
}
#endif
static void handle_set_env_string(void* /* par */, const char* s) {
  char* q = (char*)m_chrfind(s,'=');
  if ( q ) {
    *q = 0;
    if ( 0 == m_strcmp(s,"UTGID") ) 
      checkpointing_sys_set_utgid(&chkpt_sys,q+1);
    else if ( 0 == ::setenv(s,q+1,1) )
      mtcp_output(MTCP_DEBUG,"Set environment: %s=%s\n",s,q+1);
    else
      mtcp_output(MTCP_ERROR,"FAILED Set environment: %s=%s\n",s,q+1);
    return;
  }
  mtcp_output(MTCP_ERROR,"FAILED Set environment: %s\n",s);
}

/// After successful restore update the process environment from file.
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_sys_set_environment(SysInfo* sys) {
  if ( MTCP_STDIN_ENV == (sys->restart_flags&MTCP_STDIN_ENV) ) {
    mtcp_output(MTCP_INFO,"Update process environment from stdin....\n");
    checkpointing_sys_process_file(STDIN_FILENO,0,handle_set_env_string);
    return 1;
  }
  mtcp_output(MTCP_INFO,"The flag MTCP_STDIN_ENV is not set. Do not modify process environment.\n");
  return 1;
}

static void handle_env_strings_print(void* /* par */, const char* s) {
  mtcp_output(MTCP_INFO,"%s\n",s);
}

STATIC(long) CHECKPOINTING_NAMESPACE::checkpointing_sys_fwrite(int fd, const SysInfo* s) {
  long off, offset, bytes = writeMarker(fd,SYS_BEGIN_MARKER);
  int  len, count_arg, count_env;
  bytes += writeInt(fd,sizeof(SysInfo));
  bytes += m_writemem(fd,s,sizeof(SysInfo));
  if ( s->argv || s->envp ) {
    char** e;
    len = 0;
    offset = mtcp_sys_lseek(fd,0,SEEK_CUR);
    writeInt(fd,0);  // placeholder....
    // Write array lengths
    for(e=s->argv, count_arg=0; e && *e; ++e) ++count_arg;
    len += writeInt(fd,count_arg);
    for(e=s->envp, count_env=0; e && *e; ++e) ++count_env;
    len += writeInt(fd,count_env);
    // Write array data
    for(e=s->argv; e && *e; ++e)
      len += m_writemem(fd,*e,m_strlen(*e)+1);
    for(e=s->envp; e && *e; ++e)
      len += m_writemem(fd,*e,m_strlen(*e)+1);
    bytes += len;
    off = mtcp_sys_lseek(fd,0,SEEK_CUR);
    mtcp_sys_lseek(fd,offset,SEEK_SET);
    writeInt(fd,len);
    mtcp_sys_lseek(fd,off,SEEK_SET);
    mtcp_output(MTCP_INFO,"checkpoint: Wrote %d bytes of stack at address %p.\n",len,s->argv[0]);
  }
  else {
    mtcp_output(MTCP_INFO,"checkpoint: No stack saving requested!\n");
    bytes += writeInt(fd,0);
  }
  offset = mtcp_sys_lseek(fd,0,SEEK_CUR);
  bytes += writeInt(fd,0);
  AreaChkptWriteHandler hdlr(fd);
  if ( 1 != checkpointing_memory_scan(&hdlr) ) {
    mtcp_output(MTCP_FATAL,"checkpoint: Failed to write checkpointing image.\n");
  }
  len = hdlr.bytesWritten();
  off = mtcp_sys_lseek(fd,0,SEEK_CUR);
  mtcp_sys_lseek(fd,offset,SEEK_SET);
  writeInt(fd,len);
  mtcp_sys_lseek(fd,off,SEEK_SET);
  bytes += writeMarker(fd,SYS_END_MARKER);
  mtcp_output(MTCP_INFO,"checkpoint: Wrote %d [%d] bytes of restore image.\n",len,s->addrSize);
  return bytes;
}

/// Read system information from checkpoint file
STATIC(long) CHECKPOINTING_NAMESPACE::checkpointing_process_skip_sys(Process*,const void* addr)  {
  int siz;
  const_Pointer in = (const_Pointer)addr;
  in += checkMarker(in,SYS_BEGIN_MARKER);
  in += getInt(in,&siz);
  in += siz;              // skip SysInfo
  in += getInt(in,&siz);  
  in += siz;              // skip stack
  in += getInt(in,&siz);  
  in += siz;              // skip image
  in += checkMarker(in,SYS_END_MARKER);
  return addr_diff(in,addr);
}

/// Read system information from checkpoint file
STATIC(long) CHECKPOINTING_NAMESPACE::checkpointing_process_fskip_sys(Process*,int fd)  {
  int siz, in = 0;
  in += readMarker(fd,SYS_BEGIN_MARKER);
  in += readInt(fd,&siz);
  in += m_fskip(fd,siz);  // skip SysInfo
  in += readInt(fd,&siz);
  in += m_fskip(fd,siz);  // skip stack
  in += readInt(fd,&siz);
  in += m_fskip(fd,siz);  // skip image
  in += readMarker(fd,SYS_END_MARKER);
  return in;
}

/// Initialize basic variables from stack when restoring from checkpoint
STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_sys_init_restore_stack(SysInfo* sys, int argc, char** argv, char** envp) {
  char** ee = envp;
  sys->restore_argc   = argc;
  sys->restore_arg0   = mem_address_t(argv[0]);
  sys->restore_arglen = mem_address_t(argv[argc-1])+m_strlen(argv[argc-1])-sys->restore_arg0+1;
  if ( ee )   {
    for(char* ep=*ee; *ep && *ee; ++ee, ep=*ee ) {
      if ( ep )    {
        if ( 0 == m_strncmp(ep,"UTGID=",6) ) {
          char* ptr = ep + 6;
          sys->restore_utgid    = mem_address_t(ptr);
          sys->restore_utgidLen = m_strlen(ptr);
          break;
        }
        else if ( *(short*)ep == *(short*)"_=" ) break;
      }
    }
  }
}

/// Initialize basic variables from stack
STATIC(void) CHECKPOINTING_NAMESPACE::checkpointing_sys_init_stack(SysInfo* sys, int argc, char** argv, char** envp)  {
  char** ee = envp;
  const char* arg0 = argv[0];
  sys->utgid       = 0;
  sys->utgidLen    = 0;
  sys->argv        = argv;
  sys->argc        = argc;
  sys->envp        = envp;
  sys->arg0        = mem_address_t(argv[0]);
  m_memcpy(sys->arg0String,argv[0],m_strlen(argv[0])+1);
  if ( ee ) {
    for(char* ep=*ee; *ee && *ep; ep=*(++ee)) {
      if ( ep ) {
        if ( 0 == m_strncmp(ep,"UTGID=",6) ) {
          sys->utgid = ep + 6;
          sys->utgidLen = m_strlen(sys->utgid);
        }
        else if ( 0 == m_strncmp(ep,"MTCP_OUTPUT=",11) ) {
          char* ptr = ep + 12;
          if ( *ptr == 'D' || *ptr == 'V' || *ptr=='0' || *ptr == '1' )
            mtcp_set_debug_level(MTCP_DEBUG);
          if ( *ptr == 'I' || *ptr=='2' )
            mtcp_set_debug_level(MTCP_INFO);
          if ( *ptr == 'W' || *ptr=='3' )
            mtcp_set_debug_level(MTCP_WARNING);
          if ( *ptr == 'E' || *ptr=='4' )
            mtcp_set_debug_level(MTCP_ERROR);
          if ( *ptr == 'F' || *ptr=='5' )
            mtcp_set_debug_level(MTCP_FATAL);
        }
        else if ( *(short*)ep == *(short*)"_=" ) break;
      }
    }
  }
  mtcp_output(MTCP_DEBUG,"Environ:%p argv[0]:%p %s UTGID:%p %s\n",
              ee,arg0,arg0,sys->utgid,sys->utgid ? sys->utgid : "Unknown");
}


/// Setup process UTGID/argv[0] if availible
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_sys_set_utgid(SysInfo* sys, const char* new_utgid) {
  int len = m_strlen(new_utgid)+1;
#if 0
  //int max_len = sys->restore_arglen;
  if ( sys->restore_arg0 < sys->arg0 ) {
    mtcp_output(MTCP_ERROR,"Failed to set argv[0] to utgid:%s orig arg0:%p > new arg0:%p len=%d\n",
                new_utgid, sys->arg0, sys->restore_arg0, len);
  }
#endif
  if ( sys->restore_utgid ) {
    if ( len < sys->restore_utgidLen ) {
      char* p = (char*)sys->restore_utgid;
      m_memcpy(p,new_utgid,len);
      m_memset(p+len,0,sys->restore_utgidLen-len);
    }
    else {
      mtcp_output(MTCP_ERROR,"Failed to update utgid restore stack environment: len(%d)>allowed(%d)\n",
                  len, sys->restore_utgidLen);
    }
  }
  if ( sys->restore_arg0 ) {
    if ( len < sys->restore_arglen ) {
      char* p = (char*)sys->restore_arg0;
      m_memcpy(p,new_utgid,len);
      mtcp_output(MTCP_INFO,"New UTGID: %p %s - %s arglen:%d utgidlen:%d\n",
                  sys->restore_arg0,sys->restore_arg0,new_utgid,sys->restore_arglen,len);
      if ( sys->restore_arglen > len )  {
        m_memset(p+len,0,sys->restore_arglen - len);
      }
      if ( sys->utgid && sys->utgidLen>len ) {
        mtcp_output(MTCP_INFO,"Sys UTGID: %p %s - %s\n",sys->utgid, sys->utgid,new_utgid);
        m_memcpy(sys->utgid,new_utgid,len);
      }
      else {
        mtcp_output(MTCP_ERROR,"Failed to update utgid stack environment: len(%d)>allowed(%d)\n",
                    len,sys->utgidLen);
      }
    }
    else {
      mtcp_output(MTCP_ERROR,"Failed to set argv[0] to utgid:%s orig arg0:%p new arg0:%p len=%d\n",
                  new_utgid, sys->arg0, sys->restore_arg0, len);
    }
  }
  return ::setenv("UTGID",new_utgid,1);
}

/// Force process UTGID if availible
STATIC(int) CHECKPOINTING_NAMESPACE::checkpointing_sys_force_utgid(SysInfo* sys, const char* new_utgid) {
  int len = new_utgid ? m_strlen(new_utgid) : 0;
  if ( 0 == new_utgid ) {
    mtcp_output(MTCP_FATAL,"New UTGID pointer NULL.\n");
    len = 0;
    new_utgid="";
  }
  else if ( sys->utgid == 0 ) {
    mtcp_output(MTCP_WARNING,"UTGID pointer NULL adding UTGID=%s to local environment.\n", new_utgid);
  }
  else if ( sys->utgidLen != len ) {
    mtcp_output(MTCP_FATAL,"New UTGID %s too long to replace old value from process stack with length [%d].\n",
                new_utgid,sys->utgidLen);
  }
  if ( sys->utgid ) {
    m_memcpy(sys->utgid,new_utgid,len);
  }
  if ( new_utgid ) {
    return checkpointing_sys_set_utgid(sys,new_utgid);
  }
  return 1;
}


SysInfo::SysInfo() {
  checkpointing_sys_initialize(this);
}

int test_set_environment(int flag) {
  if ( flag&MTCP_STDIN_ENV) {
    checkpointing_sys_process_file(STDIN_FILENO,0,handle_env_strings_print);
    return 1;
  }
  mtcp_output(MTCP_FATAL,"The flag MTCP_STDIN_ENV is not set. Cannot handle environment.\n");
  return 0;
}

