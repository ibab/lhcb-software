#ifndef GAUDIUPI_SUBMENU_H
#define GAUDIUPI_SUBMENU_H 1
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiUPI/GaudiUPI/SubMenu.h,v 1.1 2006-10-25 09:30:46 frankb Exp $

// C++ include files
#include <string>
#include "CPP/Interactor.h"

// Forward declarations
class DialogMenu;
class Property;

/*
 *  Gaudi namespace
 */
namespace Gaudi  {

  /** @class SubMenu SubMenu.h GaudiUPI/SubMenu.h
    *
    * Dialog class to build sub menus
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class SubMenu : public CPP::Interactor {
  public:
    enum SubMenuItems  {
      COM_CLOSE = 1000000,
      CMD_CLOSE = 1000001,
      COM_SHOW  = 2000000,
      CMD_SHOW  = 2000001
    };
  protected:
    /// Main dialog window
    DialogMenu*      m_window;
    /// Parent to be notified on window close
    CPP::Interactor* m_parent;
    /// Sub- dialog window
    CPP::Interactor* m_child;
    /// Current cursor position
    int              m_cursor;
  public:
    /// Initializing constructor
    SubMenu(const std::string& title, CPP::Interactor* parent);
    /// Default destructor
    virtual ~SubMenu();
    /// Map the window
    void mapWindow(bool defCommands=true);
    /// Show message in the message window
    void print(const char* fmt, ...);
    /// Interactor overload: handle menu interupts
    void handle (const CPP::Event& event);
  };
}      // End namespace Gaudi
#endif // GAUDIUPI_SUBMENU_H
