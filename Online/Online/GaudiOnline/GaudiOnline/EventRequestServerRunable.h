// $Id: EventRequestServerRunable.h,v 1.2 2008-10-06 11:49:19 frankb Exp $
//====================================================================
//  EventRequestServerRunable
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/EventRequestServerRunable.h,v 1.2 2008-10-06 11:49:19 frankb Exp $
#ifndef GAUDISVC_EVENTREQUESTSERVERRUNABLE_H
#define GAUDISVC_EVENTREQUESTSERVERRUNABLE_H 1

// Framework includes
#include "GaudiOnline/EventServerRunable.h"

/*
 *   LHCb namespace declaration
 */
namespace LHCb  {

  /** Class definition of EventRequestServerRunable.
    *
    * Runable to serv independent network clients with event
    * data. requests are purely served on demand ie. each
    * client has to ask for every event to be processed.
    *
    *  @author Markus Frank
    *  @version 1.0
    */
  class EventRequestServerRunable : public EventServerRunable  {
    /// Consistemncy check counter for events sent
    int   m_sendCount;
    /// Consistemncy check counter for events retrieved
    int   m_mbmCount;
    /// Get next client from queue
    DataTransfer::netentry_t* getClient();
  public:
    /// Standard Constructor
    EventRequestServerRunable(const std::string& nam, ISvcLocator* svcLoc);
    /// Standard Destructor
    virtual ~EventRequestServerRunable();
    /// IService implementation: initialize the service
    virtual StatusCode initialize();
    /// IService implementation: finalize the service
    virtual StatusCode finalize();

    /// Rescan client tables, reformulate possibly pending requests and renew the request
    virtual void restartRequests();
    /// Send event data to a list of waiting clients
    virtual StatusCode sendEvent();
    /// IRunable implementation : Run the class implementation
    virtual StatusCode run();
  };
}      // End namespace LHCb
#endif // GAUDISVC_EVENTREQUESTSERVERRUNABLE_H
