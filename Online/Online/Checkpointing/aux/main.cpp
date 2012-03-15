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
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/personality.h>

using namespace Checkpointing;

static int checkMarker(int fd, Marker should) {
  Marker got;
  mtcp_sys_read(fd,&got,sizeof(got));
  const char* a = (char*)&got;
  const char* b = (char*)&should;
  if ( got != should ) {
    mtcp_output(MTCP_ERROR,"Seen invalid markers: Got:%d %c%c%c%c Should be:%d %c%c%c%c\n",
	     got,a[0],a[1],a[2],a[3],should,b[0],b[1],b[2],b[3]);
  }
  return 1;
}

static void load(const char* file_name, int advise, SysInfo::start_restore_t* func) {
  typedef void* pvoid;
  const Marker PROCESS_BEGIN_MARKER = *(Marker*)"PROC";
  const Marker SYS_BEGIN_MARKER     = *(Marker*)"PSYS";
  const Marker SYS_END_MARKER       = *(Marker*)"psys";
  int   rc, siz, fd, fdnum;
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
  checkMarker(fd,SYS_BEGIN_MARKER);
  mtcp_sys_read(fd,&siz,sizeof(siz));
  mtcp_output(MTCP_DEBUG,"SysInfo has a size of %d bytes\n",siz);
  mtcp_sys_read(fd,&sys,sizeof(SysInfo));

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
  mtcp_output(MTCP_INFO,"restore: mapped %d byte restore image region at %p - %p in execution mode.\n",
		 int(sys.addrSize),data,pvoid(sys.addrStart+sys.addrSize));
  mtcp_sys_read(fd,&siz,sizeof(siz));
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
int main(int argc, char** argv) {
  mtcp_sys_personality(ADDR_NO_RANDOMIZE|mtcp_sys_personality(0xFFFFFFFFUL));
  SysInfo::start_restore_t func = 0;
  if ( argc > 1 ) {
    int prt = MTCP_WARNING, opts=0;
    {
      int advise=0;
      const char* file_name = 0;
      for(int i=1; i<argc; ++i) {
	if      ( argc>i && argv[i][1] == 'i' ) file_name = argv[++i];
	else if ( argc>i && argv[i][1] == 'p' ) prt   = argv[++i][0]-'0';
	else if ( argc>i && argv[i][1] == 'n' ) prt  |= MTCP_PRINT_NO_PID;
	else if ( argc>i && argv[i][1] == 'e' ) opts |= MTCP_STDIN_ENV;
	else if ( argc>i && argv[i][1] == 'a' ) advise = 1;
      }
      if ( 0 == file_name ) return usage();
      mtcp_set_debug_level(MTCP_ERROR /* prt */);
      mtcp_output(MTCP_INFO,"restore: print level:%d input:%s\n",prt,file_name);
      load(file_name,advise,&func);
    }
    Stack stack;
    stack.argv = argv;
    stack.argc = argc;
    stack.environment = environ;
    // Now call the restore function - it shouldn't return. 
    // Checkpoint file is still open and positioned to the beginning.
    (*func)(&stack,prt,opts);
    mtcp_output(MTCP_FATAL,"restore: restore routine returned (it should never do this!)\n");
    mtcp_abort();
  }
  return usage();
}
