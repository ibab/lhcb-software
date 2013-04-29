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

// Framework include files
#include "Controller/Control.h"
#include "Controller/FmcSlave.h"
#include "Controller/NativeDimSlave.h"

#include "FiniteStateMachine/Machine.h"
#include "FiniteStateMachine/FSMTypes.h"
#include "CPP/IocSensor.h"

// C/C++ include files
#include <boost/assign/std/vector.hpp>
#include <cstring>



/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine {

  using namespace boost::assign;

  IocSensor& ioc() {  return IocSensor::instance();  }

  struct CtrlNativeSlave : public NativeDimSlave {
    int m_out[2];
    std::string utgid;
    Control* handler;

    /// Standard constructor
    CtrlNativeSlave(Control* handler,
		    Machine* machine, 
		    const Type* typ, 
		    const std::string& nam, 
		    const std::string& args="")
      : NativeDimSlave(typ,nam,machine), handler(handler) 
    {
      m_killCmd = "destroy";
      cloneEnv();
      m_cmd   = gentest_path();
      if ( args.empty() )  {      
	m_argv += nam,"libController.so","controller_fsm_test","-name="+nam;
      }
      else  {
	m_argv += nam,"libController.so","fsm_ctrl","-type=NativeDimSlave";
	addArgs(args);
      }
      utgid = "UTGID="+name();
    }
    /// Standard destructor
    virtual ~CtrlNativeSlave() {
    }
    /// Start slave process
    FSM::ErrCond start()  {
      if ( m_pid == 0 )  {
	std::string proc = RTL::processName();
	std::vector<char*> argv, envp;
	char text[64];
	int ret = 0;

	::pipe(m_out);
	handler->startSlaveIO();
	display(ALWAYS,"%s> Slave is forking child process.....",c_name());
	::snprintf(text,sizeof(text),"-slaves=%d",handler->numSlaves());

	for(size_t i=0; i<m_argv.size();++i)
	  argv.push_back((char*)m_argv[i].c_str());
	argv.push_back(text);
	argv.push_back(0);

	for(size_t i=0; i<m_envp.size();++i)  {
	  if ( m_envp[i].substr(0,5)=="UTGID" ) continue;
	  envp.push_back((char*)m_envp[i].c_str());
	}
	envp.push_back((char*)utgid.c_str());
	envp.push_back(0);
	switch((m_pid = ::fork()))  {
	case -1:  // Error
	  return FSM::FAIL;
	case 0:   // Child
	  ::close(m_out[0]);               // close reading end in the child
	  ::dup2(m_out[1],STDOUT_FILENO);  // send stdout to the pipe
	  ::dup2(m_out[1],STDERR_FILENO);  // send stderr to the pipe
	  ::close(m_out[1]);               // this descriptor is no longer needed
	  ret = ::execve(m_cmd.c_str(),&argv[0],&envp[0]);
	  if ( ret != 0 )  {
	  }
	  return FSM::SUCCESS;
	default:
	  ::close(m_out[1]);               // close the write end of the pipe in the parent
	  return FSM::WAIT_ACTION;
	}
      }
      return FSM::SUCCESS;
    }
    /// IOC handler
    void handleIoc(const Event& event)   {
      DimSlave::handleIoc(event);
      switch(event.type)  {
      case SLAVE_LIMBO:	
	break;
      case SLAVE_STARTING:
      case SLAVE_KILLED:
      case SLAVE_FINISHED:
      case SLAVE_TRANSITION_TIMEOUT:
	break;
      case SLAVE_FAILED:
      case SLAVE_TRANSITION:
	ioc().send(handler,SLAVE_TRANSITION,this);
	break;
      case SLAVE_ALIVE:
	ioc().send(this,SLAVE_FINISHED,event.data);
	return;
      default:
	break;
      }
      ioc().send(handler,CMD_UPDATE_SLAVE,this);
    }
    int processIO() {
      FILE* f = ::fdopen(m_out[0],"r");
      char* line = 0;
      size_t len = 0;
      while( ::getline(&line,&len,f) != -1 )   {
	ioc().send(handler,CMD_WRITE_MESSAGE,::strdup(line));
      }
      ::free(line);
      ::fclose(f);
      ioc().send(handler,CMD_WRITE_MESSAGE,::strdup("Slave died! controlling thread exiting....."));
      ioc().send(handler,CMD_STOP_SLAVEIO,handler);
      return 1;
    }
    /// Handle state updates for a particular slave
    virtual void handleState(const std::string& msg)  {
      ioc().send(handler,CMD_WRITE_MESSAGE,::strdup((name()+"> Received new message:"+msg).c_str()));
      if ( msg != DAQ::ST_NAME_UNKNOWN ) NativeDimSlave::handleState(msg);
    }
  };

  struct CtrlFmcSlave : public FmcSlave {
    Control* handler;
    /// Standard constructor
    CtrlFmcSlave(Control* h, Machine* machine, const Type* typ, const std::string& nam, const std::string& args="") 
      : FmcSlave(typ,nam,machine,false), handler(h)    {
      m_killCmd = "destroy";
      setArgs(args);
    }
    /// Standard destructor
    virtual ~CtrlFmcSlave()       {  }
    /// Nothing to do here
    int processIO()    {
      return 1;
    }
    /// IOC handler
    void handleIoc(const Event& event)   {
      DimSlave::handleIoc(event);
      switch(event.type)  {
      case SLAVE_TRANSITION:
	ioc().send(handler,SLAVE_TRANSITION,this);
	ioc().send(handler,CMD_UPDATE_SLAVE,this);
	break;
      case SLAVE_ALIVE:
	ioc().send(this,SLAVE_FINISHED,event.data);
	ioc().send(handler,CMD_UPDATE_SLAVE,this);
	return;

      case SLAVE_LIMBO:
      case SLAVE_STARTING:
      case SLAVE_FAILED:
      case SLAVE_KILLED:
      case SLAVE_FINISHED:
      case SLAVE_TRANSITION_TIMEOUT:
      default:
	ioc().send(handler,CMD_UPDATE_SLAVE,this);
	break;
      }
    }
  };
}

