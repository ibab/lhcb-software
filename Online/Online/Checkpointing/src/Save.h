#ifndef CHECKPOINTING_SAVE_H
#define CHECKPOINTING_SAVE_H

#include "Checkpointing/Namespace.h"
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

  // CHECKPOINTING_NAMESPACE::
}
#endif  // CHECKPOINTING_SAVE_H
