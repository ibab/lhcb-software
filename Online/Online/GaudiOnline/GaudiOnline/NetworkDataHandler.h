// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/NetworkDataHandler.h,v 1.1 2006-12-07 09:36:07 frankb Exp $
//	====================================================================
//  NetworkDataHandler.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef ONLINE_GAUDIONLINE_NETWORKDATAHANDLER_H
#define ONLINE_GAUDIONLINE_NETWORKDATAHANDLER_H

#include "MDF/MDFWriter.h"
#include "GaudiKernel/MsgStream.h"
#include "RTL/rtl.h"
#include <list>

// Forward declarations
class Incident;
namespace MBM {  class Producer;  }

/*
 *   LHCb namespace declaration
 */
namespace LHCb  {
  // Forward declarations
  class ISuspendable;

  /** @class NetworkDataHandler NetworkDataHandler.h GaudiOnline/NetworkDataHandler.h
    *
    *  This class handles - depending on the setup - all work to be done in
    *  order to transfer data over the network.
    *  The algorithm implements a request scheme, which allows clients to
    *  request a single event, which will be served on a first-in-first-derved basis.
    *
    *  This partial implementation handles the riquired functionality, but does not
    *  implement the actual networking capabilities. An suitable network messaging
    *  facility, which is name based may be used for a complete implementation.
    *
    *  The protocol works as follows:
    *
    *      +----------+                 +----------+       +-----+
    *      | Consumer |                 | Producer |       | MBM |
    *      +----------+                 +----------+       +-----+
    *           |                             |               |
    *           |     Get space               |               |
    *           +-------------------------------------------->|
    *           |                             |               |
    *           |     Request for event       |               |
    *           +---------------------------->|  Get event    |
    *           |                             +-------------->|
    *           |     Send event data         |               |
    *           |<----------------------------+               |
    *           |                             |               |
    *           |     Get space               |               |
    *           +-------------------------------------------->|
    *           |                             |               |
    *           |     Declare event/Send space|               |
    *           +-------------------------------------------->|
    *           |                             |               |
    *           |                             |               |
    *
    * Note:
    * 1) The consumer executes on the monitoring farm.
    *    Consumer means "consumer of network data"
    * 2) The producer executes on the storage layer.
    *    The producer produces data to the network.
    *
    * It is _always_ the consumer who is responsible for the
    * data request. The the producer dies, it's the consumer
    * which must ask for the data _again_.
    *
    *  @author Markus Frank
    */
  class NetworkDataHandler : public MDFWriter  {
  protected:
    typedef std::list<std::string>   Recipients;
    typedef std::vector<std::string> DataSources;
    /// Reference to suspendable event selector
    ISuspendable*   m_evtSelector;
    lib_rtl_lock_t m_lock;

    /// Property: Send target names
    Recipients      m_recipients;
    /// Property: printout frequence
    float           m_freq;
    /// Pointer to MBM producer
    MBM::Producer*  m_prod;
    /// Partition ID
    int             m_partitionID;
    /// MBM buffer name
    std::string     m_buffer, m_bm_name;
    /// Process name
    std::string     m_procName;
    /// Consumer implementation: data source name. If empty component is in producer mode
    DataSources     m_dataSources;
    /// Flag to indicate if a partitioned buffer should be connected
    bool            m_partitionBuffer;
    /// Monitoring item: Total number of items sent to receiver(s)
    int             m_sendReq;
    /// Monitoring item: Total number of send errors to receiver(s)
    int             m_sendError;
    /// Monitoring item: Total number of bytes sent to receiver(s)
    int             m_sendBytes;
    /// Monitoring item: Total number of items received
    int             m_recvReq; 
    /// Monitoring item: Total number of receive errors
    int             m_recvError;
    /// Monitoring item: Total number of bytes received from clients
    int             m_recvBytes;
    /// Pointer to member function for receive data action
    StatusCode  (NetworkDataHandler::*m_net_handler)(size_t);

    /// Helper for output messages
    mutable MsgStream*  m_logger;

    /// Helper for output messages
    MsgStream& output() const     {  return *m_logger;                }
    MsgStream& debug() const      {  return output() << MSG::DEBUG;   }
    MsgStream& info() const       {  return output() << MSG::INFO;    }
    MsgStream& error() const      {  return output() << MSG::ERROR;   }
    MsgStream& warning() const    {  return output() << MSG::WARNING; }

    /// Issue alarm message to error logger
    virtual void sendAlarm(const std::string& msg);
    /// Rearm network request for a single source
    StatusCode rearmNetRequest(const std::string& src);
    /// Configure access to the event selector to suspend/resume
    StatusCode configureSelector();
    /// Configure access to the buffer manager to declare events
    StatusCode configureBuffer();

  public:
    /// Standard algorithm constructor
    NetworkDataHandler(const std::string& nam, ISvcLocator* pSvc);
    /// Standard Destructor
    virtual ~NetworkDataHandler();
    /// Finalize the object: release all allocated resources
    virtual StatusCode finalize();
    /// Initialize the object: allocate all necessary resources
    virtual StatusCode initialize();

    /// Incident handler implemenentation: Inform that a new incident has occured
    void handle(const Incident& inc);
    /// Callback on task dead notification
    virtual StatusCode taskDead(const std::string& task_name);

    /// Networking layer overload: Subscribe to network requests
    virtual StatusCode subscribeNetwork()    
    {  return StatusCode::FAILURE;                                  }
    /// Networking layer overload: Unsubscribe from network requests
    virtual StatusCode unsubscribeNetwork()
    {  return StatusCode::FAILURE;                                  }
    /// Networking layer overload: Send data to target destination
    virtual StatusCode sendData(const std::string& tar, const void* data, size_t len)
    {  return StatusCode::FAILURE;                                  }
    /// Networking layer overload: Read data from network
    virtual StatusCode readData(void* data, size_t* len, char* source)
    {  return StatusCode::FAILURE;                                  }

    /// Producer implementation: Handle client request to receive event over the network
    virtual StatusCode handleEventRequest(size_t len);
    /// Consumer implementation: Request event
    virtual StatusCode requestEvents();
    /// Consumer implementation: Handle event declaration into the buffer manager
    virtual StatusCode handleEventData(size_t len);
    /// Write MDF record from serialization buffer
    virtual StatusCode writeBuffer(void* const /* ioDesc */, const void* data, size_t len);
    /// Need to override MDFWriter::execute: implement noop if no clients waiting.
    virtual StatusCode execute();
  };
}  // End namespace LHCb
#endif // ONLINE_GAUDIONLINE_NETWORKDATAHANDLER_H
