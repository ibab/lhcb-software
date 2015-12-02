/*============================================================
**
**      Finite state machine implementation to control 
**      and manipulate process groups
**
**  AUTHORS:
**
**      M.Frank  CERN/LHCb
**
**==========================================================*/
#ifndef ONLINE_FINITESTATEMACHINE_CONTROLMENU_H
#define ONLINE_FINITESTATEMACHINE_CONTROLMENU_H

// Framework include files
#include "Controller/Control.h"
#include "UPI/upidef.h"

namespace UPI {

  struct ControlMenu : public FiniteStateMachine::Control  {
    /// Menu id
    int m_id;

    /// Set the cursor in the menu if attached
    virtual void set_cursor(int cmd_id,int param_id)   {
      ::upic_set_cursor(m_id,cmd_id,param_id);
    }
    /// Update rotine to replace lines
    virtual void replace_comment(int com_id, const char* text)  {
      ::upic_replace_comment(m_id,com_id,text,"");
    }
    /// Output and printing helper function
    virtual void write_message(const char* fmt,...);
    /// Add lines and commands to menu for each task
    void addTaskMenuLines(int which, const std::string& name);

  public:
    /// Standard constructor with object setup through parameters
    ControlMenu(const std::string& config, int prt);
    /// Standard destructor
    virtual ~ControlMenu();
    /// Start the controller task
    virtual void startControllerConfig(std::vector<std::string>& tasks);
    /// Start the controller task
    virtual void startControllerNoConfig(std::vector<std::string>& tasks);
    /// Display callback handler
    virtual void handle(const CPP::Event& ev);
  };   //  End class ControlMenu
}      //  End namespace 
#endif //  ONLINE_FINITESTATEMACHINE_CONTROLMENU_H


