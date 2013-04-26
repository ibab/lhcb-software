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
#include "FiniteStateMachine/Slave.h"
#include "FiniteStateMachine/Machine.h"
#include "CPP/Interactor.h"
#include "RTL/rtl.h"

namespace UPI {

  typedef FiniteStateMachine::FSM     FSM;
  typedef FiniteStateMachine::Type    Type;
  typedef FiniteStateMachine::Slave   Slave;
  typedef FiniteStateMachine::Machine Machine;
  struct  CtrlSlave;

  struct ControlMenu : public Interactor  {
    /// Menu id
    int              m_id;
    int              m_numSlaves;
    lib_rtl_thread_t m_thread;
    char             m_slaveCmd[128];
    char             m_stateCmd[128];
    char             m_killCmd[16];
    char             m_errorCmd[16];
    char             m_modeCmd[32];
    char             m_partitionCmd[32];
    char             m_configCmd[32];
    char             m_runinfoCmd[32];

    Machine*         m_machine;
    CtrlSlave*       m_slave;

  public:
    struct SlaveTag {
      ControlMenu* ctrl;
      char*       line;
      int         id, svc;
      SlaveTag() : ctrl(0), line(0), id(0), svc(0) {}
      SlaveTag(ControlMenu* c, int l, char* p) : ctrl(c), line(p), id(l), svc(0) {}
      ~SlaveTag() { if ( line ) delete line; }
    };
    SlaveTag m_dim[64];
    int numSlaves() const { return m_numSlaves; }

  public:
    /// Standard constructor with object setup through parameters
    ControlMenu(const std::string& config);
    /// Standard destructor
    virtual ~ControlMenu();
    /// Start slave's IO pump
    void startSlaveIO();
    /// Stop the slave IO pump
    void stopSlaveIO();
    /// Start the controller task
    void startController();
    /// Kill the controller task
    void killController();
    /// Invoke transition on FSM machine
    void executeTransition(const std::string& st);
    /// Invoke transition on FSM machine to given targte state
    void gotoState(const std::string& state);
    /// Force local machine state
    void setMachineState(const std::string& state_name);
    /// Update the display with the new slave state
    void updateSlaveState();
    /// Update the display with the new FSM machine state
    void updateMachineState();
    /// Exit slave via DIM command
    void killSlave(int which);
    /// Send slave to state error via DIM command
    void errorSlave(int which);

    /// Update function when FSM machine completes transitions
    FSM::ErrCond update();
    /// Display callback handler
    virtual void handle(const Event& ev);
    /// Thread routine to execute slave task
    static int   ctrl_thread_routine(void* arg);
    /// DimInfo overload to process slave states
    static void  slave_handler(void* tag, void* address, int* size);
  };   //  End class ControlMenu
}      //  End namespace 
#endif //  ONLINE_FINITESTATEMACHINE_CONTROLMENU_H