#include "FiniteStateMachine/Type.h"
#include "Controller/XmlTaskConfiguration.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/strdef.h"
extern "C" {
#include "dis.hxx"
#include "dic.hxx"
}

// C/C++ include files
#include <string>
#include <sstream>
#include <stdexcept>
#include <unistd.h>

using namespace std;
using namespace FiniteStateMachine;
using namespace FiniteStateMachine::DAQ;

static void clean_str(char* n,size_t len)  {
  n[len-1] = 0;
  if ( ::strchr(n,' ') ) *::strchr(n,' ') = 0;
}
static void print_routine(void* param, const char* text) {
  Control* c = (Control*)param;
  char* t = (char*)text;
  *(t+strlen(text)-1) = 0;
  ioc().send(c,CMD_WRITE_MESSAGE,::strdup(text));
}

/// Standard constructor with object setup through parameters
Control::Control(const std::string& config) 
  : m_numSlaves(3), m_thread(0), m_machine(0), m_slave(0)
{
  char wd[PATH_MAX];
  TypedObject::setPrinter(this,print_routine);
  m_config_exists = 0 == ::access(config.c_str(),R_OK);
  if ( 0 == ::getcwd(wd,sizeof(wd)) )  {
    TypedObject::display(TypedObject::ALWAYS,"CANNOT retrieve current working directory: %s",RTL::errorString());
    throw runtime_error("CANNOT retrieve current working directory");
  }
  m_currWD = wd;
}

/// Standard destructor
Control::~Control()  {
  Type* type = m_machine ? const_cast<Type*>(m_machine->type()) : 0;
  if ( m_machine ) delete m_machine;
  if ( m_slave ) delete m_slave;
  if ( type ) delete type;
  m_machine = 0;
  m_slave = 0;
}

/// DimInfo overload to process slave states
void Control::slave_handler(void* tag, void* addr, int* size) {
  if ( tag ) {
    char text[1024];
    SlaveTag*   s = *(SlaveTag**)tag;
    const char* m = (size && *size>0) ? (const char*)addr : "OFFLINE - slave is DEAD";
    ::snprintf(text,sizeof(text),"         State:%s",m);
    ioc().send(s->ctrl,COM_SLAVE_STATE,new pair<int,string>(s->id,text));
  }
}

void Control::write_message(const char* fmt,...)   {
  va_list args;                                // - the log level
  va_start( args, fmt);                        // - a standard C formatted 
  char str[4096];                              //   string (like printf)
  size_t il=0, len = vsnprintf(str+il,sizeof(str)-il-2,fmt,args);
  va_end (args);
  str[il+len]   = '\n';
  str[il+len+1] = '\0';
  ::printf(str);
}

