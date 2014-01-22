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

  /** @class EventRequestServerRunable EventRequestServerRunable.h GaudiOnline/EventRequestServerRunable.h
    *
    * Runable to serv independent network clients with event
    * data. requests are purely served on demand ie. each
    * client has to ask for every event to be processed.
    * 
    * Contrary to the base class only events the request type 
    * of the server requirement is served. Clients can only get 
    * events without specifying special trigger masks etc.
    *
    *  @author Markus Frank
    *  @version 1.0
    */
  class EventRequestServerRunable : public EventServerRunable  {
  public:
    /// Standard Constructor
    EventRequestServerRunable(const std::string& nam, ISvcLocator* svcLoc);
    /// Standard Destructor
    virtual ~EventRequestServerRunable();
    /// IService implementation: start the service
    virtual StatusCode start();

    /// Rescan client tables, reformulate possibly pending requests and renew the request
    virtual void restartRequests();
    /// Send event data to a list of waiting clients
    virtual StatusCode sendEvent();
  };
}      // End namespace LHCb
#endif // GAUDISVC_EVENTREQUESTSERVERRUNABLE_H
