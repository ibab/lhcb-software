// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/NetworkDataSender.h,v 1.4 2009-05-18 11:17:28 frankb Exp $
//  ====================================================================
//  NetworkDataSender.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_GAUDIONLINE_NETWORKDATASENDER_H
#define ONLINE_GAUDIONLINE_NETWORKDATASENDER_H

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "MDF/MDFWriter.h"
#include "RTL/rtl.h"
#include <list>

// Forward declarations
class MsgStream;
class Incident;
class IIncidentSvc;

namespace MBM {  class Producer;  }

/*
 *   LHCb namespace declaration
 */
namespace LHCb  {
  // Forward declarations
  class ISuspendable;

  /** @class NetworkDataSender NetworkDataSender.h GaudiOnline/NetworkDataSender.h
    *
    *  This class handles the evacuation of events from the HLT farm to the storage layer.
    *
    *
    *  +-------------------+  +---------------------+
    *  | NetworkDataSender |  | NetworkDataReceiver |
    *  +-------------------+  +---------------------+
    *         |                           |                     Initialization:
    *         |  subscribeNetwork()       |                     
    *         +-------------------------->+  addDataSource()    Producer sends data-sink request
    *         |                           +-----+               to sink. Sink will add the data
    *         |                           |     |               source.
    *         |                           +<----+               Then data source suspends access
    *         +-----+                     |                     to MBM events by suspending the
    *         |     | suspendEvents()     |                     event selector until a requests
    *         |<----+                     |                     for an event from the data sink. 
    *         |                           |                     is received.
    *         |         rearmNetRequest() |     
    *         +<--------------------------+                     Upon request receival the sender
    *         |                           |                     resumes access to events from MBM
    *         +-----+                     |
    *         |     |  resumeEvents()     |
    *         +<----+                     |
    *         |                           |
    *  ====================================================
    *         |                           |
    *  +----->+                           |     
    *  |      |                           |                     Event loop execution:
    *  |      +-----+                     |
    *  |      |     |  resumeEvents()     |
    *  |      +<----+                     |                     When an event is received
    *  |      |                           |                     (execute is called), the data
    *  |      | execute/sendData()        |                     are sent to the sink and the
    *  |      +-------------------------->+                     access to events is again
    *  |      |                           |                     suspended until a new request
    *  |      | execute/suspendEvents()   |                     arrives (rearm by receiver)
    *  |      +-----+                     |                     and the loop continues....
    *  |      |     |                     |
    *  |      +<----+                     |
    *  |      |                           |
    *  |      |         rearmNetRequest() |     
    *  |      +<--------------------------+
    *  |      |
    *  +------+
    *
    *  @author Markus Frank
    */
  class NetworkDataSender : public MDFWriter, virtual public IIncidentListener  {
  protected:

    /** @class NetworkDataSender::Recipient NetworkDataSender.h GaudiOnline/NetworkDataSender.h
      *
      */
    struct Recipient  {
      std::string        name;
      long               identifier;
      NetworkDataSender* handler;

      Recipient() : identifier(0), handler(0) {}
      Recipient(const Recipient& e) 
      : name(e.name), identifier(e.identifier), handler(e.handler) {}
      Recipient(NetworkDataSender* h, const std::string& n, long id)
      : name(n), identifier(id), handler(h) {}
      Recipient& operator=(const Recipient& e)  {
        name = e.name;
        identifier = e.identifier;
        handler = e.handler;
        return *this;
      }
    };
    /// Definition of recipient queue
    typedef std::list<Recipient> Recipients;
    /// Defintiion of data sinks queue
    typedef std::vector<std::string> DataSinks;

    /// Property: [Producer] Data sink name
    std::string         m_target;
    /// Property: [Producer] Multiple data sinks
    DataSinks           m_sinks;
    /// Property: [Producer] Require event request from source before sending
    bool                m_useEventRequests;
    /// Property: [Producer] Allow suspending Event selector
    bool                m_allowSuspend;
    /// Property: [Sender] Invoke DAQ_PAUSE incident on error
    bool                m_pauseOnError;
    /// Property: [Sender] Delay before resending after failure
    int                 m_sendErrorDelay;
    /// Monitoring item: Total number of items sent to receiver(s)
    int                 m_sendReq;
    /// Monitoring item: Total number of send errors to receiver(s)
    int                 m_sendError;
    /// Monitoring item: Total number of bytes sent to receiver(s)
    int                 m_sendBytes;
    /// Reference to suspendable event selector
    ISuspendable*       m_evtSelector;
    /// Internal lock to synchronize access to queue of targets with pending event requests
    lib_rtl_lock_t      m_lock;
    /// [Network Producer] Queue of targets with pending event requests
    Recipients          m_recipients;
    /// Flag to indicate if events should further be processed
    int                 m_sendEvents;
    /// Reference to incident service
    IIncidentSvc*       m_incidentSvc;

    /// Issue alarm message to error logger
    virtual void sendAlarm(const std::string& msg);
    /// Resume event access from MBM
    StatusCode resumeEvents();
    /// Resume event access from MBM
    StatusCode suspendEvents();
    /// Add request to request queue
    StatusCode addRequest(const Recipient& task);

  public:
    /// Standard algorithm constructor
    NetworkDataSender(const std::string& nam, ISvcLocator* pSvc);
    /// Standard Destructor
    virtual ~NetworkDataSender();
    /// Finalize the object: release all allocated resources
    virtual StatusCode finalize();
    /// Initialize the object: allocate all necessary resources
    virtual StatusCode initialize();
    /// Incident handler implemenentation: Inform that a new incident has occured
    void handle(const Incident& inc);

    /// Callback on task dead notification
    virtual StatusCode taskDead(const std::string& task_name);
    /// Networking layer overload [Net producer]: Subscribe to network requests
    virtual StatusCode subscribeNetwork() = 0;
    /// Networking layer overload [Net producer]: Unsubscribe from network requests
    virtual StatusCode unsubscribeNetwork() = 0;
    /// Networking layer overload [Net producer]: Send data to target destination
    virtual StatusCode sendData(const Recipient& tar, const void* data, size_t len) = 0;
    /// Networking layer overload [Net producer]: Handle client request to receive event over the network
    virtual StatusCode handleEventRequest(long clientID,const std::string& source, const char* data);
    /// Networking layer overload [Net producer+consumer]: Cancel current I/O
    virtual StatusCode cancelNetwork() = 0;

    /// Write MDF record from serialization buffer
    virtual StatusCode writeBuffer(void* const /* ioDesc */, const void* data, size_t len);
    /// Need to override MDFWriter::execute: implement noop if no clients waiting.
    virtual StatusCode execute();
  };
}  // End namespace LHCb
#endif // ONLINE_GAUDIONLINE_NETWORKDATASENDER_H
