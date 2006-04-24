#ifndef GAUDIUPI_SUBMENU_H
#define GAUDIUPI_SUBMENU_H 1
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiUPI/Gaudi/SubMenu.h,v 1.1 2006-04-24 13:45:56 frankb Exp $

// C++ include files
#include <string>
#include "CPP/Interactor.h"

// Forward declarations
class DialogMenu;
class Property;
class Event;

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
  class SubMenu : public Interactor {
  public:
    enum SubMenuItems  {
      COM_CLOSE = 1000000,
      CMD_CLOSE = 1000001,
      COM_SHOW  = 2000000,
      CMD_SHOW  = 2000001
    };
  protected:
    /// Main dialog window
    DialogMenu*    m_window;
    /// Sub- dialog window
    Interactor*    m_child;
    /// Parent to be notified on window close
    Interactor*    m_parent;
    /// Current cursor position
    int            m_cursor;
  public:
    /// Initializing constructor
    SubMenu(const std::string& title, Interactor* parent);
    /// Default destructor
    virtual ~SubMenu();
    /// Map the window
    void mapWindow(bool defCommands=true);
    /// Show message in the message window
    void print(const char* fmt, ...);
    /// Interactor overload: handle menu interupts
    void handle (const Event& event);
  };
}      // End namespace Gaudi
#endif // GAUDIUPI_SUBMENU_H
