#ifndef CHECKPOINTING_PROCESS_H
#define CHECKPOINTING_PROCESS_H

#define _ALIGN(x) x __attribute__((__packed__))

#include <climits>
#include <sys/stat.h>

/*
 * CheckPointing namespace declaration
 */
namespace CheckPointing {

  /** @class Process
   *
   * @author  M.Frank
   * @version 1.0
   */
  class Process {
    mutable long m_length;
    int          m_flag;
  public:
    enum Flags {
      PROCESS_READ,
      PROCESS_WRITE,
      PROCESS_RESTORE
    };
  public:

    /// Standard constructor
    Process(int f=PROCESS_READ) : m_length(0), m_flag(f) {}
    /// Calculate the length of the data to be stored
    long length() const;
    void setLength(long len) {     m_length = len; }
    /// Write full process information to checkpoint file
    int write(void* addr)  const;
    /// Write header information to checkpoint file
    int writeHeader(void* addr)  const;
    /// Write trailer information to checkpoint file
    int writeTrailer(void* addr)  const;
    /// Write system information to checkpoint file
    int writeSysInfo(void* addr)  const;
    /// Write the file descriptor information to checkpoint file
    int writeFiles(void* addr)  const;
    /// Write the memory areas to checkpoint file
    int writeMemory(void* addr)  const;
    
    /// Read full process information from checkpoint file
    int read(const void* addr)  const;
    /// Read header information from checkpoint file
    int readHeader(const void* addr)  const;
    /// Read trailer information from checkpoint file
    int readTrailer(const void* addr)  const;
    /// Read system information from checkpoint file
    int readSysInfo(const void* addr)  const;
    /// Read the file descriptor information from checkpoint file
    int readFiles(const void* addr)  const;
    /// Read the memory areas from checkpoint file
    int readMemory(const void* addr)  const;
  };

}

#endif // CHECKPOINTING_PROCESS_H
