// $Id: Sys.h,v 1.4 2010-10-15 07:42:00 frankb Exp $
//====================================================================
//  OnlineBase
//--------------------------------------------------------------------
//
//  Package    : Online/OnlineBase
//
//  Author     : M.Frank
//  Created    : 29/10/2008
//
//====================================================================
// $Header: $
#ifndef ONLINE_ONLINEBASE_RTL_SYS_H
#define ONLINE_ONLINEBASE_RTL_SYS_H

// C++ include files
#include <iostream>
#include <vector>
#include <string>
#include <set>

// Framework include files
#include "RTL/Pack.h"

///  ROMon namespace declaration
namespace RTL {

  /// Class which represents the memory information of a single node.
  /**
   *
   * Class which represents the memory information of a single node.
   * Note: All numbers are in KB!
   *
   * \author  M.Frank
   * \version 1.0
   */
  PACK_DATA(class) SystemMemory {
  public:
    int memTotal;     // 1
    int memFree;      // 2
    int buffers;      // 3
    int cached;       // 4
    int swapCached;   // 5
    int active;       // 6
    int inactive;     // 7
    int highTotal;    // 8
    int highFree;     // 9
    int lowTotal;     // 10
    int lowFree;      // 11
    int swapTotal;    // 12
    int swapFree;     // 13
    int dirty;        // 14
    int writeback;    // 15
    int mapped;       // 16
    int slab;         // 17
    int commitLimit;  // 18
    int committed_AS; // 19
    int pageTables;   // 20
    int vmallocTotal; // 21
    int vmallocUsed;  // 22
    int vmallocChunk; // 23
    int _last;        // 24 ... padding
    /// Empty constructor
    SystemMemory();
    /// Reset data content
    SystemMemory* reset();
    /// Size of the object
    long   sizeOf() const {  return sizeof(SystemMemory); }
    /// Print system memory data
    void print() const;
  };

  /// Class which represents the Diskspace information of a single node.
  /**
   *
   * \author  M.Frank
   * \version 1.0
   */
  PACK_DATA(class) SystemDiskspace {
  public:
    long    blockSize;
    long    numBlocks;
    long    freeBlocks;

    /// Empty constructor
    SystemDiskspace();
    /// Reset data content
    SystemDiskspace* reset();
    /// Size of the object
    long sizeOf() const {  return sizeof(SystemDiskspace); }
  };

  /// Class which represents the CPU information of one core in a single node.
  /**
   *
   * \author  M.Frank
   * \version 1.0
   */
  PACK_DATA(class) SystemCPU {
  public:
    /// Processor ID in box
    unsigned short processor;
    /// Total number of CPU siblings (hyperthreads)
    unsigned short numSibling;
    /// Total number of cores in this slot
    unsigned char  numCore;
    /// CPU slot number (physical ID)
    unsigned char  physID;
    /// CPU core ID on slot
    unsigned char  coreID;
    /// CPU family identifier
    unsigned char  family;
    
    /// Cache size [from cpu/info] in kB
    int            cache;
    /// Clock speed [from cpu/info]
    float          clock;
    /// Bogomips estimator
    float          bogomips;

    /// Empty constructor
    SystemCPU();
    /// Reset data content
    SystemCPU* reset();
    /// Print CPU core data
    void print() const;
    /// Size of the object
    long sizeOf() const {  return sizeof(SystemCPU); }
  };

  std::vector<std::string> psItems(const char*& ptr);
  std::vector<std::string> psLabels();
  enum PsDataItems {
    UTGID=0, 
    TID, 
    TGID, 
    PPID, 
    PGID, 
    NLWP, 
    PROC_USER_NAME,
    PROC_GROUP_NAME,
    TTY,
    CMD,
    SCHED,
    PRIO,
    RTPRIO,
    NICE,
    PSR,
    STAT,
    PERCENT_CPU,
    PERCENT_MEMORY,
    VIRTUAL_SIZE,
    LOCK,
    RESIDENT_MEMORY,
    DATA_MEMORY,
    STACK_MEMORY,
    EXECUTABLE,
    LIB,
    SHARE,
    IGNORED_SIGNALS,
    PENDING_SIGNALS,
    CATCHED_SIGNALS,
    BLOCKED_SIGNALS,
    START_TIME,
    ELAPSED_TIME,
    CPU_TIME,
    CMDLINE
  };

