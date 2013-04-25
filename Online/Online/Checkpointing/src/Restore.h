#ifndef CHECKPOINTING_RESTORE_H
#define CHECKPOINTING_RESTORE_H

#include "Checkpointing/Namespace.h"
/*
 * Checkpointing namespace declaration
 */
namespace CHECKPOINTING_NAMESPACE  {

  class FileDesc;

  /// Print file descriptor information
  STATIC(void) checkpoint_file_print(int lvl, const FileDesc* d);
  /// Reopen file descriptor for process restoration
  STATIC(int) checkpoint_file_reopen(FileDesc* d);
  /// Read file descriptor information from file
  STATIC(int) checkpoint_file_fread(FileDesc* d, int fd, bool restore);
  /// Read file descriptor information from memory block
  STATIC(int) checkpoint_file_read(FileDesc* d, const void* addr, bool restore);

  class Process;
  /// Read process descriptor header from memory
  STATIC(int) checkpointing_process_read_header(Process*, const void* addr);
  /// Read process descriptor trailer from memory
  STATIC(int) checkpointing_process_read_trailer(Process*, const void* addr);
  /// Read process descriptor header from file
  STATIC(int) checkpointing_process_fread_header(Process*,int fd);
  /// Read process descriptor trailer from file
  STATIC(int) checkpointing_process_fread_trailer(Process*,int fd);
  STATIC(int) checkpointing_process_skip_sys(Process*,const void* addr);
  /// Read full file descriptor information from memory
  STATIC(int) checkpointing_process_read_files(Process*,const void* addr);

  STATIC(int) checkpointing_process_read_memory(Process*, const void* addr);
  STATIC(int) checkpointing_process_read(Process* p, const void* addr);

  /// Skip library section. We got it already....
  STATIC(int) checkpointing_process_fskip_libs(Process*, int fd);

  STATIC(int) checkpointing_process_fskip_sys(Process*,int fd);
  STATIC(int) checkpointing_process_fread_files(Process*,int fd);
  STATIC(int) checkpointing_process_fread_memory(Process*,int fd);
  STATIC(int) checkpointing_process_fread(Process* p, int fd);

  class AreaHandler;
  class Area;
  STATIC(int) checkpointing_process_map_memory(const AreaHandler* ,const Area& a, const unsigned char* data, int data_len);
  STATIC(int) checkpointing_process_fmap_memory(const AreaHandler* ,const Area& a, int fd, int data_len);
  STATIC(int) checkpointing_area_datalength(const Area* a);
  STATIC(void) checkpointing_area_print(const Area* a,int lvl,const char* opt);
  STATIC(int) checkpointing_area_read(Area* a, const void* addr, int (*map)(const AreaHandler*, const Area& a,const unsigned char*,int), const AreaHandler* handler);

  STATIC(int) checkpointing_area_fread(Area* a,int fd, int (*map)(const AreaHandler*, const Area& a,int fd,int), const AreaHandler* handler);
  STATIC(int) checkpointing_area_protection(const Area* a);
  STATIC(int) checkpointing_area_mapFlags(const Area* a);

  STATIC(int) checkpointing_area_map(const Area& a,int fd_in,const unsigned char* in,int data_len);

  class SysInfo;
  struct Stack;
  typedef void (*checkpointing_string_handler_t)(void* param, const char* s);

  /// Initialize SysInfo structure
  STATIC(void) checkpointing_sys_initialize(SysInfo* sys);
  /// Print data content of SysInfo structure
  STATIC(void) checkpointing_sys_print(const SysInfo* s);
  /// Aquire system information (for writing/initialization)
  STATIC(void) checkpointing_sys_aquire(SysInfo* s);
  /// Set save flags
  STATIC(void) checkpointing_sys_set_save_flags(SysInfo* sys, int flags);
  /// Set save flags
  STATIC(void) checkpointing_sys_set_lib_directory(SysInfo* sys, const char* dir_name);
  /// Set restart flags
  STATIC(void) checkpointing_sys_set_restart_flags(SysInfo* sys, int flags);

