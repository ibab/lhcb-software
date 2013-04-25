#ifndef CHECKPOINTING_PROCESS_H
#define CHECKPOINTING_PROCESS_H

#define _ALIGN(x) x __attribute__((__packed__))

#include "Checkpointing/Namespace.h"
#include <climits>
#include <sys/stat.h>

/*
 * Checkpointing namespace declaration
 */
namespace CHECKPOINTING_NAMESPACE {

  /** @class Process
   *
   * @author  M.Frank
   * @version 1.0
   */
  class Process {
    public:
    enum Flags {
      PROCESS_READ,
      PROCESS_WRITE,
      PROCESS_RESTORE
    };
    public:
    
    /// Standard constructor
    Process() {};
    /// Write full process information to checkpoint file
    static int write(int fd);
    /// Write header information to checkpoint file
    static int writeHeader(int fd);
    /// Write trailer information to checkpoint file
    static int writeTrailer(int fd);
    /// Write system information to checkpoint file
    static int writeSysInfo(int fd);
    /// Write the file descriptor information to checkpoint file
    static int writeFiles(int fd);
    /// Write the memory areas to checkpoint file
    static int writeMemory(int fd);
    
    /// Read full process information from checkpoint file
    int read(const void* addr);
    /// Read header information from checkpoint file
    int readHeader(const void* addr);
    /// Read trailer information from checkpoint file
    int readTrailer(const void* addr);
    /// Read system information from checkpoint file
    int readSysInfo(const void* addr);
    /// Read the file descriptor information from checkpoint file
    int readFiles(const void* addr);
    /// Read the memory areas from checkpoint file
    int readMemory(const void* addr);

    /// Finalize restore process
    int restoreFinish();
  };

}

#endif // CHECKPOINTING_PROCESS_H
