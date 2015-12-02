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
#ifndef ONLINE_CONTROLLER_CONTROL_H
#define ONLINE_CONTROLLER_CONTROL_H

// Framework include files
#include "FiniteStateMachine/Machine.h"
#include "Controller/DimSlave.h"
#include "CPP/Interactor.h"
#include "RTL/rtl.h"

// C/C++ include files
#include <cstring>


/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine {

  /// Forward declarations
  class Machine;

  enum  _commands {
    CMD_CONFIG_FSM = 1,
    CMD_START_SLAVEIO,
    CMD_STOP_SLAVEIO,
    CMD_SLAVE_RESET,

    CMD_SET_SLAVETYPE,
    CMD_SET_PARTITION,
    CMD_SET_RUNINFO,
    CMD_SET_CONFIG,
    CMD_SET_MODE,

    CMD_KILL_SLAVE,
    CMD_KILL_SLAVE_END = CMD_KILL_SLAVE+20,
 
    CMD_KILL_CTRL,
    CMD_SEND_CTRL,
    CMD_SETSTATE_CTRL,
    CMD_NUM_SLAVES,
    CMD_CLOSE,

    CMD_WRITE_MESSAGE,
    CMD_UPDATE_SLAVE,
    CMD_UPDATE_MACHINE,

    COM_MACH_STATE,
    COM_MACH_METASTATE,
    COM_CTRL_STATE,
    COM_CTRL_METASTATE,

    COM_SLAVE_STATE,

    CMD_LAST = 2000
  };
  struct Control : public CPP::Interactor  {
  public:

  protected:
    int              m_numSlaves;
    int              m_print;
    lib_rtl_thread_t m_thread;
    char             m_slaveCmd[128];
    char             m_stateCmd[128];
    char             m_anyCmd[32];
    char             m_modeCmd[32];
    char             m_slvTypeCmd[32];
    char             m_partitionCmd[32];
    char             m_configCmd[32];
    char             m_runinfoCmd[32];
    char             m_tmoCmd[16];
    char             m_killCmd[16];
    char             m_pauseCmd[16];
    char             m_errorCmd[16];

    Machine*         m_machine;
    DimSlave*        m_slave;
    bool             m_config_exists;
    std::string      m_currWD;

  public:
    struct SlaveTag {
      Control*    ctrl;
      char*       line;
      int         id, svc;
      std::string name;
    SlaveTag() : ctrl(0), line(0), id(0), svc(0), name() {}
    SlaveTag(const SlaveTag& c) :ctrl(c.ctrl),line(0),id(c.id),svc(c.svc),name(c.name) {
      line=c.line?strcpy(new char[strlen(c.line)+1],c.line):0; 
    }
    SlaveTag(Control* c, const std::string& n, int l, char* p=0) : ctrl(c), line(p), id(l), svc(0), name(n) {}
      ~SlaveTag() { 
	if ( line ) delete line; 
      }
      SlaveTag& operator=(const SlaveTag& c) { 
	if ( this != &c )  {
	  size_t l = c.line ? strlen(c.line)+1 : 0;
	  ctrl = c.ctrl; 
	  if ( line ) delete line;
	  line = c.line ? strncpy(new char[l],c.line,l) : 0; 
	  id   = c.id; 
	  svc  = c.svc; 
	  name = c.name; 
	}
	return *this;
      }
    };
    SlaveTag m_dim[64];
    int numSlaves() const { return m_numSlaves; }

    /// Output and printing helper function
    virtual void write_message(const char* fmt,...);
    /// Set the cursor in the menu if attached
    virtual void set_cursor(int,int) {}
    /// Update rotine to replace lines
    virtual void replace_comment(int, const char*) {}
  public:
    /// Standard constructor with object setup through parameters
    Control(const std::string& config, int prt);
    /// Standard destructor
    virtual ~Control();
    /// Start slave's IO pump
    virtual void startSlaveIO();
    /// Stop the slave IO pump
    virtual void stopSlaveIO();
    /// Start the controller task
    virtual void startController();
    /// Start the controller task
    virtual void startControllerConfig(std::vector<std::string>& tasks);
    /// Start the controller task
    virtual void startControllerNoConfig(std::vector<std::string>& tasks);
    /// Kill the controller task
    virtual void killController();
    /// Invoke transition on FSM machine
    virtual void executeTransition(const std::string& st);
    /// Invoke transition on FSM machine to given targte state
    virtual void gotoState(const std::string& state);
    /// Force local machine state
    virtual void setMachineState(const std::string& state_name);
    /// Update the display with the new slave state
    virtual void updateSlaveState();
    /// Update the display with the new FSM machine state
    virtual void updateMachineState();
    /// Send a command to one of the slave's data points
    void execDimCommand(const std::string& dp, const std::string& data);

    /// Destroy the controller task
    FSM::ErrCond destroy();
    /// Update function when FSM machine completes transitions
    FSM::ErrCond update();
    /// Display callback handler
    virtual void handle(const CPP::Event& ev);
    /// Static routine to execute the I/O Thread
    static int   ctrl_thread_routine(void* arg);
    /// DimInfo overload to process slave states
    static void  slave_handler(void* tag, void* address, int* size);
  };   //  End class Control
}      //  End namespace 
#endif //  ONLINE_FINITESTATEMACHINE_CONTROL_H


