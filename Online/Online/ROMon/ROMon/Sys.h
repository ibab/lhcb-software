// $Id: Sys.h,v 1.4 2010-10-15 07:42:00 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Author     : M.Frank
//  Created    : 29/10/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/Sys.h,v 1.4 2010-10-15 07:42:00 frankb Exp $
#ifndef ONLINE_ROMON_SYS_H
#define ONLINE_ROMON_SYS_H

// Framework include files
#include "ROMon/CPUMon.h"

// C++ include files
#include <vector>
#include <string>
#include <cstdlib>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  struct SysFile  {
    std::string m_name;
    SysFile(const char* name) : m_name(name) {}
    ~SysFile() {}
    int read(char* buffer, size_t len) const;
  };

  struct SystemUptime {
    float uptime;
    float idletime;
    /// Default constructor
    SystemUptime() {}
    /// Read system data from proc file system
    int read();
    /// Print system data from proc file system
    void print() const;
  };

  struct EnvironProcess {
    typedef std::pair<std::string,std::string> Variable;
    typedef std::vector<Variable> Environ;
    Environ env;
    /// Default constructor
    EnvironProcess() {}
    /// Read system data from proc file system
    int read(int proc_id);
    /// Print system data from proc file system
    void print() const;
  };
  struct UtgidProcess {
    std::string utgid;
    /// Default constructor
    UtgidProcess() {}
    /// Read system data from proc file system
    int read(int proc_id);
    /// Print system data from proc file system
    void print() const;
  };
  struct StatusProcess {
    char   comm[400];
    char   state;
    int    sleepAvg;
    int    tgid;
    int    pid;
    int    ppid;
    int    uid;
    int    gid;
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
    long unsigned staBrk;
    long unsigned brk;
    long unsigned staStk;
    int           nThreads;
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
    /// Read process data from proc file system
    int read(int proc_id);
    /// Print process data from proc file system
    void print() const;
  };

  struct SysProcess {
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
    /// Read process data from proc file system
    int read(int pid);
    /// Print process data from proc file system
    void print() const;
  };
  
  /// Read current CPU information prom proc file system
  int readInfo(CPUset& data, size_t max_len);
  /// Read current CPU statistics
  int readStat(CPUset& data, size_t max_len, size_t num_cores);
  /// Combined read of both blocks
  int read(CPUset& data, size_t max_len);
  /// Read memory information block
  int read(Memory& memory);
  /// Read process table on current node
  int read(Procset& procset, size_t max_len);
}
#endif  // ONLINE_ROMON_SYS_H

