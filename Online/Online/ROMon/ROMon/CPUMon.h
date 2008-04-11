// $Id: CPUMon.h,v 1.1 2008-04-11 12:12:36 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Structure of basic items published by the readout
//               monitoring.
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/CPUMon.h,v 1.1 2008-04-11 12:12:36 frankb Exp $
#ifndef ROMON_RUNDB_H
#define ROMON_RUNDB_H 1

// ++ include files
#include <ctime>
#include "ROMon/Collections.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class CPU CPUInfo.h ROMon/CPUInfo.h
   *
   * Class which represents a CPU core in a single node
   *
   * @author M.Frank
   */
  class CPU {
  public:
    /// Clock speed [from cpu/info]
    float          clock;
    /// Cache size [from cpu/info]
    int            cache;
    /// Bogomips estimator
    float          bogomips;
    struct Stat {
      /// Usage of CPU in user mode in [%] [from cpu/stat]
      float          user;
      /// Usage of CPU in system mode in [%] [from cpu/stat]
      float          system;
      /// CPU nice value [from cpu/stat]
      float          nice;
      /// Usage of CPU in idle mode in [%] [from cpu/stat]
      float          idle;
      /// CPU in IO wait in [%] [from cpu/stat]
      float          iowait;
      /// Interrupt rate in [%] [from cpu/stat]
      float          IRQ;
      /// Soft interrupt rate in [%] [from cpu/stat]
      float          softIRQ;
    } stats;
    /// Empty constructor
    CPU();
    /// Reset data content
    CPU*          reset();
    /// Size of the object
    long   sizeOf() const {  return sizeof(CPU); }
  };

  /**@class CPUset ROMon.h ROMon/ROMon.h
   *
   * Class which represents all CPU cores in a single node
   *
   * @author M.Frank
   */
  class CPUset {
  public:
    typedef FixItems<CPU> Cores;
    typedef std::pair<time_t,unsigned int> TimeStamp;
    /// Node name
    char   name[32];
    /// CPU Familiy name
    char   family[32];
    /// Context switch rate in [Hz] [from cpu/stat]
    float  ctxtRate;
    /// Time stamp of last information update
    time_t time;
    /// Time stamp of the monitor snapshot [milli seconds]
    unsigned int millitm;
    /// Variable size array of node items
    Cores   cores;
    /// Default constructor
    CPUset();
    /// Reset object structure
    CPUset* reset();
    /// Length of the object in bytes
    int length()  const {  return sizeof(CPUset)+cores.data_length(); }
  };


  /**@class CPUfarm ROMon.h ROMon/ROMon.h
   *
   * Class which represents all CPUs in a DIM_DNS_NODE
   *
   * @author M.Frank
   */
  class CPUfarm {
  public:
    typedef VarItems<CPUset> Nodes;
    typedef std::pair<time_t,unsigned int> TimeStamp;
    enum { TYPE = 4 };
    /// First word: Data type descriptor (MUST always be 4)
    int   type;
    /// Total size of the node information
    int  totalSize;
    /// Date of last update
    time_t time;
    /// Node name
    char  name[32];
    /// Variable size array of node items
    Nodes  nodes;
    /// Reset object structure
    CPUfarm* reset();
    /// Fix-up object sizes
    void fixup();
    /// Length of the object in bytes
    int length()  const {  return sizeof(CPUfarm)+nodes.data_length(); }
    /// Retrieve timestamp of earliest updated node
    TimeStamp firstUpdate() const;
    /// Retrieve timestamp of most recent updated node
    TimeStamp lastUpdate() const;
  };

  union CPUMonData {
    char*      str;
    void*      ptr;
    CPU*       cpu;
    CPUset*    cpuSet;
    CPUfarm*   farm;
    CPUMonData(void* buffer) { ptr=buffer; }
    int type() { return this->ptr ? *(int*)this->ptr : -1; }
  };
}
#endif /* ROMON_RUNDB_H */