  /// Get program context
  //STATIC(int) checkpointing_sys_get_context();
  /// Main restart routine in checkpointing image
  STATIC(void) checkpointing_sys_restore_start(int argc, char** argv, char** environ, const char* libs_dir, int print_level,int optional_flags);
  /// Secondary restore routine. Execution starts once we jumped to the local stack.
  STATIC(void) checkpointing_sys_restore_process();
  /// Final restart routine. Execution starts once we are back on the stack of the restored process.
  STATIC(void) checkpointing_sys_restore_finish();
  /// Handle input file to set environment etc.
  STATIC(void) checkpointing_sys_process_file(int fd, void* par, checkpointing_string_handler_t handler);
  /// Initialize basic variables from stack when writing checkpoint
  STATIC(void) checkpointing_sys_init_stack(SysInfo* sys, int argc, char** argv, char** envp);
  /// Initialize basic variables from stack when restoring from checkpoint
  STATIC(void) checkpointing_sys_init_restore_stack(SysInfo* sys, int argc, char** argv, char** envp);
  /// Setup process UTGID if availible
  STATIC(int) checkpointing_sys_set_utgid(SysInfo* sys, const char* new_utgid);
  /// Force process UTGID/argv[0] if availible
  STATIC(int) checkpointing_sys_force_utgid(SysInfo* sys, const char* new_utgid);
  /// After successful restore update the process environment from file.
  STATIC(int) checkpointing_sys_set_environment(SysInfo* sys);
  /// Set the file descriptor for the checkpointing file
  STATIC(int) checkpointing_sys_set_checkpoint_file(SysInfo* sys, int fd);

  /// Print content to stack saved in context
  STATIC(void) checkpointing_print_stack(const char* comment);

  class Thread;
  /**  Thread execution wrapper
   *
   *   Simply call the object function.
   */
  STATIC(int)  checkpointing_thread_main(void *threadv);
  /** Entry point to restart thread after a fork.
   */
  STATIC(int)  checkpointing_thread_restart_main(void* arg);
  STATIC(void) checkpointing_thread_restore_tls(Thread* thr);
  STATIC(void) checkpointing_thread_restore_signals(Thread* thr);
  STATIC(void) checkpointing_thread_restore_sigactions();
  STATIC(int)  checkpointing_thread_restart(Thread* thr, int force_context);
  STATIC(Thread*) checkpointing_thread_queue();
  ///  Save signal handlers and block signal delivery
  STATIC(void) checkpointing_thread_save_signals(Thread* thr);
  /// Setup the signal handling to stop/restart threads
  STATIC(void) checkpointing_thread_setup_signals();
  /// Thread has exited - unlink it from lists and free struct
  STATIC(int) checkpointing_thread_cleanup(Thread* thr);

  STATIC(void) checkpointing_threads_unlock();
  STATIC(void) checkpointing_threads_lock();

  // Final last restore call
  STATIC(void) checkpointing_finish_restore();

  /// Save the system info of the process
  STATIC(void) checkpointing_thread_save_sys_info();

  // CHECKPOINTING_NAMESPACE::
}

/*
 * Checkpointing namespace declaration
 */
namespace CHECKPOINTING_NAMESPACE  {

  struct SysInfo;
  STATIC(long) checkpointing_sys_fwrite(int fd, const SysInfo* s);

  struct FileDesc;
  /// Write descriptor and possibly data to file identified by fileno fd_out
  STATIC(int) checkpoint_file_fwrite(const FileDesc* d, int fd_out);
  /// Write descriptor and possibly data to memory block
  STATIC(int) checkpoint_file_write(const FileDesc* d, void* address);

  /// Write header information to checkpoint file
  STATIC(int) checkpointing_process_write_header(int fd);
  /// Write trailer information to checkpoint file
  STATIC(int) checkpointing_process_write_trailer(int fd);
  /// Write full process information to checkpoint file
  STATIC(int) checkpointing_process_fwrite(int fd);

  /// Write the file descriptor information to checkpoint file
  STATIC(int) checkpointing_files_write(int fd);

  /// Collect in process information about the memory mappings
  STATIC(int) checkpointing_memory_scan(AreaHandler* handler);
  /// Write the memory areas to checkpoint file
  STATIC(int) checkpointing_memory_write(int fd);

  class Area;
  /// Write all mapped libraries to file identified by fileno fd_out
  STATIC(int) checkpointing_libs_fwrite(int fd);
  /// Write single library content to file identified by fileno fd_out
  STATIC(int) checkpointing_library_fwrite(int fd, const Area* a);


  // CHECKPOINTING_NAMESPACE::
}
#endif  // CHECKPOINTING_RESTORE_H
