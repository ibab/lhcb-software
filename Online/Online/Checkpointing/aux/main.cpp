#define __STATIC__
#define CHECKPOINTING_RESTORE
#define CHECKPOINTING_SYSINFO_STRUCT_ONLY

// Checkpointing include files
#include "Checkpointing/Static.h"
#include "Checkpointing/SysCalls.h"
#include "Checkpointing/SysCallsImp.h"

// C/C++ include files
#include <cerrno>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/personality.h>

using namespace Checkpointing;
//static void* mtcp_global_stack_end = 0;
static int checkMarker(int fd, Marker should) {
  Marker got;
  mtcp_sys_read(fd,&got,sizeof(got));
  const char* a = (char*)&got;
  const char* b = (char*)&should;
  if ( got != should ) {
    mtcp_output(MTCP_ERROR,"Seen invalid markers: Got:%d '%c%c%c%c' Should be:%d '%c%c%c%c'\n",
                got,a[0],a[1],a[2],a[3],should,b[0],b[1],b[2],b[3]);
  }
  return 1;
}

static inline size_t m_strlen(const char* s) {
  size_t l = 0;
  for(; *s; ++s) ++l;
  return l;
}
static inline size_t m_strcpy(char* t, const char* s) {
  const char* q=s;
  for(; *q; ++q, ++t) *t = *q;
  *t = 0;
  return q-s;
}

void mtcp_executable(char* cmd_file) {
  // Copy command line to libmtcp.so, so that we can re-exec if randomized vdso
  //   steps on us.  This won't be needed when we use the linker to map areas.
  cmd_file[0] = 0;
  int cmd_len = mtcp_sys_readlink("/proc/self/exe", cmd_file, PATH_MAX);
  if (cmd_len == -1) mtcp_output(MTCP_FATAL,"Couldn't find /proc/self/exe.\n");
  cmd_file[cmd_len] = 0;
}

