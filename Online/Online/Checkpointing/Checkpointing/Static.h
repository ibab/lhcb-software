#ifndef CHECKPOINTING_STATIC_H
#define CHECKPOINTING_STATIC_H
#include "Checkpointing/Namespace.h"

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
  _ALIGN(struct) SysInfo {


    enum RestartTypes {
      RESTART_NORMAL = 0,
      RESTART_CHECKPOINT = 1
    };

    typedef void  (*start_restore_t)(Stack* stack,int print_level,int optional_flags);
    typedef void  (*end_restore_t)();

    /// Current stack limit
    unsigned long   stackLimitCurr;
    /// Hard OS dependent stack limit
    unsigned long   stackLimitHard;
    /// The start address of the checkpointing image
    unsigned long   addrStart;
    /// The end address of the checkpointing image
    unsigned long   addrEnd;
    /// The size of the checkpointing image
    unsigned long   addrSize;
    /// The start address of the checkpointing image
    unsigned long   chkptStart;
    /// The size of the checkpointing image
    unsigned long   chkptSize;
    /// The low address of the vsyscall area
    unsigned long   vsyscallStart;
    /// The high address of the vsyscall area
    unsigned long   vsyscallEnd;
    /// The saved end-pointer of the process heap
    void*           saved_break;

    /// The function pointer to restore the process
    start_restore_t startRestore;
    /// The function pointer to finalize the restore of the process
    end_restore_t   finishRestore;

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
    /// Process stack info: argv[0]
    char*           arg0;
    /// Length of argv[0] string
    int             arg0Len;
    /// Process stack info: argv[0]
    char*           utgid;
    /// Length of argv[0] string
    int             utgidLen;

    long            savedSP;

    /// The name/path of the checkpointing code image
    char            checkpointImage[1024];
    /// The name/path of the checkpoint file
    char            checkpointFile[1024];

#ifndef CHECKPOINTING_SYSINFO_STRUCT_ONLY
    /// Standard constructor
    SysInfo();
    /// Set the file descriptor for the checkpointing file
    void setCheckpointFile(int fd);
    /// Aquire system information (for writing/initialization)
    void aquire();
    /// Write static system information to file
    long write(int fd);
    /// Initialize basic variables from stack
    void init_stack(Stack* s);
    /// Setup process UTGID if availible
    int setUTGID(const char* new_utgid);
    /// Force process UTGID/argv[0] if availible
    int forceUTGID(const char* new_utgid);

    /// After successful restore update the process environment from file.
    long setEnvironment();
    /// Print data content
    void print();
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
