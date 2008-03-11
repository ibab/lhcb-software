// $Id: ROMon.h,v 1.6 2008-03-11 12:37:52 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/ROMon.h,v 1.6 2008-03-11 12:37:52 frankb Exp $
#ifndef ROMON_ROMON_H
#define ROMON_ROMON_H 1

// ++ include files
#include <ctime>
// Framework include files
#include "MBM/bmdef.h"
#include "MBM/bmstruct.h"
#include "ROMon/Collections.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class MBMClient ROMon.h ROMon/ROMon.h
   *
   * Class which represents an MBM client in the ROMon monitoring structure.
   *
   * @author M.Frank
   */
  class MBMClient {
  public:
    /// Object name
    char name[BM_USER_NAME_LEN];
    int  partitionID;
    int  processID;
    int  events;
    char reqs[4];
    char type;
    char state;
    /// 3 Byte payload padding to ensure 32 bit alignment
    char pad[2];
  public:    // Public data accessors
    /// Length of the object in bytes
    int length()  const {  return sizeof(MBMClient); }
    /// Reset object structure
    MBMClient* reset();
  };

  /**@class MBMBuffer ROMon.h ROMon/ROMon.h
   *
   * Class which represents an MBM buffer in the ROMon monitoring structure.
   *
   * @author M.Frank
   */
  class MBMBuffer {
  public:
    typedef FixItems<MBMClient> Clients;
    typedef CONTROL Control;
    /// Object name
    char    name[BM_USER_NAME_LEN];
    /// Copy of the BM control structure
    CONTROL ctrl;
    /// Variable array of fixed size clients
    Clients clients;
  public:    // Public data accessors
    /// Length of the object in bytes
    int length()  const {  return sizeof(MBMBuffer)+clients.data_length(); }
    /// Reset object structure
    MBMBuffer* reset();
  };

  /**@class FSMTask ROMon.h ROMon/ROMon.h
   *
   * Class which represents a FSM task in the ROMon monitoring structure.
   *
   * @author M.Frank
   */
  class FSMTask {
  public:
    /// Object name
    char name[BM_USER_NAME_LEN];
    /// Process identifier on node
    int  processID;
    /// Current FSM state
    char state;
    /// Target state of the transition
    char targetState;
    /// FSM meta state
    char metaState;
    /// 1 Byte payload padding to ensure 32 bit alignment
    char __pad;
    /// Time stamp of last command issued
    time_t lastCmd;
    /// Time stamp of comamnd completion
    time_t doneCmd;
  public:    // Public data accessors
    /// Default constructor
    FSMTask();
    /// Copt constructor
    FSMTask(const FSMTask& copy);
    /// Copy operator
    FSMTask& operator=(const FSMTask& copy);
    /// Length of the object in bytes
    int length()  const {  return sizeof(FSMTask); }
    /// Reset object structure
    FSMTask* reset();
  };

  /**@class Node ROMon.h ROMon/ROMon.h
   *
   * Class which represents a node with MBM Buffers and 
   * FSM tasks in the ROMon monitoring structure.
   *
   * @author M.Frank
   */
  class Node {
  public:
    typedef VarItems<MBMBuffer> Buffers;
    typedef FixItems<FSMTask>   Tasks;

    enum { TYPE = 1 };
    /// First word: Data type descriptor (MUST always be 1)
    int  type;
    /// Total size of the node information
    int  totalSize;
    /// Size of the MBM buffer information
    int  bmSize;
    /// Size of the FSM task list
    int  taskSize;
    /// Time stamp of the monitor snapshot [seconds since epoche]
    time_t time;
    /// Time stamp of the monitor snapshot [milli seconds]
    unsigned int millitm;
    /// Object name
    char name[32];

  public:    // Public data accessors
    /// Reset object structure
    Node* reset();
    /// Access to the variable length array of variable length buffers. On filling buffers MUST be first filled!
    Buffers* buffers() const;
    /// Access to the variable length array of Tasks
    Tasks*   tasks() const;
    /// Fix-up object sizes
    void fixup();
    /// Length of the object in bytes
    int length()  const {  return totalSize; }
  };

  /**@class Nodeset ROMon.h ROMon/ROMon.h
   *
   * Class which represents a set of nodes in the ROMon monitoring structure.
   *
   * @author M.Frank
   */
  class Nodeset {
  public:
    typedef VarItems<Node> Nodes;
    typedef std::pair<time_t,unsigned int> TimeStamp;
    enum { TYPE = 2 };
    /// First word: Data type descriptor (MUST always be 2)
    int  type;
    /// Object name
    char name[32];
    /// Variable size array of node items
    Nodes nodes;
    /// Reset object structure
    Nodeset* reset();
    /// Length of the object in bytes
    int length()  const {  return sizeof(Nodeset)+nodes.data_length(); }
    /// Retrieve timestamp of earliest updated node
    TimeStamp firstUpdate() const;
    /// Retrieve timestamp of most recent updated node
    TimeStamp lastUpdate() const;
  };

  typedef VarItems<Nodeset> Processorfarm;

  union ROMonData {
    char*      str;
    void*      ptr;
    MBMBuffer* buffer;
    FSMTask*   task;
    Node*      node;
    Nodeset*   nodeset;
    ROMonData(void* buffer) { ptr=buffer; }
    int type() { return this->ptr ? *(int*)this->ptr : -1; }
  };
}
#endif /* ROMON_ROMON_H */

