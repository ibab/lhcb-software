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

  // CHECKPOINTING_NAMESPACE::
}
#endif  // CHECKPOINTING_SAVE_H
