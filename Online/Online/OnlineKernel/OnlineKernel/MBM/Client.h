// $Id:$
//==========================================================================
//  LHCb Online software suite
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see OnlineSys/LICENSE.
//
// Author     : M.Frank
//
//==========================================================================
#ifndef ONLINE_ONLINEKERNEL_MBM_CLIENT_H
#define ONLINE_ONLINEKERNEL_MBM_CLIENT_H

#include "MBM/bmdef.h"
#include <string>

///  MBM (Multi-Buffer-Manager) namespace declaration
namespace MBM {

  /// The Desriptor of one event received from the MBM
  /** 
   *   Every readout function is separated into independent tasks running 
   *   asynchronously in order to derandomize the flow of events. The producer 
   *   task is responsible for reading/assembling data from data sources. 
   *
   *   As soon as the read operation is finished, the data block is declared 
   *   to a managed shared memory area, the buffer manager (BM) and the 
   *   producer is ready for the next read operation.
   *
   *   The consumer task is activated each time an event is declared in the BM. 
   *   The consumer is responsible for releasing the space occupied as 
   *   soon as the consumer has finished processing. Thus, the two activities 
   *   of reading and processing can proceed asynchronously, provided there is 
   *   always sufficient space in the buffer to accommodate at least one read 
   *   operation. 
   *
   *   A BM is managed by a dedicated task executing on each node, which creates 
   *   and initializes the shared memory area. The synchronization between the 
   *   various participating processes has been realized using shared semaphores. 
   *   Any task using the BM registers itself with a name and a partition identifier.
   *   Consumers will only receive data declared by producers with the same 
   *   partition identifier and a boolean accept and veto mask of a width of 128 bits, 
   *   which allows consumers to be specific in the type of data they wish to 
   *   receive. Consumers may request data from the BM in three different ways:
   *   \li The consumer sees all data declared to the BM according to the request mask.
   *   \li A group of consumers see the data exactly once. This mode allows 
   *       executing multiple instances of an application and taking advantage 
   *       of multiple CPU cores.
   *   \li A consumer is served on a best effort basis. In this mode producers 
   *       are always able to declare new data. Possibly pending data, which 
   *       have to be released to satisfy the space requests, are not seen by 
   *       these consumers.
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  class EventDesc {
  public:
    int* data;
    int  len;
    int  type;
    unsigned int  mask[4];
    EventDesc() : data(0), len(0), type(0) {
      mask[0] = mask[1] = mask[2] = mask[3] = 0;
    }
  };

  /// Generic client bas class (Consumer and Producer)
  /** 
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  class Client {
  protected:
    /// Buffer identifier
    BMID         m_bmid;
    /// Buffer name
    std::string  m_buffName;
    /// Process name
    std::string  m_name;
    /// Partition ID
    int          m_partID;
    /// Facility code for asynchronous operation
    unsigned int m_facility;
    /// Flag to run in blocking mode
    bool         m_blocking;
    /// The event descriptor
    EventDesc    m_event;

  public:
    /// Initializing constructor
    Client(const std::string& buffer_name, const std::string& client_name, int partition_id, int com_type);

    /// Initializing constructor
    Client(BMID bmid, const std::string& client_name, int partition_id);

    /// Standard destructor
    virtual ~Client();

    /// Access to process id
    static int pid();

    /// Buffer identifier
    BMID  id()  const {
      return m_bmid;
    }

    // Access to processstart address of the buffer
    const char* bufferAddress() const;

    /// Buffer name
    const std::string& bufferName()  const {
      return m_buffName;
    }

    /// Client name
    const std::string& name() const {
      return m_name;
    }

    /// Partition ID
    int partitionID() const {
      return m_partID;
    }

    /// Include MBM client into buffer
    virtual int include(int communication_type);

    /// Exclude MBM client into buffer
    virtual int exclude();

    // Clean receive fifo
    virtual int clear();

    /// Switch to non-blocking asynchronous execution mode using WT
    virtual void setNonBlocking(int facility, bool subscribe);

    /// Run the application with WT asynchronously 
    virtual int run();

    /// Run the application with WT asynchronously 
    virtual int runAsynchronous();

    /// Run the application with synchronously 
    virtual int runSynchronous();

    /// Switch to blocking synchronous execution mode
    virtual void setBlocking();
  };
}
#endif  // ONLINE_ONLINEKERNEL_MBM_CLIENT_H