/// Static routine to execute the I/O Thread
int Control::ctrl_thread_routine(void* arg)  {
  CtrlNativeSlave* s = (CtrlNativeSlave*)arg;
  return s->processIO();
}

/// Update function when FSM machine completes transitions
FSM::ErrCond Control::update()      {
  //ioc().send(this,CMD_UPDATE_MACHINE,this);
  updateSlaveState();
  return FSM::SUCCESS;
}

/// Destroy the controller task
FSM::ErrCond Control::destroy()   {
  if ( m_slave ) m_slave->kill();
  ioc().send(this,CMD_UPDATE_SLAVE,this);
  return FSM::SUCCESS;
}


/// Update the display with the new FSM machine state
void Control::updateMachineState()   {
  char text[256];
  ::snprintf(text,sizeof(text),"FSM Machine State:%s",m_machine->c_state());
  replace_comment(COM_MACH_STATE,text);
  ::snprintf(text,sizeof(text),"FSM Machine Meta: %s",m_machine->currentMetaName());
  replace_comment(COM_MACH_METASTATE,text);
}

/// Update the display with the new slave state
void Control::updateSlaveState()   {
  char text[256];
  ::snprintf(text,sizeof(text),"CTRL FSM State:   %s",m_slave->c_state());
  replace_comment(COM_CTRL_STATE,text);
  ::snprintf(text,sizeof(text),"CTRL FSM Meta:    %s",m_slave->metaStateName());
  replace_comment(COM_CTRL_METASTATE,text);
  updateMachineState();
}

/// Start the controller task
void Control::startController()   {
  Type* typ = fsm_type("DAQSteer");
  const Transition* trans = typ->transition(ST_NAME_OFFLINE,ST_NAME_UNKNOWN);
  m_machine = new Machine(typ,RTL::processName()+"::DAQ");
  m_machine->setCompletionAction(Callback::make(this,&Control::update));
  m_machine->setFailAction(Callback::make(this,&Control::update));
  m_machine->setAction(trans,Callback::make(this,&Control::destroy));
  m_machine->setHandler(this);
  if ( m_config_exists )
    startControllerConfig();
  else
    startControllerNoConfig();
  m_machine->addSlave(m_slave);
  ioc().send(this,CMD_UPDATE_SLAVE,this);
}

/// Start the controller task
void Control::startControllerNoConfig()   {
  string node = RTL::str_upper(RTL::nodeNameShort());
  Type* daq = fsm_type("DAQ");
  char text[256];

  if ( string(m_slvTypeCmd) == "NativeDimSlave" )   {
    m_slave = new CtrlNativeSlave(this,m_machine,daq,"CTRL_0");
  }
  else  {
    CtrlFmcSlave* slave = new CtrlFmcSlave(this,m_machine,daq,"CTRL_0");
    slave->setFmcArgs("-g onliners -n online -w "+m_currWD+" -O /tmp/logSrv.fifo -E /tmp/logSrv.fifo");
    slave->setCommand("runFarmCtrl.sh");
    m_slave = slave;
  }
  for(int i=0; i<m_numSlaves; ++i)  {
    ::snprintf(text,sizeof(text),"SLAVE_%d",i);
    m_dim[i] = SlaveTag(this,text,CMD_KILL_SLAVE+2*i+1);
    ::snprintf(text,sizeof(text),"SLAVE_%d/status",i);
    m_dim[i].svc = ::dic_info_service(text,MONITORED,0,0,0,slave_handler,(long)&m_dim[i],0,0);   
  }
}

