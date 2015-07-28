// $Id: CPUMon.h,v 1.14 2010-10-15 07:42:00 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/CPUMon.h,v 1.14 2010-10-15 07:42:00 frankb Exp $
#ifndef ROMON_CPUMON_H
#define ROMON_CPUMON_H 1

// C++ include files
#include <ctime>
#include <vector>
#include <string>
#include "ROMon/Collections.h"
#include "RTL/Sys.h"
#include "RTL/Pack.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  void ro_get_node_name(char* name, size_t len);

  typedef RTL::SystemMemory Memory;
  
  /**@class Memory CPUMon.h ROMon/CPUMon.h
   *
   * Class which represents the Diskspace information of a single node.
   *
   * @author M.Frank
   */
  PACK_DATA(class) Diskspace {
  public:
    /**@class Diskspace CPUMon.h ROMon/CPUMon.h
     *
     * Class which represents a CPU core in a single node
     *
     * @author M.Frank
     */
    long    blockSize;
    long    numBlocks;
    long    freeBlocks;

    /// Empty constructor
    Diskspace();
    /// Reset data content
    Diskspace* reset();
    /// Size of the object
    long sizeOf() const {  return sizeof(Diskspace); }
  };

  PACK_DATA(class) CPU {
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
  PACK_DATA(class) CPUset {
  public:
    typedef FixItems<CPU> Cores;
    /// Node name
    char         name[32];
    /// CPU Familiy name
    char         family[32];
    /// Context switch rate in [Hz] [from cpu/stat]
    float        ctxtRate;
    /// Boot time in seconds since epoch
    int          boot;
    /// Total memory in kB
    int          memory;
    /// Free memory in kB
    int          memfree;
    /// Time stamp of last information update
    int          time;
    /// Time stamp of the monitor snapshot [milli seconds]
    unsigned int millitm;
    /// Average readings
    CPU::Stat    averages;
    /// Variable size array of node items
    Cores        cores;
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
  PACK_DATA(class) CPUfarm {
  public:
    typedef VarItems<CPUset> Nodes;
    enum { TYPE = 4 };
    /// First word: Data type descriptor (MUST always be 4)
    int   type;
    /// Total size of the node information
    int  totalSize;
    /// Date of last update
    int time;
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

  /**@class Process CPUMon.h ROMon/CPUMon.h
   *
   * Class which represents a single Process (task)
   *
   * @author M.Frank
   */
  PACK_DATA(class) Process {
  public:
    /// Processes UTGID
    char           utgid[64];
    /// Processes command
    char           cmd[64];
    /// Processes owner
    char           owner[32];
    /// CPU usage in percent
    double         cpu;
    /// Memory usage in percent
    float          mem;
    /// Size of virtual memory
    float          vsize;
    /// Size of resident memory
    float          rss;
    /// Stack size of the process
    float          stack;
    /// Startup time in seconds since epoch
    int            start;
    /// Process ID
    unsigned short pid;
    /// Parent ID
    unsigned short ppid;
    /// Number of threads
    unsigned short threads;
    /// Process state
    char           state;
    /// Padding
    unsigned char pad;
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
  PACK_DATA(class) Procset {
  public:
    typedef FixItems<Process> Processes;
    /// Node name
    char   name[32];
    /// Time stamp of last information update
    int time;
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
  PACK_DATA(class) ProcFarm {
  public:
    typedef VarItems<Procset> Nodes;
    enum { TYPE = 5 };
    /// First word: Data type descriptor (MUST always be 5)
    int   type;
    /// Total size of the node information
    int  totalSize;
    /// Date of last update
    int time;
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

  /**@class NodeStats ROMon.h ROMon/ROMon.h
   *
   * Class which represents all stat counters of a given node
   * -- Memory information
   * -- CPU information
   * -- Process information
   *
   * @author M.Frank
   */
  PACK_DATA(class) NodeStats {
  public:
    enum { TYPE = 6 };
    /// First word: Data type descriptor (MUST always be 6)
    int  type;
    /// Total size of the node information
    int  totalSize;
    /// Size of the MBM buffer information
    int  cpuSize;
    /// Size of the FSM task list
    int  procSize;
    /// Time stamp of the monitor snapshot [seconds since epoche]
    int  time;
    /// Time stamp of the monitor snapshot [milli seconds]
    unsigned int millitm;
    /// Memory information
    Memory    memory;
    /// Local disk information
    Diskspace localdisk;
    /// Object name
    char      name[32];

  public:    // Public data accessors
    /// Reset object structure
    NodeStats* reset();
    /// Access to the variable length array of CPU information
    CPUset*  cpu() const;
    /// Access to the variable length array of process descriptors
    Procset* procs() const;
    /// Fix-up object sizes
    void fixup();
    /// Length of the object in bytes
    int length()  const {  return totalSize; }
  };

  /**@class Connection ROMon.h ROMon/ROMon.h
   *
   * Class which represents a single outgoing Connection from node
   *
   * @author M.Frank
   */
  PACK_DATA(class) Connection {
  public:
    char node[32];
    int  last;
    int  status;
    /// Standard constructor
    Connection();
    /// Reset data structure content
    Connection* reset();
  };

  /**@class Connectionset ROMon.h ROMon/ROMon.h
   *
   * Class which represents all outgoing ping-Connections of a single node
   *
   * @author M.Frank
   */
  PACK_DATA(class) Connectionset {
  public:
    enum { TYPE = 7 };
    typedef FixItems<Connection> Connections;
    /// First word: Data type descriptor (MUST always be 6)
    int  type;
    /// Node name
    char   name[32];
    /// Time stamp of last information update
    int time;
    /// Time stamp of the monitor snapshot [milli seconds]
    unsigned int millitm;
    /// Variable size array of node items
    Connections connections;
    /// Default constructor
    Connectionset();
    /// Reset object structure
    Connectionset* reset();
    /// Length of the object in bytes
    int length()  const {  return sizeof(Connectionset)+connections.data_length(); }
  };


  /**@class NodeSummary ROMon.h ROMon/ROMon.h
   *
   * Class which represents the summary imformation from a single node
   *
   * @author M.Frank
   */
  PACK_DATA(class) NodeSummary {
  public:
    enum { BAD=1, OK, ALIVE, DEAD };
    /// Node name
    char name[32];
    /// Update time stamp
    int  time;
    /// Overall Status
    int  state;
    /// Alive state
    int  status;
    /// Number of bad processes in this node
    int  numBadTasks;
    /// Number of bad connections from this node    
    int  numBadConnections;
    /// Total number of MB of the local disk capacity
    float diskSize;
    /// Total number of MB left of the local disk capacity
    float diskAvailible;
    /// Standard constructor
    NodeSummary(const std::string& n);
    /// Reset data structure content
    NodeSummary* reset();
  };

  /**@class SubfarmSummary ROMon.h ROMon/ROMon.h
   *
   * Class which represents all outgoing ping-Connections of a single node
   *
   * @author M.Frank
   */
  PACK_DATA(class) SubfarmSummary {
  public:
    enum { TYPE = 9 };
    typedef FixItems<NodeSummary> Nodes;
    /// First word: Data type descriptor (MUST always be 9)
    int  type;
    /// Node name
    char   name[32];
    /// Time stamp of last information update
    int time;
    /// Time stamp of the monitor snapshot [milli seconds]
    unsigned int millitm;
    /// Variable size array of node items
    Nodes nodes;
    /// Default constructor
    SubfarmSummary(const std::string& n);
    /// Reset object structure
    SubfarmSummary* reset();
    /// Length of the object in bytes
    int length()  const {  return sizeof(SubfarmSummary)+nodes.data_length(); }
  };


  /**@class SubfarmSummary ROMon.h ROMon/ROMon.h
   *
   * Class which collects run-dependent information on pending 
   * deferred HLT running.
   *
   * @author M.Frank
   */
  PACK_DATA(class) DeferredHLTStats {
  public:
    enum { TYPE = 11 };
    typedef std::pair<int,int> RunItem;
    typedef FixItems<RunItem>  Runs;
    /// First word: Data type descriptor (MUST always be 9)
    int          type;
    /// Node name
    char         name[32];
    /// Time stamp of last information update
    int          time;
    /// Time stamp of the monitor snapshot [milli seconds]
    unsigned int millitm;
    /// MEPRx Deferred state
    int          overflowState;
    /// Disk space info
    Diskspace    localdisk;
    /// Variable size array of node items
    Runs         runs;
    /// Default constructor
    DeferredHLTStats(const std::string& n);
    /// Reset object structure
    DeferredHLTStats* reset();
    /// Load data into object
    void update();
    /// Length of the object in bytes
    int length()  const {  return sizeof(DeferredHLTStats)+runs.data_length(); }
  };

  /**@class CPUfarm ROMon.h ROMon/ROMon.h
   *
   * Class which represents all CPUs in a DIM_DNS_NODE
   *
   * @author M.Frank
   */
  PACK_DATA(class) DeferredHLTSubfarmStats {
  public:
    typedef DeferredHLTStats Node;
    typedef Node::Runs       Runs;
    typedef Node::RunItem    RunItem;
    typedef VarItems<Node>   Nodes;
    enum { TYPE = 12 };
    /// First word: Data type descriptor (MUST always be 4)
    int    type;
    /// Total size of the node information
    int    totalSize;
    /// Total size of Runs object
    int    runsSize;
    /// Date of last update [seconds since epoche]
    int    time;
    /// Farm name
    char   name[32];
    /// Variable size array of run items
    Runs   runs;
    /// Reset object structure
    DeferredHLTSubfarmStats* reset();
    /// Fix-up object sizes
    void fixup();
    /// Length of the object in bytes
    int length()  const {  return totalSize; }
    /// Variable size array of node items
    VarItems<DeferredHLTStats>*  nodes() const;
    /// Retrieve timestamp of earliest updated node
    TimeStamp firstUpdate() const;
    /// Retrieve timestamp of most recent updated node
    TimeStamp lastUpdate() const;
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

  union CPUMonData {
    char*             str;
    void*             ptr;
    CPU*              cpu;
    CPUset*           cpuSet;
    CPUfarm*          farm;
    Process*          proc;
    Procset*          processes;
    Connectionset*    connections;
    SubfarmSummary*   subfarmSummary;
    ProcFarm*         procFarm;
    NodeStats*        node;
    DeferredHLTStats* hlt;
    CPUMonData(void* buffer) { ptr=buffer; }
    int type() { return this->ptr ? *(int*)this->ptr : -1; }
  };
}
#include "RTL/Unpack.h"

#include <iostream>
namespace ROMon  {
  std::ostream& operator<<(std::ostream& os, const Memory& m);
  std::ostream& operator<<(std::ostream& os, const CPU::Stat& c);
  std::ostream& operator<<(std::ostream& os, const CPU& c);
  std::ostream& operator<<(std::ostream& os, const CPUset& c);
  std::ostream& operator<<(std::ostream& os, const CPUfarm& c);
  std::ostream& operator<<(std::ostream& os, const Process& c);
  std::ostream& operator<<(std::ostream& os, const Procset& c);
  std::ostream& operator<<(std::ostream& os, const ProcFarm& c);
  std::ostream& operator<<(std::ostream& os, const NodeStats& c);
  std::ostream& operator<<(std::ostream& os, const ProcFarm& c);
  std::ostream& operator<<(std::ostream& os, const Diskspace& c);
  std::ostream& operator<<(std::ostream& os, const DeferredHLTStats& c);
  std::ostream& operator<<(std::ostream& os, const DeferredHLTSubfarmStats& c);
}

#endif /* ROMON_CPUMON_H */

