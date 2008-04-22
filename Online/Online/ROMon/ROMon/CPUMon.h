// $Id: CPUMon.h,v 1.3 2008-04-22 15:22:09 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/CPUMon.h,v 1.3 2008-04-22 15:22:09 frankb Exp $
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
    /// Farm name
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

  /**@class Process CPUInfo.h ROMon/CPUInfo.h
   *
   * Class which represents a single Process (task)
   *
   * @author M.Frank
   */
  class Process {
  public:
    /// Processes UTGID
    char           utgid[64];
    /// Processes owner
    char           owner[32];
    /// CPU usage in percent
    float          cpu;
    /// Memory usage in percent
    float          mem;
    /// Size of virtual memory
    float          vsize;
    /// Size of resident memory
    float          rss;
    /// Process ID
    unsigned short pid;
    /// Parent ID
    unsigned short ppid;
    /// Number of threads
    unsigned short threads;
    /// Padding
    unsigned short pad;
    /// Empty constructor
    Process();
    /// Reset data content
    Process*      reset();
    /// Size of the object
    long   sizeOf() const {  return sizeof(Process); }
  };

  /**@class Procset ROMon.h ROMon/ROMon.h
   *
   * Class which represents all Processes in a single node
   *
   * @author M.Frank
   */
  class Procset {
  public:
    typedef FixItems<Process> Processes;
    /// Node name
    char   name[32];
    /// Time stamp of last information update
    time_t time;
    /// Time stamp of the monitor snapshot [milli seconds]
    unsigned int millitm;
    /// Variable size array of node items
    Processes processes;
    /// Default constructor
    Procset();
    /// Reset object structure
    Procset* reset();
    /// Length of the object in bytes
    int length()  const {  return sizeof(Procset)+processes.data_length(); }
  };

  /**@class ProcFarm ROMon.h ROMon/ROMon.h
   *
   * Class which represents all CPUs in a DIM_DNS_NODE
   *
   * @author M.Frank
   */
  class ProcFarm {
  public:
    typedef VarItems<Procset> Nodes;
    typedef std::pair<time_t,unsigned int> TimeStamp;
    enum { TYPE = 5 };
    /// First word: Data type descriptor (MUST always be 5)
    int   type;
    /// Total size of the node information
    int  totalSize;
    /// Date of last update
    time_t time;
    /// Farm name
    char  name[32];
    /// Variable size array of node items
    Nodes  nodes;
    /// Reset object structure
    ProcFarm* reset();
    /// Fix-up object sizes
    void fixup();
    /// Length of the object in bytes
    int length()  const {  return sizeof(ProcFarm)+nodes.data_length(); }
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
    Process*   proc;
    Procset*   processes;
    ProcFarm*  procFarm;
    CPUMonData(void* buffer) { ptr=buffer; }
    int type() { return this->ptr ? *(int*)this->ptr : -1; }
  };
}
#endif /* ROMON_RUNDB_H */