static void load(int org_argc, char** org_argv, const char* file_name, const char* lib_dir, int advise, bool restart, SysInfo::start_restore_t* func) {
  typedef void* pvoid;
  const Marker PROCESS_BEGIN_MARKER = *(Marker*)"PROC";
  const Marker SYS_BEGIN_MARKER     = *(Marker*)"PSYS";
  const Marker SYS_END_MARKER       = *(Marker*)"psys";
  const Marker LIBS_BEGIN_MARKER    = *(Marker*)"LIBS";
  const Marker LIBS_END_MARKER      = *(Marker*)"libs";
  const Marker LIBRARY_BEGIN_MARKER = *(Marker*)"XLIB";
  const Marker LIBRARY_END_MARKER   = *(Marker*)"xlib";
  int   rc, siz, fd, fdnum, num_libs;
  long  libs_size = 0;
  struct stat sb;
  SysInfo sys;

  *func = 0;
  rc = mtcp_sys_lstat(file_name,&sb);
  if ( rc < 0 ) {
    mtcp_output(MTCP_FATAL,"restore: Failed [%d] to stat checkpoint file:%s\n",mtcp_sys_errno,file_name);
  }
  fd = mtcp_sys_open(file_name, O_RDONLY, 0);
  if ( fd < 0 ) {
    mtcp_output(MTCP_FATAL,"Failed to open checkpoint file:%s\n",file_name);
  }
  checkMarker(fd,PROCESS_BEGIN_MARKER);

  checkMarker(fd,LIBS_BEGIN_MARKER);
  mtcp_sys_read(fd,&libs_size,sizeof(long));
  mtcp_sys_read(fd,&num_libs,sizeof(int));
  if ( libs_size > 0 )   {
    if ( !lib_dir || !restart ) {
      long offset = ::lseek(fd,0,SEEK_CUR);
      mtcp_sys_lseek(fd,offset+libs_size,SEEK_SET);
      mtcp_output(MTCP_INFO,"Ignore image section in checkpoint file:%s - restoring originals\n",file_name);
    }
    else {
      int dir_len = m_strlen(lib_dir);
      for ( int ilib=0; ilib<num_libs; ++ilib ) {
        int name_len = 0;
        char lib[PATH_MAX], buffer[PATH_MAX];
        mtcp_sys_mkdir(lib_dir,0777);
        m_strcpy(lib,lib_dir);
        if ( lib[dir_len-1] != '/' ) { lib[dir_len]='/';lib[dir_len+1]=0; }
        checkMarker(fd,LIBRARY_BEGIN_MARKER);
        mtcp_sys_read(fd,&name_len,sizeof(name_len));
        if ( name_len > 0 ) mtcp_sys_read(fd,lib+m_strlen(lib),name_len);
        mtcp_sys_read(fd,&siz,sizeof(siz));
        if ( name_len > 0 )   {
          mtcp_sys_unlink(lib);
          int lib_fd = mtcp_sys_open(lib,O_WRONLY|O_TRUNC|O_CREAT,0777);
          if ( lib_fd < 0 ) {
            mtcp_output(MTCP_FATAL,"restore: error restoring image file:%s  errno:%d\n",lib,mtcp_sys_errno);
          }
          mtcp_output(MTCP_INFO,"Restoring %d bytes for library[%d/%d] %s\n",siz,ilib+1,num_libs,lib);
          while(siz>=PATH_MAX) {
            mtcp_sys_read(fd,buffer,sizeof(buffer));
            int rc = mtcp_sys_write(lib_fd,buffer,sizeof(buffer));
            if ( rc < 0 ) {
              mtcp_output(MTCP_FATAL,"restore: error writing image file:%s  errno:%d\n",lib,mtcp_sys_errno);
            }
            siz -= PATH_MAX;
          }
          while(siz>0) {
            mtcp_sys_read(fd,buffer,1);
            int rc = mtcp_sys_write(lib_fd,buffer,1);
            if ( rc < 0 ) {
              mtcp_output(MTCP_FATAL,"restore: error writing image file:%s  errno:%d\n",lib,mtcp_sys_errno);
            }
            siz -= 1;
          }
          mtcp_sys_close(lib_fd);
        }
        checkMarker(fd,LIBRARY_END_MARKER);
      }
    }
  }
  else {
    mtcp_output(MTCP_INFO,"Image section is EMPTY in checkpoint file:%s - restoring originals\n",file_name);
  }
  checkMarker(fd,LIBS_END_MARKER);

  checkMarker(fd,SYS_BEGIN_MARKER);
  mtcp_sys_read(fd,&siz,sizeof(siz));
  mtcp_output(MTCP_DEBUG,"SysInfo has a size of %d bytes\n",siz);
  mtcp_sys_read(fd,&sys,sizeof(SysInfo));
  mtcp_sys_read(fd,&siz,sizeof(siz));
  mtcp_output(MTCP_DEBUG,"SysInfo has %d bytes of stack information. argc=%d argv=%p arg0:%p\n",siz,sys.argc,sys.argv,sys.arg0);
  mtcp_output(MTCP_DEBUG,"SysInfo arg0:%s\n",sys.arg0String);
#define MBYTE 1024*1024
  if ( siz > 0 ) {
    int i, num_arg, num_env, count;
    char buffer[MBYTE];  // must be from stack! Don't use malloc
    char cmdLine[PATH_MAX+1], *ptr = buffer;
    char *argv[128], *envp[1024], **e;
    mtcp_sys_read(fd,&num_arg,sizeof(num_arg));
    mtcp_sys_read(fd,&num_env,sizeof(num_env));

    if ( siz > MBYTE ) {
      mtcp_output(MTCP_ERROR,"SysInfo stack buffer of %d bytes too small to hold %d bytes of stack.\n",MBYTE,siz);
      mtcp_output(MTCP_ERROR,"You need to increase the buffer. file:%s line:%d\n",__FILE__,__LINE__);
      mtcp_sys_exit(0);
    }
    mtcp_sys_read(fd,buffer,siz-2*sizeof(int));    
    if ( restart ) {
      int l1=0, l2=0;
      mtcp_executable(cmdLine);
      for(i=0; i<num_arg;++i) {
        mtcp_output(MTCP_DEBUG,"SysInfo-arg[%d] = %s\n",i,ptr);
        ptr += m_strlen(ptr)+1;
        l1+= m_strlen(ptr)+1;
      }
      for(i=0; i<num_env;++i) {
        mtcp_output(MTCP_DEBUG,"SysInfo-env[%d] = %s\n",i,ptr);
        envp[i] = ptr;
        ptr += m_strlen(ptr)+1;
      }
      envp[num_env] = 0;
      for(i=0; i<org_argc;++i) {
        mtcp_output(MTCP_DEBUG,"SysInfo-restart-arg[%d] = %s\n",i,org_argv[i]);
        argv[i] = org_argv[i];
        l2 += m_strlen(argv[i])+1;
      }
      argv[org_argc] = (char*)"-runnow";
      l2 += m_strlen(argv[org_argc])+1;
      argv[org_argc+1] = 0;
      for(e=environ, count=0;e&&*e; ++e)count++;
      mtcp_output(MTCP_INFO,"SysInfo-arg-len  %d \n",long(org_argv[0])-sys.arg0);
      mtcp_output(MTCP_INFO,"SysInfo-arg-len  %d  checkpointed:%d\n",l2,l1);
      mtcp_output(MTCP_INFO,"SysInfo-#argc:   %d  checkpointed:%d   #env:    %d  checkpointed:%d\n",
                  org_argc+1,sys.argc,count,num_env);
      mtcp_output(MTCP_INFO,"SysInfo-argv[0]: %p  checkpointed:%p\n",org_argv[0],sys.arg0);
      mtcp_output(MTCP_INFO,"SysInfo-cmdline = %s\n",cmdLine);
      mtcp_output(MTCP_INFO,"SysInfo-cmdline = %s\n",sys.arg0String);
      mtcp_output(MTCP_INFO,"SysInfo: execve restart process.....\n");
      mtcp_sys_close(fd);
      execve(cmdLine,argv,envp);
    }
  }
  else {
    mtcp_output(MTCP_DEBUG,"checkpoint: SysInfo: No stack environment and args present.\n");
  }
  mtcp_output(MTCP_DEBUG,"checkpoint: SysInfo:       %p \n",sys.sysInfo);
  mtcp_output(MTCP_DEBUG,"checkpoint: Page   size:   %d [%X]\n",int(sys.pageSize),int(sys.pageSize));
  mtcp_output(MTCP_DEBUG,"checkpoint: Image  name:  '%s'\n",sys.checkpointImage);
  mtcp_output(MTCP_DEBUG,"checkpoint: Image  begin:  %p end    %p [%X bytes]\n",
              pvoid(sys.addrStart),pvoid(sys.addrEnd),int(sys.addrSize));
  mtcp_output(MTCP_DEBUG,"checkpoint: Heap   saved:  %p\n",pvoid(sys.saved_break));
  mtcp_output(MTCP_DEBUG,"checkpoint: Stack lim soft:%p hard:  %p\n",
              pvoid(sys.stackLimitCurr),pvoid(sys.stackLimitHard));
  // The finishRestore function pointer:
  mtcp_output(MTCP_DEBUG,"checkpoint: Restore start: %p finish:%p\n",
              sys.startRestore,sys.finishRestore);

  mtcp_output(MTCP_INFO,"SysInfo-argc:    %d  checkpointed:%d\n",org_argc,sys.argc);
  mtcp_output(MTCP_INFO,"SysInfo-argv[0]: %p  checkpointed:%p\n",org_argv[0],sys.arg0);

  void* data = (void*)mtcp_sys_mmap((void*)sys.addrStart,sys.addrSize,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_ANONYMOUS|MAP_FIXED|MAP_PRIVATE,-1,0);
  if (data == MAP_FAILED) {
    if (errno != EBUSY) {
      mtcp_output(MTCP_FATAL,"restore: error creating %d byte restore region at %p: %s\n",
                  int(sys.addrSize),pvoid(sys.addrStart),::strerror(errno));
    } 
    else {
      mtcp_sys_close(fd);
      mtcp_output(MTCP_FATAL,"restore:  address conflict...\n");
    }
  }

  if ( data != (void*)sys.addrStart ) {
    mtcp_output(MTCP_FATAL,"restore: %d byte restore region at %p got mapped at %p\n",
                int(sys.addrSize),pvoid(sys.addrStart),pvoid(data));
  }
  mtcp_sys_read(fd,&siz,sizeof(siz));
  mtcp_output(MTCP_INFO,"restore: mapped %d [%d] bytes restore image region at %p - %p in execution mode.\n",
              int(sys.addrSize),int(siz),data,pvoid(sys.addrStart+sys.addrSize));
  mtcp_sys_read(fd,data,sys.addrSize);
  checkMarker(fd,SYS_END_MARKER);

  // Move file descriptor to number expected afterwards and seek back to start
  fdnum = sys.checkpointFD;
  if ( fd != fdnum ) {  // Move file to the same descriptor used for writing the checkpoint!
    if ( mtcp_sys_dup2(fd,fdnum) < 0) {
      mtcp_output(MTCP_FATAL,"FileDesc: error %d [%s] restore %s fd %d to %d.\n", 
                  mtcp_sys_errno, strerror(mtcp_sys_errno), file_name, fd, fdnum);
    }
    mtcp_sys_close(fd);
  }
  fd = fdnum;
  rc = mtcp_sys_lseek(fd,0,SEEK_SET);
  if ( rc < 0 ) {
    mtcp_output(MTCP_FATAL,"restore: Failed [%d] to seek back to file begin:%s\n",mtcp_sys_errno,file_name);
  }
  if ( advise ) {
    int rc = ::posix_fadvise64(fd,0,0,POSIX_FADV_WILLNEED|POSIX_FADV_SEQUENTIAL);
    if ( 0 != rc ) {
      mtcp_output(MTCP_ERROR,"restore: fadvise error:%s\n",::strerror(errno));
    }
  }
  *func = sys.startRestore;
}
#if 0
extern "C" int __libc_start_main (int (*main) (int, char **, char **),
                                  int argc, char **argv,
                                  void (* /*init */) (void), void (* /*fini */) (void),
                                  void (* /* rtld_fini */) (void), void * stack_end)
{
  char **envp = argv + argc + 1;
  mtcp_global_stack_end = stack_end;
  int result = main (argc, argv, envp);
  mtcp_sys_exit(result);
  return result;
}
#endif
#if 0
extern "C" void
__libc_csu_init (int /* argc */, char ** /* argv */, char ** /*envp */)
{
  while(1);
}

