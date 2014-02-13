// $Id: EventServerRunable.h,v 1.7 2008-10-14 08:37:21 frankb Exp $
//====================================================================
//  MooreTestSvc
//--------------------------------------------------------------------
//
//  Description: Runable to run Moore standalone on a single node.
//  Author     : M.Frank
//====================================================================
// $Header: $
#ifndef MOORESTANDALONE_MOOREMONITORSVC_H
#define MOORESTANDALONE_MOOREMONITORSVC_H

// Framework include files
#include "GaudiOnline/OnlineService.h"
#include "CPP/Interactor.h"
#include "Internals.h"

/*
 *   LHCb namespace declaration
 */
namespace LHCb {

  /** @class MooreMonitorSvc MooreMonitorSvc.h MooresStandalone/MooreMonitorSvc....h
   *
   *   Class definition of MooreMonitorSvc.
   *   An instance of this class acts as a data-hub to various displays
   *
   *
   *  @author Markus Frank
   *  @version 1.0
   */
  class MooreMonitorSvc : public OnlineService, public Interactor {
    typedef std::vector<Interactor*> Clients;
    typedef std::map<std::string,MooreTest::UserStats> Statistics;
    Statistics m_data;
    Clients m_clients;

  public:
    /// Initializing constructor
    MooreMonitorSvc(const std::string& nam, ISvcLocator* loc);
    /// Standard destructor
    virtual ~MooreMonitorSvc();

    /// IService implementation: initialize the service
    virtual StatusCode initialize();
    /// IService implementation: finalize the service
    virtual StatusCode finalize();
    /// Inform that a new incident has occurred
    virtual void handle(const Incident& incident);
    /// Interactor handler
    virtual void handle (const Event& event);
  };
}
#endif // MOORESTANDALONE_MOOREMONITORSVC_H
