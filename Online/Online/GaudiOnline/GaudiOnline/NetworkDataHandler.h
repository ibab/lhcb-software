// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/NetworkDataHandler.h,v 1.3 2007-05-18 13:58:54 frankm Exp $
//  ====================================================================
//  NetworkDataHandler.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
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
  class MEPManager;

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
    struct Recipient  {
      std::string         name;
      long                identifier;
      NetworkDataHandler* handler;
      Recipient() : identifier(0), handler(0) {}
      Recipient(const Recipient& e) 
      : name(e.name), identifier(e.identifier), handler(e.handler) {}
      Recipient(NetworkDataHandler* h, const std::string& n, long id)
      : name(n), identifier(id), handler(h) {}
      Recipient& operator=(const Recipient& e)  {
	if ( this != &e )  {
	  name = e.name;
	  identifier = e.identifier;
	  handler = e.handler;
	}
        return *this;
      }
    };
    struct RecvEntry  : public Recipient  {
      MBM::Producer* producer;
      RecvEntry() : producer(0) {}
      RecvEntry(const RecvEntry& e) : Recipient(e), producer(e.producer) {}
      RecvEntry(NetworkDataHandler* h, const std::string& n,MBM::Producer* p)
      : Recipient(h,n,0), producer(p) {}
      RecvEntry& operator=(const RecvEntry& e)  {
	if ( this != &e )  {
	  Recipient::operator=(e);
	  producer = e.producer;
	}
        return *this;
      }
    };
    typedef std::list<Recipient>                  Recipients;
    typedef std::vector<std::string>              DataSources;
    typedef std::vector<RecvEntry>                Receivers;
    /// Property: [Consumer] MBM buffer name
    std::string     m_buffer;
    /// Property: [Consumer] data source names. If empty component is in producer mode
    DataSources     m_dataSources;
    /// Property: Buffer space [in kBytes] to be preallocated for event data.
    int             m_buffLength;
    /// Property: Flag to indicate asynchronous request ream mechanism
    bool            m_asynchRearm;
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

    /// Reference to MEP Manager
    MEPManager*     m_mepMgr;
    /// Reference to suspendable event selector
    ISuspendable*   m_evtSelector;
    /// Internal lock to synchronize access to queue of targets with pending event requests
    lib_rtl_lock_t m_lock;
    /// [Network Producer] Queue of targets with pending event requests
    Recipients      m_recipients;
    Receivers       m_receivers;

    /// WT callback for asynchronous request rearm
    static int rearm_net_request(unsigned int facility,void* param);
    /// Issue alarm message to error logger
    virtual void sendAlarm(const std::string& msg);
    /// Configure access to the event selector to suspend/resume
    StatusCode configureNetProducer();
    /// Configure access to the buffer manager to declare events
    StatusCode configureNetConsumer();
    /// Is the task a network event consumer or not ?
    bool isNetEventConsumer() const { return !m_dataSources.empty();  }
    /// Is the task a network event consumer or not ?
    bool isNetEventProducer() const { return  m_dataSources.empty();  }
    /// Access Request message content
    const std::string& requestMessage() const;
    /// Resume event access from MBM
    StatusCode resumeEvents();
    /// Resume event access from MBM
    StatusCode suspendEvents();
    /// Add request to request queue
    StatusCode addRequest(const Recipient& task);
    /// Retrieve receiver entry by name
    RecvEntry* receiver(const std::string& nam);
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
    virtual StatusCode unsubscribeNetwork() = 0;
    /// Networking layer overload: Send data to target destination
    virtual StatusCode sendData(const Recipient& tar, const void* data, size_t len) = 0;
    /// Networking layer overload: Read data from network
    virtual StatusCode readData(void* data, size_t* len) = 0;

    /// Networking layer overload [Net consumer]: Formulate network request for a single event source
    virtual StatusCode createNetRequest(RecvEntry& src);
    /// Networking layer overload [Net consumer]: Rearm event request
    virtual StatusCode rearmRequest(const RecvEntry& src);
    /// Networking layer overload [Net consumer]: Reset event request
    virtual StatusCode resetNetRequest(const RecvEntry& src);
    /// Networking layer overload [Net consumer]: Rearm event buffer space
    virtual StatusCode rearmBufferSpace(const RecvEntry& src);
    /// Networking layer overload [Net consumer]: Rearm network request for a single event source
    virtual StatusCode rearmNetRequest(const RecvEntry& src);
    /// Networking layer overload [Net consumer]: Cancel network request for a single event source
    virtual void deleteNetRequest(RecvEntry& src);
    /// Networking layer overload [Net producer]: Read request for event data from network
    virtual StatusCode readEventRequest(void* data, size_t* len) = 0;
    /// Networking layer overload [Net consumer]: Handle event declaration into the buffer manager
    virtual StatusCode handleEventData(const std::string& src, size_t len);

    /// Networking layer overload [Net producer]: Handle client request to receive event over the network
    virtual StatusCode handleEventRequest(long clientID,const std::string& source, size_t len);

    /// Write MDF record from serialization buffer
    virtual StatusCode writeBuffer(void* const /* ioDesc */, const void* data, size_t len);
    /// Need to override MDFWriter::execute: implement noop if no clients waiting.
    virtual StatusCode execute();
  };
}  // End namespace LHCb
#endif // ONLINE_GAUDIONLINE_NETWORKDATAHANDLER_H
