// $Id: EventServerRunable.h,v 1.7 2008-10-14 08:37:21 frankb Exp $
//====================================================================
//  EventServerRunable
//--------------------------------------------------------------------
//
//  Package    : GaudiOnline
//
//  Description: Runable to serv independent network clients with event
//               data. requests are purely served on demand ie. each
//               client has to ask for every event to be processed.
//
//  Author     : M.Frank
//  Created    : 4/12/2007
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/EventServerRunable.h,v 1.7 2008-10-14 08:37:21 frankb Exp $
#ifndef GAUDISVC_EVENTSERVERRUNABLE_H
#define GAUDISVC_EVENTSERVERRUNABLE_H 1

#include <map>

// Framework includes
#include "GaudiKernel/IRunable.h"
#include "GaudiOnline/OnlineService.h"
#include "NET/DataTransfer.h"
#include "MBM/bmdef.h"
#include "MBM/Consumer.h"
#include "MBM/Requirement.h"
#include "RTL/rtl.h"

// Forward declarations
class IIncidentSvc;

/*
 *   LHCb namespace declaration
 */
namespace LHCb  {

  class MEPManager;

  /** @class EventServerRunable EventServerRunable.h GaudiOnline/EventServerRunable.h
    *
    * Runable to serve independent network clients with event
    * data. requests are purely served on demand ie. each
    * client has to ask for every event to be processed.
    *
    *  @author Markus Frank
    *  @version 1.0
    */
  class EventServerRunable : public OnlineService, virtual public IRunable  {

  protected:
    enum RunableStates { WAIT_REQ, WAIT_EVT, ACTION_EVT, DONE };
    /// Request entry definition
    typedef std::pair<MBM::Requirement,DataTransfer::netentry_t*> ReqEntry;
    /// Definition of the container containing waiting clients
#ifdef EventServerRunable_USE_MAP
    typedef std::map<std::string, ReqEntry > Recipients;
#else
    typedef std::pair<std::string,ReqEntry> Recipient;
    typedef std::list<Recipient> Recipients;
#endif
    /// Reference to MEP manager service
    MEPManager*          m_mepMgr;
    /// Property: MEP manager service name/type
    std::string          m_mepMgrName;
    /// Number of events to be processed
    int                  m_evtMax;
    /// Monitoring quantity: Number of events processed
    int                  m_evtCount;
    /// Reference to MBM consumer to access event data
    MBM::Consumer*       m_consumer;
    /// Reference to network plug to send events
    DataTransfer::NET*   m_netPlug;
    /// Container of currently waiting event receivers
    Recipients           m_recipients;
    /// Event flag to wake up the server when a new request arrives
    lib_rtl_event_t      m_suspend;
    /// Lock to protect internal data structures
    lib_rtl_lock_t       m_lock;
    /// Lock to protect internal data structures
    lib_rtl_lock_t       m_rcpLock;
    /// String containing node_name::UTGID of event data provider task
    std::string          m_input;
    /// String representation of basic request (trigger and veto masks will be taken from requestors)
    std::string          m_req;
    /// Parses requirement data buffer
    MBM::Requirement     m_request;
    /// State variable of the consumer obejct
    int                  m_consState;
    /// Property: Print delivery after each m_printNum events sent
    int                  m_printNum;
    /// Property: Number of thread instances used by the network library
    int                  m_nThreads;

  public:
    /// Standard Constructor
    EventServerRunable(const std::string& nam, ISvcLocator* svcLoc);
    /// Standard Destructor
    virtual ~EventServerRunable();
    // IInterface implementation : queryInterface
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
    /// IService implementation: initialize the service
    virtual StatusCode initialize();
    /// IService implementation: start the service
    virtual StatusCode start();
    /// IService implementation: stop the service
    virtual StatusCode stop();
    /// IService implementation: finalize the service
    virtual StatusCode finalize();
    /// Incident handler implemenentation: Inform that a new incident has occured
    virtual void handle(const Incident& inc);
    /// IRunable implementation : Run the class implementation
    virtual StatusCode run();
    /// Remove a given target process from the list of pending clients
    virtual void removeTarget(const std::string& src);
    /// Callback to handle a new request of a client to receive a given event
    virtual void handleEventRequest(const DataTransfer::netheader_t& hdr, void* ptr);
    /// Rescan client tables, reformulate possibly pending requests and renew the request
    virtual void restartRequests();
    /// Send event data to a list of waiting clients
    virtual StatusCode sendEvent();
    /// Shutdown all event requests from the network
    virtual void shutdownRequests();
    /// Accessor the netplu object
    DataTransfer::NET* netPlug() const {  return m_netPlug; }
  };
}      // End namespace LHCb
#endif // GAUDISVC_EVENTSERVERRUNABLE_H
