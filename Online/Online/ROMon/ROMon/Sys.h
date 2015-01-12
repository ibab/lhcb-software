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
#include "RTL/Sys.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  // These classes were once implemented in this package, 
  // but moved to OnlineKernel.
  // Hence only declare the usage of them....
  using RTL::SysFile;
  using RTL::SystemUptime;
  using RTL::EnvironProcess;
  using RTL::UtgidProcess;
  using RTL::StatusProcess;
  using RTL::SysProcess;

  /// Read current CPU information prom proc file system
  int readInfo(CPUset& data, size_t max_len);
  /// Read current CPU statistics
  int readStat(CPUset& data, size_t max_len, size_t num_cores);
  /// Combined read of both blocks
  int read(CPUset& data, size_t max_len);
  /// Read process table on current node
  int read(Procset& procset, size_t max_len);
}
#endif  // ONLINE_ROMON_SYS_H

