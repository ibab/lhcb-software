#ifndef GAUDIUPI_SERVICEMENU_H
#define GAUDIUPI_SERVICEMENU_H 1
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiUPI/GaudiUPI/ServiceMenu.h,v 1.1 2006-10-25 09:30:46 frankb Exp $

// C++ include files
#include <map>
#include "GaudiUPI/SubMenu.h"

// Forward declarations
class ISvcLocator;
class IService;
class Property;

/*
 *  Gaudi namespace
 */
namespace Gaudi  {

  /** @class ServiceMenu ServiceMenu.h GaudiUPI/ServiceMenu.h
    *
    * Dialog class to show /list all services present in an existing
    * Gaudi application
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class ServiceMenu : public SubMenu  {
  protected:
    typedef std::map<int,IService*> Lines;
    /// Pointer to service locator object
    ISvcLocator*   m_svcLoc;
    /// Map command items to services
    Lines          m_lines;

  public:
    /// Initializing constructor
    ServiceMenu(ISvcLocator* svc, CPP::Interactor* par);
    /// Default destructor
    virtual ~ServiceMenu();
    /// Interactor overload: handle menu interupts
    void handle (const CPP::Event& event);
  };
}      // End namespace Gaudi
#endif // GAUDIUPI_SERVICEMENU_H