  /// Class to simplyfy reading data from /proc file system
  /**
   *
   * \author  M.Frank
   * \version 1.0
   */
  class SysFile  {
  public:
    class FileDescriptor    {
      /// File handle
      int m_fd;
      
    public:
      /// Initializing constructor
      FileDescriptor(int value) : m_fd(value)      {}
      /// Default destructor. Non-virtuality is intended. Do not inherit!
      ~FileDescriptor();
      /// Access file handle
      int get() const      {   return m_fd;   }
    };
    
      
  public:
    /// File name
    std::string m_name;
    /// Initializing constructor
    SysFile(const char* name) : m_name(name) {}
    /// Default destructor. Non-virtuality is intended. Do not inherit!
    ~SysFile() {}
    /// Read buffer from file in  one go
    int read(char* buffer, size_t len) const;
    /// Write buffer in one go to file
    int write(char* buffer, size_t len, int flags) const;
  };

  /// Class to store system uptime information
  /**
   *
   * \author  M.Frank
   * \version 1.0
   */
  PACK_DATA(class) SystemUptime {
  public:
    float uptime;
    float idletime;
    /// Default constructor
    SystemUptime() {}
    /// Reset data content
    SystemUptime* reset();
    /// Print system data from proc file system
    void print() const;
  };

  /// Class to store the environment variables of a process
  /**
   *
   * \author  M.Frank
   * \version 1.0
   */
  class EnvironProcess {
  public:
    typedef std::pair<std::string,std::string> Variable;
    typedef std::vector<Variable> Environ;
    Environ env;
    /// Default constructor
    EnvironProcess() {}
    /// Print system data from proc file system
    void print() const;
  };

  /// Class to store the UTGID of a process
  /**
   *
   * \author  M.Frank
   * \version 1.0
   */
  class UtgidProcess {
  public:
    std::string utgid;
    /// Default constructor
    UtgidProcess() {}
    /// Print system data from proc file system
    void print() const;
  };

  /// Class to store the status information of a process from /proc/<pid>/status
  /**
   *
   * \author  M.Frank
   * \version 1.0
   */
  PACK_DATA(class) StatusProcess {
  public:
    char   comm[399];
    char   state;
    int    sleepAvg;
    int    tgid;
    int    pid;
    int    ppid;
    int    uid;
    int    gid;
    int    utrace;
    int    fdSize;
    int    vmPeak;
    int    vmSize;
    int    vmLock;
    int    vmHWM;
    int    vmRSS;
    int    vmData;
    int    vmStack;
    int    vmExe;
    int    vmLib;
    int    vmPTE;
    int    vmSwap;

    int           nThreads;

    long unsigned staBrk;
    long unsigned brk;
    long unsigned staStk;
    //int           nThreads;
    long unsigned sigPend;
    long unsigned shdPend;
    long unsigned sigBlk;
    long unsigned sigIgn;
    long unsigned sigCgt;
    long unsigned capInh;
    long unsigned capPrm;
    long unsigned capEff;
    /// Default constructor
    StatusProcess() {}
    /// Reset data content
    StatusProcess* reset();
    /// Print process data from proc file system
    void print() const;
  };