/* This function should not be used anymore.  We run the executable's
   destructor now just like any other.  We cannot remove the function,
   though.  */
extern "C" void __libc_csu_fini (void)
{
}
#endif

static int usage() {
  mtcp_output(MTCP_ERROR,"Usage: restore -p(rint) <print-level> -i(nput) <file-name> \n"
              "       print-level = 1...5  : DEBUG,INFO,WARNING,ERROR,FATAL.         \n"
              "                              Default:%d                              \n"
              "       file-name = string   : Name of the checkpoint file.            \n"
              "       -n                   : Do not write PID in mtcp output.        \n"
              "       -e                   : Read new environment vars from stdin.   \n"
              , mtcp_get_debug_level());
  return 1;
}

/// Print data content
extern "C" char** environ;
int main(int argc, char** argv, char** /* envp */) {
  mtcp_sys_personality(ADDR_NO_RANDOMIZE|mtcp_sys_personality(0xFFFFFFFFUL));
  SysInfo::start_restore_t func = 0;
  //environ = envp;
  if ( argc > 1 ) {
    int prt = MTCP_WARNING, opts=0;
    const char* libs_dir = 0;
    {
      int advise=0, norestart=1;
      const char* file_name = 0;
      for(int i=1; i<argc; ++i) {
        if      ( argc>i && argv[i][1] == 'i' ) file_name = argv[++i];
        else if ( argc>i && argv[i][1] == 'p' ) prt   = argv[++i][0]-'0';
        else if ( argc>i && argv[i][1] == 'n' ) prt  |= MTCP_PRINT_NO_PID;
        else if ( argc>i && argv[i][1] == 'e' ) opts |= MTCP_STDIN_ENV;
        else if ( argc>i && argv[i][1] == 'a' ) advise = 1;
        else if ( argc>i && argv[i][1] == 'r' ) norestart = 0;
        else if ( argc>i && argv[i][1] == 'l' ) libs_dir = argv[++i];
        else if ( argc>i && argv[i][1] == 'd' ) ::sleep(10);
      }
      if ( 0 == file_name ) return usage();
      mtcp_set_debug_level(prt /* MTCP_ERROR prt */);
      mtcp_output(MTCP_INFO,"restore: print level:%d input:%s\n",prt,file_name);
      load(argc, argv, file_name, libs_dir, advise, norestart==1, &func);
    }
    // Now call the restore function - it shouldn't return. 
    // Checkpoint file is still open and positioned to the beginning.
    (*func)(argc,argv,environ,libs_dir,prt,opts);
    mtcp_output(MTCP_FATAL,"restore: restore routine returned (it should never do this!)\n");
    mtcp_abort();
  }
  return usage();
}
