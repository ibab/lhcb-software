#ifndef CHECKPOINTING_STATIC_H
#define CHECKPOINTING_STATIC_H
#include "Checkpointing/Namespace.h"
#include "ucontext.h"

#define _ALIGN(x) x __attribute__((__packed__))

// Forward declarations
struct user_desc;

/*
 * Checkpointing namespace declaration
 */
namespace CHECKPOINTING_NAMESPACE    {

  // Forward declarations
  class Thread;

  struct Stack {
    int argc;
    char** argv;
    char** environment;
  };

  /**@struct SysInfo
   *
   * @author  M.Frank
   * @version 1.0
   */
  _ALIGN(class) SysInfo {
  public:
    enum RestartTypes {
      RESTART_NORMAL = 0,
      RESTART_CHECKPOINT = 1
    };

    typedef unsigned long mem_address_t;
    typedef void  (*start_restore_t)(int argc, char** argv, char** environ, const char* libs_dir,int print_level,int optional_flags);
    typedef void  (*end_restore_t)();

    /// Current stack limit
    unsigned long   stackLimitCurr;
    /// Hard OS dependent stack limit
    unsigned long   stackLimitHard;
    /// The start address of the checkpointing image
    mem_address_t   addrStart;
    /// The end address of the checkpointing image
    mem_address_t   addrEnd;
    /// The size of the checkpointing image
    unsigned long   addrSize;
    /// The start address of the checkpointing image
    mem_address_t   chkptStart;
    /// The size of the checkpointing image
    mem_address_t   chkptSize;
    /// The low address of the vsyscall area
    mem_address_t   vsyscallStart;
    /// The high address of the vsyscall area
    mem_address_t   vsyscallEnd;
    /// The saved end-pointer of the process heap
    void*           saved_break;

    /// The function pointer to restore the process
    start_restore_t startRestore;
    /// The function pointer to finalize the restore of the process
    end_restore_t   finishRestore;

    /// Process stack info: pointer
    mem_address_t   arg0;
    char**          argv;
    char**          envp;
    int             argc;

    mem_address_t   restore_arg0;
    int             restore_arglen;
    int             restore_argc;
    mem_address_t   restore_utgid;
    int             restore_utgidLen;

    /// Pointer to the process dependent system information (opaque)
    void*           sysInfo;
    /// Page size of the node
    unsigned long   pageSize;
    /// File descriptor for the checkpointing image to avoid clashes on restart from checkpoint
    int             checkpointFD;
    /// Pointer to the bootstraping thread
    Thread*         motherofall;
    /// The PID of the executing process
    int             motherPID;
    /// The PID of the process performing the original checkpoint (=motherPID for normal running)
    int             chkptPID;
    /// Static flag containing the processing type (0=normal running,1=start from checkpoint)
    int             restart_type;
    /// Optional flags to steer restore
    int             restart_flags;
    /// Optional flags for saving
    int             save_flags;
    /// Process stack info: argv[0]
    char*           utgid;
    /// Length of argv[0] string
    int             utgidLen;
    /// Pointer to mother's register context
    ucontext_t*     savedContext;

    /// The name/path of the checkpointing code image
    char            checkpointImage[1024];
    /// The name/path of the checkpoint file
    char            checkpointFile[1024];
    /// The name/path of the library restoration directory
    char            checkpointLibs[1024];
    /// The full argv[0] string
    char            arg0String[1024];

    int numTmpFiles;
    struct TmpFile {
      int  fd;
      char name[256];
    } tmpFiles[128];
#ifndef CHECKPOINTING_SYSINFO_STRUCT_ONLY
    /// Standard constructor
    SysInfo();
#endif
  };
}

#undef _ALIGN

#ifndef CHECKPOINTING_SYSINFO_STRUCT_ONLY
#ifndef CHECKPOINTING_STATIC_CPP
namespace CHECKPOINTING_NAMESPACE {  extern SysInfo chkpt_sys;   }

#endif
#endif

#endif // CHECKPOINTING_STATIC_H
