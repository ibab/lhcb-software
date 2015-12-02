#ifndef GAUDIUPI_UPISERVICE_H
#define GAUDIUPI_UPISERVICE_H 1
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiUPI/GaudiUPI/UPIService.h,v 1.2 2008-07-04 09:37:36 frankb Exp $

#include "GaudiKernel/StateMachine.h"
#include "GaudiKernel/Service.h"
#include "CPP/Interactor.h"

class DialogMenu;
class IAppMgrUI;

/*
 *  Gaudi namespace
 */
namespace Gaudi  {

    /** @class UPIService UPIService.h GaudiUPI/UPIService.h
      *
      * Dialog class to interact with a Gaudi task
      * 
      * @author  M.Frank
      * @version 1.0
      */
  class UPIService : public Service, public CPP::Interactor  {
    /// Main dialog window
    DialogMenu* m_window;
    Interactor* m_child;
    IAppMgrUI*  m_appMgr;
  protected:
    void setNewChild(Interactor* c);
    void showTopMenu();
    /// Interactor overload: handle menu interupts
    void handle (const CPP::Event& event);
    /// Show message in the message window
    void print(const char* fmt, ...);
    /// Update state on display
    void updateState(Gaudi::StateMachine::State state);

  public:
    /// Standard Constructor.
    UPIService( const std::string& nam, ISvcLocator* svc);
    /// Destructor.
    virtual ~UPIService();
    /// Initialize the service.
    virtual StatusCode initialize();
    /// Finalise the service.
    virtual StatusCode finalize();

  };
}      // Enad namespace Gaudi
#endif // GAUDIUPI_UPISERVICE_H
