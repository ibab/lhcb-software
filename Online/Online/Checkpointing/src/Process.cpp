#include "Checkpointing/Static.h"
#include "Checkpointing/Process.h"
#include "Restore.h"

using namespace Checkpointing;

/// Read full process information from checkpoint file
int Process::read(const void* addr) {
  return checkpointing_process_read(this,addr);
}

/// Read header information from checkpoint file
int Process::readHeader(const void* addr)   {
  return checkpointing_process_read_header(this,addr);
}

/// Read trailer information from checkpoint file
int Process::readTrailer(const void* addr)  {
  return checkpointing_process_read_trailer(this,addr);
}

/// Read system information from checkpoint file
int Process::readSysInfo(const void* addr)   {
  return checkpointing_process_skip_sys(this,addr);
}

/// Read the file descriptor information to checkpoint file
int Process::readFiles(const void* addr)   {
  return checkpointing_process_read_files(this,addr);
}

/// Read the memory areas to checkpoint file
int Process::readMemory(const void* addr)   {
  return checkpointing_process_read_memory(this,addr);
}

/// Finalize restore process
int Process::restoreFinish() {
  checkpointing_sys_restore_finish();
  return 1;
}

/// Write the memory areas to checkpoint file
int Process::writeMemory(int fd)   {
  return checkpointing_memory_write(fd);
}

/// Write the file descriptor information to checkpoint file
int Process::writeFiles(int fd)   {
  return checkpointing_files_write(fd);
}

/// Write system information to checkpoint file
int Process::writeSysInfo(int fd)   {
  return checkpointing_sys_fwrite(fd,&chkpt_sys);
}

/// Write trailer information to checkpoint file
int Process::writeTrailer(int fd)   {
  return checkpointing_process_write_trailer(fd);
}

/// Write header information to checkpoint file
int Process::writeHeader(int fd)   {
  return checkpointing_process_write_header(fd);
}

/// Write full process information to checkpoint file
int Process::write(int fd)   {
  return checkpointing_process_fwrite(fd);
}