  /// Class to store the status information of a process from /proc/<pid>/stat
  /**
   *
   * \author  M.Frank
   * \version 1.0
   */
  PACK_DATA(class) SysProcess {
  public:
    int pid; // %d
    char comm[400]; // %s
    char state; // %c
    int ppid; // %d
    int pgrp; // %d
    int session; // %d
    int tty; // %d
    int tpgid; // %d
    unsigned long flags; // %u
    unsigned long minflt; // %u
    unsigned long cminflt; // %u
    unsigned long majflt; // %u
    unsigned long cmajflt; // %u
    unsigned long utime; // %d
    unsigned long stime; // %d
    unsigned long cutime; // %d
    unsigned long cstime; // %d
    long priority; // %d
    long nice; // %d
    long num_threads;
    unsigned long itrealvalue; // %u
    unsigned long starttime; // %d
    unsigned long vsize; // %u
    long rss; // %u
    unsigned long rlim; // %u
    unsigned long startcode; // %u
    unsigned long endcode; // %u
    unsigned long startstack; // %u
    unsigned long kstkesp; // %u
    unsigned long kstkeip; // %u
    unsigned long signal; // %d
    unsigned long blocked; // %d
    unsigned long sigignore; // %d
    unsigned long sigcatch; // %d
    unsigned long wchan; // %u
    /// Default constructor
    SysProcess() {}
    /// Reset data content
    SysProcess* reset();
    /// Print process data from proc file system
    void print() const;
  };
  
  /// Read system data from proc file system: memory information block
  int read(SystemMemory& memory);
  /// Read system data from proc file system: current CPU information /proc/cpuinfo
  int read(std::vector<SystemCPU>& data);
  /// Read system data from proc file system: system uptime information /proc/uptime
  int read(SystemUptime& uptime);
  /// Read system data from proc file system: process environment /proc/<pid>/environ
  int read(EnvironProcess& env, int proc_id);
  /// Read system data from proc file system: process UTGID from  /proc/<pid>/environ
  int read(UtgidProcess& env, int proc_id);
  /// Read system data from proc file system: process statistics /proc/<pid>/status
  int read(StatusProcess& env, int proc_id);
  /// Read system data from proc file system: process statistics /proc/<pid>/stat
  int read(SysProcess& env, int proc_id);

  enum ExtractCPUInfoCommand  { 
    SYSTEMCPU_EXTRACT_CPUSLOTS,
    SYSTEMCPU_EXTRACT_PROCESSORS,
    SYSTEMCPU_EXTRACT_CPUCORES,
    SYSTEMCPU_PRINT
  };

  /// Class to extract information from the vector of CPU entries
  /**
   *
   * \author  M.Frank
   * \version 1.0
   */
  template <int I> class ExtractCPUInfo  {
  public:
    int select_slot;
    std::set<int> result;
    ExtractCPUInfo(int sel_slot=-1) : select_slot(sel_slot) {}
    void operator()(const SystemCPU& cpu);
  };
  template <int I> inline std::ostream& operator<<(std::ostream& o, const ExtractCPUInfo<I>& e)  {
    o << e.select_slot << " [ ";
    for(std::set<int>::const_iterator it=e.result.begin(); it!=e.result.end();++it) o << (*it) << " ";
    return o << "]";
  }

  /// Retrive the physical CPU IDs of a node from the SystemCPU blocks
  std::set<int> physical_cpu_ids(const std::vector<SystemCPU>& data);
  /// Retrive the core IDs of a node from the SystemCPU blocks
  std::set<int> cpucore_ids(const std::vector<SystemCPU>& data, int cpu_slot=-1);
  /// Retrive the processor IDs of a node from the SystemCPU blocks
  std::set<int> processor_ids(const std::vector<SystemCPU>& data, int cpu_slot=-1);
}
#include "RTL/Unpack.h"

#define _PROCFILE_NAME(len,name)                        \
  namespace { const char* fn_process_##name (int pid) { \
    static char txt[len];                               \
    ::snprintf(txt,sizeof(txt),"/proc/%d/"#name,pid);   \
    return txt; }}

#endif  // ONLINE_ONLINEBASE_RTL_SYS_H

