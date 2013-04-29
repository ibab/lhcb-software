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
#include "FiniteStateMachine/DimSlave.h"
#include "FiniteStateMachine/Machine.h"
#include "CPP/Interactor.h"
#include "RTL/rtl.h"
#include <cstring>

namespace UPI {

  typedef FiniteStateMachine::FSM        FSM;
  typedef FiniteStateMachine::Type       Type;
  typedef FiniteStateMachine::Slave      Slave;
  typedef FiniteStateMachine::Machine    Machine;
  typedef FiniteStateMachine::DimSlave   DimSlave;

  struct ControlMenu : public Interactor  {
    /// Menu id
    int              m_id;
    int              m_numSlaves;
    lib_rtl_thread_t m_thread;
    char             m_slaveCmd[128];
    char             m_stateCmd[128];
    char             m_tmoCmd[16];
    char             m_killCmd[16];
    char             m_errorCmd[16];
    char             m_modeCmd[32];
    char             m_slvTypeCmd[32];
    char             m_partitionCmd[32];
    char             m_configCmd[32];
    char             m_runinfoCmd[32];

    Machine*         m_machine;
    DimSlave*        m_slave;
    bool             m_config_exists;

  public:
    struct SlaveTag {
      ControlMenu* ctrl;
      char*       line;
      int         id, svc;
      std::string name;
      SlaveTag() : ctrl(0), line(0), id(0), svc(0), name() {}
      SlaveTag(const SlaveTag& c) :ctrl(c.ctrl),line(0),id(c.id),svc(c.svc),name(c.name) {
       line=c.line?strcpy(new char[strlen(c.line)+1],c.line):0; 
      }
      SlaveTag(ControlMenu* c, const std::string& n, int l, char* p=0) : ctrl(c), line(p), id(l), svc(0), name(n) {}
      ~SlaveTag() { 
	if ( line ) delete line; 
      }
      SlaveTag& operator=(const SlaveTag& c) { 
	ctrl=c.ctrl; 
	line=c.line?strcpy(new char[strlen(c.line)+1],c.line):0; 
	id=c.id; 
	svc=c.svc; 
	name=c.name; 
	return *this;
      }
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
    /// Start the controller task
    void startControllerConfig();
    /// Start the controller task
    void startControllerNoConfig();
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
    /// Send slave to state error via triggered timeout
    void timeoutSlave(int which);

    /// Destroy the controller task
    FSM::ErrCond destroy();
    /// Update function when FSM machine completes transitions
    FSM::ErrCond update();
    /// Display callback handler
    virtual void handle(const Event& ev);
    /// Static routine to execute the I/O Thread
    static int   ctrl_thread_routine(void* arg);
    /// DimInfo overload to process slave states
    static void  slave_handler(void* tag, void* address, int* size);
  };   //  End class ControlMenu
}      //  End namespace 
#endif //  ONLINE_FINITESTATEMACHINE_CONTROLMENU_H


