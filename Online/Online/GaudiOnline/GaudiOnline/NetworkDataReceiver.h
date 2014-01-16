// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/NetworkDataReceiver.h,v 1.8 2009-03-26 16:44:21 frankb Exp $
//  ====================================================================
//  NetworkDataReceiver.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_GAUDIONLINE_NETWORKDATARECEIVER_H
#define ONLINE_GAUDIONLINE_NETWORKDATARECEIVER_H

#include "GaudiOnline/OnlineService.h"
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

  /** @class NetworkDataReceiver NetworkDataReceiver.h GaudiOnline/NetworkDataReceiver.h
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
  class NetworkDataReceiver : public OnlineService  {
  protected:
    struct RecvEntry  {
      std::string           name, service;
      long                  identifier;
      NetworkDataReceiver*  handler;
      char*                 buffer;
      size_t                size;
      RecvEntry() : identifier(0), handler(0), buffer(0), size(0) {}
      RecvEntry(const RecvEntry& e) : name(e.name), service(e.service), identifier(e.identifier), 
        handler(e.handler), buffer(e.buffer), size(e.size) {}
      RecvEntry(NetworkDataReceiver* h, const std::string& n, const std::string& s, long id)
      : name(n), service(s), identifier(id), handler(h), buffer(0), size(0) {}
      RecvEntry& operator=(const RecvEntry& e)  {
	if ( this != &e )  {
	  name = e.name;
	  service = e.service;
	  identifier = e.identifier;
	  handler = e.handler;
	  buffer = e.buffer;
	  size = e.size;
	}
	return *this;
      }
    };
    typedef std::vector<RecvEntry>                Receivers;
    /// Property: [Consumer] MBM buffer name
    std::string     m_buffer;
    /// Property: Require event request from source before sending
    bool            m_useEventRequests;
    /// Property: Declare event asynchronously to MBM after receiving
    bool            m_declareAsynch;
    /// Property: Additional AND mask to declare event (default: 0)
    int             m_routingMask;
    /// Property: Additional OR mask to declare event (default: 0)
    int             m_vetoMask;
    /// Monitoring item: Total number of items received
    int             m_recvReq; 
    /// Monitoring item: Total number of receive errors
    int             m_recvError;
    /// Monitoring item: Total number of bytes received from clients
    int             m_recvBytes;
    /// Monitoring item: Backlog of buffered events
    int             m_backlog;
    /// Do not issue backlog messages too often
    int             m_lastbacklog;
    /// Pointer to buffer manager service
    MEPManager*     m_mepMgr;
    /// Pointer to MBM producer, where received data should be delivered.
    MBM::Producer*  m_prod;
    /// Reference to suspendable event selector
    ISuspendable*   m_evtSelector;
    /// Internal lock to synchronize access to queue of targets with pending event requests
    lib_rtl_lock_t  m_lock;
    /// [Network Consumer] Queue of data sources
    Receivers       m_receivers;
    /// Flag track event receiving loop.
    bool            m_recvEvents;

    /// WT callback for asynchronous request rearm
    static int rearm_net_request(unsigned int facility,void* param);
    /// Retrieve receiver entry by name
    virtual RecvEntry* receiver(const std::string& nam);
    /// Rearm network request for a single event source
    virtual StatusCode rearmRequest(const RecvEntry& src);
  public:
    /// Standard algorithm constructor
    NetworkDataReceiver(const std::string& nam, ISvcLocator* pSvc);
    /// Standard Destructor
    virtual ~NetworkDataReceiver();
    /// Service overload: Finalize the object: release all allocated resources
    virtual StatusCode finalize();
    /// Service overload: Initialize the object: allocate all necessary resources
    virtual StatusCode initialize();
    /// Service overload: Start transition callback
    virtual StatusCode start();
    /// Service overload: Stop transition callback
    virtual StatusCode stop();

    /// Incident handler implemenentation: Inform that a new incident has occured
    void handle(const Incident& inc);

    /// Access to receivers queue
    Receivers& receivers()  {  return m_receivers;  }

    /// Callback on task dead notification
    virtual StatusCode taskDead(const std::string& task_name);
    /// Networking layer overload: Subscribe to network requests
    virtual StatusCode subscribeNetwork() = 0;
    /// Networking layer overload: Unsubscribe from network requests
    virtual StatusCode unsubscribeNetwork() = 0;

    /// Networking layer overload [Net consumer]: Formulate network request for a single event source
    virtual StatusCode createNetRequest(RecvEntry& src);
    /// Networking layer overload [Net consumer]: Reset event request
    virtual StatusCode resetNetRequest(const RecvEntry& src);
    /// Networking layer overload [Net consumer]: Rearm network request for a single event source
    virtual StatusCode rearmNetRequest(const RecvEntry& src) = 0;
    /// Networking layer overload [Net consumer]: Cancel network request for a single event source
    virtual void deleteNetRequest(RecvEntry& src);
    /// Networking layer overload [Net consumer]: Reset event request and insert entry into data queue of the buffer manager
    virtual StatusCode handleEventData(const std::string& src,void* buf,size_t len);
    /// Networking layer overload [Net consumer]: Reset event request and insert entry into data queue of the buffer manager
    virtual StatusCode handleEventData(const RecvEntry& entry,void* buf,size_t len);

    /// Copy event data into buffer manager
    virtual StatusCode copyEventData(void* to, void* from, size_t len);
    /// Networking layer overload [Net consumer]: Handle event data source registration
    virtual StatusCode handleSourceRequest(int clientID,const std::string& source,const std::string& svc);
    /// Networking layer overload [Net consumer]: Handle event declaration into the buffer manager
    StatusCode declareEventData(RecvEntry& entry);
  };
}       // End namespace LHCb
#endif  // ONLINE_GAUDIONLINE_NETWORKDATARECEIVER_H