/// Start the controller task
void Control::startControllerConfig()   {
  string node = RTL::str_upper(RTL::nodeNameShort());
  Type* daq = fsm_type("DAQ");
  stringstream args, nam;

  nam.str("");
  nam << m_partitionCmd << "_" << node << "_CTRL_0";
  args.str("");
  args << "-count=" << m_numSlaves 
       << " -runinfo=bla.py"
       << " -taskconfig=" << m_configCmd
       << " -partition=" << m_partitionCmd
       << " -mode=normal";
  if ( string(m_slvTypeCmd) == "NativeDimSlave" )   {
    m_slave = new CtrlNativeSlave(this,m_machine,daq,nam.str(),args.str());
  }
  else  {
    args << " -type=FmcSlave";
    CtrlFmcSlave* slave = new CtrlFmcSlave(this,m_machine,daq,nam.str(),args.str());
    slave->setFmcArgs("-g onliners -n online -w "+m_currWD+" -O /tmp/logSrv.fifo -E /tmp/logSrv.fifo");
    slave->setCommand("runFarmCtrl.sh");
    m_slave = slave;
  }
  XmlTaskConfiguration cfg(m_partitionCmd,m_configCmd,m_runinfoCmd,m_modeCmd,m_numSlaves);
  vector<string> tasks;
  if ( cfg.getTasks(tasks) )  {
    char text[256];
    for(size_t i=0; i<tasks.size(); ++i)  {
      ::snprintf(text,sizeof(text),"%s/status",tasks[i].c_str());
      m_dim[i] = SlaveTag(this,tasks[i],CMD_KILL_SLAVE+2*i+1,0);
      m_dim[i].svc = ::dic_info_service(text,MONITORED,0,0,0,slave_handler,(long)&m_dim[i],0,0);   
    }
  }
}

/// Start the slave IO pump
void Control::startSlaveIO()   {
  if ( 0 == m_thread )  {
    int ret = ::lib_rtl_start_thread(ctrl_thread_routine,m_slave,&m_thread);
    if ( lib_rtl_is_success(ret) )  {
      write_message("Started the IO thread....");
      return;
    }
    write_message("Failed to start the IO thread: %s",RTL::errorString());
  }
}

/// Stop the slave IO pump
void Control::stopSlaveIO()   {
  if ( 0 != m_thread )  {
    ::lib_rtl_delete_thread(m_thread);
    m_thread = 0;
    write_message("Stopped the slave IO pump....");
  }
}

/// Kill the controller task
void Control::killController()   {
  if ( m_slave )  {
    write_message("Killing the controller task....");
    m_slave->kill();
  }
}

/// Invoke transition on FSM machine
void Control::executeTransition(const string& transition)   {
  const State*      st = m_machine->state();
  const Transition* tr = st->findTransByName(transition);
  if ( !tr )  {
    write_message("%s> There is no valid transition %s from state %s",
		  m_machine->c_name(),transition.c_str(),st->c_name());
    return;
  }
  m_machine->invokeTransition(tr);
  ioc().send(this,CMD_UPDATE_SLAVE,this);
}

/// Invoke transition on FSM machine to given targte state
void Control::gotoState(const string& state)   {
  const State*      st = m_machine->state();
  const Transition* tr = st->findTrans(state);
  if ( !tr )  {
    write_message("%s> There is no valid transition from state %s to state %s",
		  m_machine->c_name(), st->c_name(), state.c_str());
    return;
  }
  m_machine->invokeTransition(tr,Rule::SLAVE2MASTER);
  ioc().send(this,CMD_UPDATE_SLAVE,this);
}

/// Force local machine state
void Control::setMachineState(const string& state_name)   {
  const Type*  type  = m_machine->type();
  const State* state = type->state(state_name);
  if ( !state )  {
    write_message("%s> There is no valid state %s in FSM type %s",
		  m_machine->c_name(),state_name.c_str(),type->c_name());
    return;
  }
  m_machine->setState(state).setTarget(0);
  ioc().send(this,CMD_UPDATE_SLAVE,this);
}

/// Exit slave via DIM command
void Control::killSlave(int which)   {
  char cmd[256];
  int  data = 1;
  const SlaveTag& tag = m_dim[which];
  ::snprintf(cmd,sizeof(cmd),"%s/EXIT",tag.name.c_str());
  string msg = cmd;
  int ret = ::dic_cmnd_service(cmd,&data,sizeof(data));
  if ( ret == 1 )
    msg += "> Successfully sent command to slave's EXIT datapoint.";
  else
    msg += "> Failed to send command to slave's EXIT datapoint.";
  write_message(msg.c_str());
  ioc().send(this,CMD_UPDATE_SLAVE,this);
}

