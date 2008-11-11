// $Id: Sys.h,v 1.1 2008-11-11 18:31:09 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/Sys.h,v 1.1 2008-11-11 18:31:09 frankb Exp $
#ifndef ONLINE_ROMON_SYS_H
#define ONLINE_ROMON_SYS_H

// Framework include files
#include "ROMon/CPUMon.h"

// C++ include files
#include <string>
#include <cstdlib>
#include <sys/param.h>

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
    int read(int pid);
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

