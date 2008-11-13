// $Id: SysInfo.h,v 1.2 2008-11-13 08:29:41 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/SysInfo.h,v 1.2 2008-11-13 08:29:41 frankb Exp $
#ifndef ROMON_SYSINFO_H
#define ROMON_SYSINFO_H

// Framework include files
#include "ROMon/CPUMon.h"
#include "ROMon/FMCMonListener.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon  {

  // Forward declarations
  class CPUset;
  class NodeStats;
  class ROMonInfo;
  class FMCMonListener;

  class SysInfo  {
  protected:
    typedef FMCMonListener::Descriptor DSC;
    struct TmProc {
      const char* ptr;
      int pid;
      const char *cmd, *utgid, *date, *state;
      TmProc() : ptr(0), pid(0), cmd(0), utgid(0), date(0), state(0) {}
      TmProc(const char* q, int p, const char* c, const char* u, const char* d, const char* s)
	: ptr(q), pid(p), cmd(c), utgid(u), date(d), state(s) {}
      TmProc(const TmProc& c)
	: ptr(c.ptr), pid(c.pid), cmd(c.cmd), utgid(c.utgid), date(c.date), state(c.state) {}
    };

    typedef std::map<int,TmProc> TmProcs;
    DSC             m_taskInfo;

    char**          m_cpuInfo;
    char**          m_procInfo;
    NodeStats*      m_buffer;
    size_t          m_buffLen;
    unsigned int    m_idx;
    int             m_readings;
    int             m_nBuffs;
    int             m_numCores;

    ROMonInfo*      m_dns;
    FMCMonListener* m_utgid;
    TmProcs         m_procs;
    Memory          m_mem;

    /// Read the processes with UTGID info from the DIM service
    int readTmProcs(DSC& info, TmProcs& procs);
    /// Combine the CPU readings from 2 iterations
    int combineCPUInfo();
    /// Combine the process readings from 2 iterations
    int combineProcessInfo();

  public:
    /// Default constructor
    SysInfo(NodeStats* buff, size_t len, int nbuffs);
    /// Default destructor
    ~SysInfo();
    /// Initiate new reading cycle
    void       newReading()   {  m_idx  = ++m_idx;                                }
    /// Buffer to current CPU information
    CPUset*    cpuNow()       {  return (CPUset*) m_cpuInfo[m_idx%m_nBuffs];      }
    /// Buffer to previous CPU information
    CPUset*    cpuLast()      {  return (CPUset*) m_cpuInfo[(m_idx+1)%m_nBuffs];  }
    /// Buffer to current process information
    Procset*   procsNow()     {  return (Procset*)m_procInfo[m_idx%m_nBuffs];     }
    /// Buffer to previous process information
    Procset*   procsLast()    {  return (Procset*)m_procInfo[(m_idx+1)%m_nBuffs]; }
    /// Buffer to resulting node statistics information
    NodeStats* statistics()   {  return m_buffer;                                 }

    /// Initialize the object with all static information
    int init();
    /// Update changing object data items
    int update();
  };
}      // End namespace ROMon
#endif /* ROMON_SYSINFO_H */