/// Send slave to state error via DIM command
void Control::errorSlave(int which)   {
  char cmd[256];
  char data[] = {"ERROR"};
  const SlaveTag& tag = m_dim[which];
  ::snprintf(cmd,sizeof(cmd),"%s",tag.name.c_str());
  string msg = cmd;
  int ret = ::dic_cmnd_service(cmd,data,strlen(data)+1);
  if ( ret == 1 )
    msg += "> Successfully sent ERROR command to slave's command datapoint.";
  else
    msg += "> Failed to send ERROR command to slave's command datapoint.";
  write_message(msg.c_str());
  ioc().send(this,CMD_UPDATE_SLAVE,this);
}

/// Send slave to state error via triggered timeout
void Control::timeoutSlave(int which)   {
  char cmd[256];
  char data[] = {"TIMEOUT"};
  const SlaveTag& tag = m_dim[which];
  ::snprintf(cmd,sizeof(cmd),"%s",tag.name.c_str());
  string msg = cmd;
  int ret = ::dic_cmnd_service(cmd,data,strlen(data)+1);
  if ( ret == 1 )
    msg += "> Successfully sent TIMEOUT command to slave's command datapoint.";
  else
    msg += "> Failed to send TIMEOUT command to slave's command datapoint.";
  write_message(msg.c_str());
  ioc().send(this,CMD_UPDATE_SLAVE,this);
}

/// Display callback handler
void Control::handle(const Event& ev)   {
  Slave* slave;
  switch(ev.eventtype) {
  case IocEvent:
    switch(ev.type)  {
    case Slave::SLAVE_TRANSITION:
      slave = ev.iocPtr<Slave>();
      if ( slave )  {
	const State* state = slave->state();
	if ( state->name() == ST_NAME_OFFLINE )
	  gotoState(state->name());
	else if ( state->name() == ST_NAME_ERROR )
	  gotoState(state->name());
	ioc().send(this,CMD_UPDATE_SLAVE,this);
	return;
      }
      ioc().send(this,CMD_UPDATE_SLAVE,this);
      write_message("Unknown request SLAVE_TRANSITION ");
      return;
    case CMD_START_SLAVEIO:
      startSlaveIO();
      return;
    case CMD_STOP_SLAVEIO:
      stopSlaveIO();
      return;
    case CMD_KILL_CTRL:
      killController();
      return;
    case CMD_UPDATE_SLAVE:
      updateSlaveState();
      return;
    case CMD_UPDATE_MACHINE:
      updateMachineState();
      return;
    case CMD_SET_SLAVETYPE:
      clean_str(m_slvTypeCmd,sizeof(m_slvTypeCmd));
      return;
    case CMD_SET_PARTITION:
      clean_str(m_partitionCmd,sizeof(m_partitionCmd));
      return;
    case CMD_SET_CONFIG:
      clean_str(m_configCmd,sizeof(m_configCmd));
      return;
    case CMD_SET_RUNINFO:
      clean_str(m_runinfoCmd,sizeof(m_runinfoCmd));
      return;
    case CMD_SET_MODE:
      clean_str(m_modeCmd,sizeof(m_modeCmd));
      return;
    case CMD_SETSTATE_CTRL:
      clean_str(m_stateCmd,sizeof(m_stateCmd));
      setMachineState(m_stateCmd);
      set_cursor(CMD_SEND_CTRL,0);
      return;
    case CMD_SEND_CTRL:
      clean_str(m_slaveCmd,sizeof(m_slaveCmd));
      executeTransition(m_slaveCmd);
      return;
    case CMD_SLAVE_RESET:
      executeTransition("RESET");
      set_cursor(CMD_SEND_CTRL,0);
      return;
    case COM_SLAVE_STATE:   {
      pair<int,string>* p = ev.iocPtr<pair<int,string> >();
      replace_comment(p->first,p->second.c_str());
      delete p;
      return;
    }
    case CMD_WRITE_MESSAGE:
      write_message((const char*)ev.data);
      ::free(ev.data);
      return;
    case CMD_CLOSE:
      ::lib_rtl_sleep(1000);
      ::exit(2);
    case CMD_KILL_SLAVE:
      if      ( ev.iocData()<100 ) killSlave(ev.iocData()%100);
      else if ( ev.iocData()<200 ) errorSlave(ev.iocData()%100);
      else timeoutSlave(ev.iocData()%100);
      set_cursor(CMD_SEND_CTRL,0);
      return;
    case CMD_CONFIG_FSM:
      startController();
      set_cursor(CMD_SEND_CTRL,0);
      return;
    default:
      return;
    }
    return;
  default:
    return;
  }
}
