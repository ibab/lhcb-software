// $Id: SysInfo.h,v 1.4 2008-11-19 11:07:57 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/SysInfo.h,v 1.4 2008-11-19 11:07:57 frankb Exp $
#ifndef ROMON_SYSINFO_H
#define ROMON_SYSINFO_H

// Framework include files
#include "ROMon/CPUMon.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon  {

  // Forward declarations
  class CPUset;
  class NodeStats;

  class SysInfo  {
  protected:

    char**          m_cpuInfo;
    char**          m_procInfo;
    NodeStats*      m_buffer;
    size_t          m_buffLen;
    unsigned int    m_idx;
    int             m_readings;
    int             m_nBuffs;
    int             m_numCores;
    Memory          m_mem;

    /// Combine the CPU readings from 2 iterations
    int combineCPUInfo();
    /// Combine the process readings from 2 iterations
    int combineProcessInfo();

  private:
    /// Private copy constructor
    SysInfo(const SysInfo&) {}
    /// Private assignment operator
    SysInfo& operator=(const SysInfo&) { return *this; }
  public:
    /// Default constructor
    SysInfo(NodeStats* buff, size_t len, int nbuffs);
    /// Default destructor
    ~SysInfo();
    /// Initiate new reading cycle
    void       newReading()   {  ++m_idx;                                         }
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

