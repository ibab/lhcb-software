#include "ControlMenu.h"

#include <boost/assign/std/vector.hpp>
#include "FiniteStateMachine/FmcSlave.h"
#include "FiniteStateMachine/NativeDimSlave.h"
#include "CPP/IocSensor.h"
#include <cstring>

enum  _commands {
  CMD_BACKSPACE = -1,
  CMD_CONFIG_FSM = 1,
  CMD_START_SLAVEIO,
  CMD_STOP_SLAVEIO,
  COM_MACH_STATE,
  COM_MACH_METASTATE,
  COM_CTRL_STATE,
  COM_CTRL_METASTATE,

  COM_SLAVE_STATE,
  CMD_SLAVE_RESET,

  CMD_SET_SLAVETYPE,
  CMD_SET_PARTITION,
  CMD_SET_RUNINFO,
  CMD_SET_CONFIG,
  CMD_SET_MODE,

  COM_LINE_0,
  COM_LINE_1,
  COM_LINE_2,
  COM_LINE_3,
  COM_LINE_4,
  COM_LINE_5,
  COM_LINE_6,

  CMD_KILL_SLAVE,
  CMD_KILL_SLAVE_END = CMD_KILL_SLAVE+20,
 
  CMD_KILL_CTRL,
  CMD_SEND_CTRL,
  CMD_SETSTATE_CTRL,
  CMD_NUM_SLAVES,

  CMD_CLOSE = 100,
  CMD_UPI_WRITE_MESSAGE,
  CMD_UPDATE_SLAVE,
  CMD_UPDATE_MACHINE,
  CMD_LAST = 2000
};

namespace UPI {
  using namespace boost::assign;

  IocSensor& ioc() {  return IocSensor::instance();  }

  struct CtrlNativeSlave : public FiniteStateMachine::NativeDimSlave {
    int m_out[2];
    ControlMenu* handler;

    /// Standard constructor
    CtrlNativeSlave(ControlMenu* handler,Machine* machine, const Type* typ, const std::string& nam, const std::string& args="")
      : NativeDimSlave(typ,nam,machine), handler(handler) 
    {
      if ( args.empty() )  {      
	m_cmd   = FiniteStateMachine::gentest_path();
	m_argv += nam,"libController.so","controller_fsm_test","-name="+nam;
      }
      else  {
	std::string copy = args;
	for( char* p = (char*)copy.c_str(), *savePtr=0;; p=0)  {
	  char* token = ::strtok_r(p," ",&savePtr);
	  if ( !token ) break;
	  m_argv += token;
	}
      }
      cloneEnv();
    }
    /// Standard destructor
    virtual ~CtrlNativeSlave() {
    }
    /// Start slave process
    FSM::ErrCond start()  {
      if ( m_pid == 0 )  {
	std::vector<char*> argv, envp;
	char text[64];
	int ret = 0;
	::pipe(m_out);
	handler->startSlaveIO();
	display(ALWAYS,"%s> Slave is forking child process.....",c_name());
	::sprintf(text,"-slaves=%d",handler->numSlaves());

	for(size_t i=0; i<m_argv.size();++i)
	  argv.push_back((char*)m_argv[i].c_str());
	argv.push_back(text);
	argv.push_back(0);

	for(size_t i=0; i<m_envp.size();++i)
	  envp.push_back((char*)m_envp[i].c_str());
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
	ioc().send(handler,CMD_UPDATE_SLAVE,this);
	break;
      case SLAVE_STARTING:
      case SLAVE_FAILED:
      case SLAVE_KILLED:
      case SLAVE_FINISHED:
      case SLAVE_TRANSITION_TIMEOUT:
	ioc().send(handler,CMD_UPDATE_SLAVE,this);
	break;
      case SLAVE_TRANSITION:
	ioc().send(handler,SLAVE_TRANSITION,this);
	break;
      case SLAVE_ALIVE:
	ioc().send(this,SLAVE_FINISHED,event.data);
	ioc().send(handler,CMD_UPDATE_SLAVE,this);
	return;
      default:
	break;
      }
    }
    int processIO() {
      FILE* f = ::fdopen(m_out[0],"r");
      char* line = 0;
      size_t len = 0;
      while( ::getline(&line,&len,f) != -1 )   {
	ioc().send(handler,CMD_UPI_WRITE_MESSAGE,::strdup(line));
      }
      ::free(line);
      ::fclose(f);
      ioc().send(handler,CMD_UPI_WRITE_MESSAGE,::strdup("Slave died! controlling thread exiting....."));
      ioc().send(handler,CMD_STOP_SLAVEIO,handler);
      return 1;
    }
  };

  struct CtrlFmcSlave : public FiniteStateMachine::FmcSlave {
    ControlMenu* handler;
    /// Standard constructor
    CtrlFmcSlave(ControlMenu* h, Machine* machine, const Type* typ, const std::string& nam, const std::string& args="") 
      : FmcSlave(typ,nam,machine,false), handler(h)    {  cmd_args = args;   }
    /// Standard destructor
    virtual ~CtrlFmcSlave()       {  }
    /// Nothing to do here
    int processIO()            {      return 1;    }
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
#include "FiniteStateMachine/FSMTypes.h"
#include "FiniteStateMachine/XmlTaskConfiguration.h"
#include "UPI/UpiSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "UPI/upidef.h"
#include "RTL/strdef.h"
#include "dis.hxx"
#include "dic.hxx"
#include <string>
#include <sstream>

using namespace UPI;
using namespace std;
using namespace FiniteStateMachine;
using namespace FiniteStateMachine::DAQ;

namespace UPI {

  void clean_str(char* n,size_t len)  {
    n[len-1] = 0;
    if ( ::strchr(n,' ') ) *::strchr(n,' ') = 0;
  }

  void backSpaceCallBack (int menu,int /* cmd */,int par,const void* param)  {
    Event ev;
    ev.target     = (Interactor*)param;
    ev.eventtype  = UpiEvent;
    ev.menu_id    = menu;
    ev.command_id = CMD_BACKSPACE;
    ev.param_id   = par;
    ev.index_id   = 0;
    ev.target->handle(ev);
  }
  static void print_routine(void* , const char* text) {
    char* t = (char*)text;
    *(t+strlen(text)-1) = 0;
    ::upic_write_message(text,"");
  }
}
static const char* s_slvList[]    = {"Native","FMC"};
static const char* s_partList[]   = {"LHCb","LHCb1","LHCb2","FEST","RICH","VELO"};
static const char* s_modeList[]   = {"NORMAL","FORKING","CHECKPOINTING"};
static const char* s_cmdList[]    = {"create","load","configure","start","stop","reset","unload","destroy","recover"};
static const char* s_stateList[]  = {"OFFLINE","NOT_READY","READY","RUNNING","ERROR","PAUSED","UNKNOWN"};

/// Standard constructor with object setup through parameters
ControlMenu::ControlMenu(const std::string& config) 
  : m_id(0), m_numSlaves(3), m_thread(0), m_machine(0), m_slave(0)
{
  TypedObject::setPrinter(this,print_routine);
  string line = "------------------------------------------------------------";
  m_config_exists = 0 == ::access(config.c_str(),R_OK);
  m_id = UpiSensor::instance().newID();
  ::strcpy(m_killCmd,"Kill");
  ::strcpy(m_errorCmd,"ERROR");
  ::strcpy(m_tmoCmd,"TIMEOUT");
  ::strcpy(m_modeCmd,s_modeList[0]);
  ::strcpy(m_slaveCmd,s_cmdList[0]);
  ::strcpy(m_stateCmd,s_stateList[0]);
  ::strcpy(m_slvTypeCmd,s_slvList[0]);
  ::strcpy(m_runinfoCmd,"OnlineEnv.py");
  ::strcpy(m_partitionCmd,s_partList[0]);
  ::strcpy(m_configCmd,config.empty() ? "Tasklist.xml" : config.c_str());

  ::upic_open_window();
  ::upic_open_menu(m_id,0,0,"FSM Controller test facility",RTL::processName().c_str(),RTL::nodeName().c_str());
  ::upic_declare_callback(m_id,CALL_ON_BACK_SPACE,(Routine)backSpaceCallBack,this);
  ::upic_add_comment(COM_MACH_STATE,       "FSM Machine State: ----","");
  ::upic_add_comment(COM_MACH_METASTATE,   "FSM Machine Meta:  ----","");
  ::upic_add_comment(COM_LINE_1,           line.c_str(),"");
  ::upic_add_comment(COM_CTRL_STATE,       "CTRL FSM State:    ----","");
  ::upic_add_comment(COM_CTRL_METASTATE,   "CTRL FSM Meta:     ----","");
  ::upic_add_comment(COM_LINE_2,           line.c_str(),"");
  ::upic_set_param  (&m_numSlaves,1, "I3", m_numSlaves, 0,0,0,0,0);
  ::upic_add_command(CMD_NUM_SLAVES,       "Number of slaves: ^^^                   ","");
  ::upic_set_param  (m_slvTypeCmd,1, "A15",  m_slvTypeCmd, 0,0,s_slvList,sizeof(s_slvList)/sizeof(s_slvList[0]),1);
  ::upic_add_command(CMD_SET_SLAVETYPE,    "FSM Slave type:   ^^^^^^^^^^^^^^^       ","");
  if ( m_config_exists )  {
    ::upic_set_param  (m_partitionCmd, 1, "A15", m_partitionCmd, 0,0,s_partList,sizeof(s_partList)/sizeof(s_partList[0]),1);
    ::upic_add_command(CMD_SET_PARTITION,  "Select partition: ^^^^^^^^^^^^^^^       ",""); 
    ::upic_set_param  (m_configCmd,1, "A20", m_configCmd, 0,0,0,0,0);
    ::upic_add_command(CMD_SET_CONFIG,     "Task Config:      ^^^^^^^^^^^^^^^^^^^   ","");
    ::upic_set_param  (m_runinfoCmd,1, "A20", m_runinfoCmd, 0,0,0,0,0);
    ::upic_add_command(CMD_SET_RUNINFO,    "Run Info:         ^^^^^^^^^^^^^^^^^^^   ","");
    ::upic_set_param  (m_modeCmd, 1, "A20", m_modeCmd, 0,0,s_modeList,sizeof(s_modeList)/sizeof(s_modeList[0]),1);
    ::upic_add_command(CMD_SET_MODE,       "Running mode:     ^^^^^^^^^^^^^^^^^^^   ",""); 
  }
  ::upic_add_comment(COM_LINE_3,           line.c_str(),"");
  ::upic_add_command(CMD_CONFIG_FSM,       "Configure FSM and slave                 ","");
  ::upic_set_param  (m_slaveCmd, 1, "A15", m_slaveCmd, 0,0,s_cmdList,sizeof(s_cmdList)/sizeof(s_cmdList[0]),1);
  ::upic_add_command(CMD_SEND_CTRL,        "Send  ^^^^^^^^^^^^^^                    ",""); 
  ::upic_add_command(CMD_SLAVE_RESET,      "Reset slaves                            ","");
  ::upic_set_param  (m_stateCmd, 1, "A15", m_stateCmd, 0,0,s_stateList,sizeof(s_stateList)/sizeof(s_stateList[0]),1);
  ::upic_add_command(CMD_SETSTATE_CTRL,    "Set my state  ^^^^^^^^^^^^^^            ",""); 
  ::upic_add_comment(COM_LINE_4,           line.c_str(),"");
  //::upic_add_command(CMD_KILL_CTRL,        "Kill  Controller             ",""); 
  ::upic_add_command(CMD_CLOSE,            "Close                                   ","");
  ::upic_add_comment(COM_LINE_5,           line.c_str(),"");
  ::upic_add_comment(COM_LINE_6,line.c_str(),"");
  ::upic_close_menu();
  ::upic_set_cursor(m_id,CMD_SEND_CTRL,0);
  UpiSensor::instance().add(this,m_id);
  ::upic_write_message("Opened window.","");
  ::upic_disable_command(m_id,CMD_SET_CONFIG);
}

/// Standard destructor
ControlMenu::~ControlMenu()  {
  Type* type = m_machine ? const_cast<Type*>(m_machine->type()) : 0;
  if ( m_machine ) delete m_machine;
  if ( m_slave ) delete m_slave;
  if ( type ) delete type;
  m_machine = 0;
  m_slave = 0;
  UpiSensor::instance().remove(this,m_id);
  ::upic_delete_menu(m_id);
  ::upic_write_message("Close window.","");
}

/// DimInfo overload to process slave states
void ControlMenu::slave_handler(void* tag, void* addr, int* size) {
  if ( tag ) {
    char text[128];
    SlaveTag*   s = *(SlaveTag**)tag, *t;
    const char* m = (size && *size>0) ? (const char*)addr : "OFFLINE - slave is DEAD";
    ::sprintf(text,"         State:%s",m);
    t = new SlaveTag(*s);
    t->line = ::strdup(text);
    ioc().send(s->ctrl,COM_SLAVE_STATE,t);
  }
}

int ControlMenu::ctrl_thread_routine(void* arg)  {
  CtrlNativeSlave* s = (CtrlNativeSlave*)arg;
  return s->processIO();
}

/// Update function when FSM machine completes transitions
FSM::ErrCond ControlMenu::update()      {
  //ioc().send(this,CMD_UPDATE_MACHINE,this);
  updateSlaveState();
  return FSM::SUCCESS;
}

/// Start the controller task
void ControlMenu::startController()   {
  Type* typ = fsm_type("DAQSteer");
  m_machine = new Machine(typ,RTL::processName()+"::DAQ");
  m_machine->setCompletionAction(Callback::make(this,&ControlMenu::update));
  m_machine->setFailAction(Callback::make(this,&ControlMenu::update));
  m_machine->setHandler(this);
  if ( m_config_exists )
    startControllerConfig();
  else
    startControllerNoConfig();
  ioc().send(this,CMD_UPDATE_SLAVE,this);
}

/// Start the controller task
void ControlMenu::startControllerNoConfig()   {
  string node = RTL::str_upper(RTL::nodeNameShort());
  Type* daq = fsm_type("DAQ");
  char text[128];

  if ( string(m_slvTypeCmd) == s_slvList[0] )   {
    m_slave = new CtrlNativeSlave(this,m_machine,daq,"CTRL_0");
  }
  else  {
    CtrlFmcSlave* slave = new CtrlFmcSlave(this,m_machine,daq,"CTRL_0");
    slave->fmc_args = "-g onliners -n online -w /home/frankm -O /tmp/logSrv.fifo -E /tmp/logSrv.fifo";
    m_slave = slave;
  }
  m_machine->addSlave(m_slave);

  //::upic_delete_command(m_id,CMD_CONFIG_FSM);
  for(int i=0; i<m_numSlaves; ++i)  {
    ::upic_set_param(m_killCmd, 1,"A4",m_killCmd, 0,0,0,0,1);
    ::upic_set_param(m_errorCmd,2,"A5",m_errorCmd,0,0,0,0,1);
    ::upic_set_param(m_tmoCmd,  3,"A7",m_tmoCmd,0,0,0,0,1);
    ::sprintf(text,"SLAVE_%d  ^^^^ Send to ^^^^^  Force ^^^^^^^",i);
    ::upic_insert_command(m_id,COM_LINE_6,CMD_KILL_SLAVE+2*i,  text,""); 
    ::upic_insert_comment(m_id,COM_LINE_6,CMD_KILL_SLAVE+2*i+1,"          ------------------",""); 
    ::sprintf(text,"SLAVE_%d",i);
    m_dim[i] = SlaveTag(this,text,CMD_KILL_SLAVE+2*i+1);
    ::sprintf(text,"SLAVE_%d/status",i);
    m_dim[i].svc = ::dic_info_service(text,MONITORED,0,0,0,slave_handler,(long)&m_dim[i],0,0);   
  }
}

/// Start the controller task
void ControlMenu::startControllerConfig()   {
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
  if ( string(m_slvTypeCmd) == s_slvList[0] )   {
    m_slave = new CtrlNativeSlave(this,m_machine,daq,nam.str(),args.str());
  }
  else  {
    CtrlFmcSlave* slave = new CtrlFmcSlave(this,m_machine,daq,nam.str(),args.str());
    slave->fmc_args = "-g onliners -n online -w /home/frankm -O /tmp/logSrv.fifo -E /tmp/logSrv.fifo";
    m_slave = slave;
  }
  m_slave->setCommand("runFarmCtrl.sh");
  m_machine->addSlave(m_slave);

  XmlTaskConfiguration cfg(m_partitionCmd,m_configCmd,m_runinfoCmd,m_modeCmd,m_numSlaves);
  vector<string> tasks;
  if ( cfg.getTasks(tasks) )  {
    char text[256];
    //::upic_delete_command(m_id,CMD_CONFIG_FSM);
    for(size_t i=0; i<tasks.size(); ++i)  {
      ::upic_set_param(m_killCmd, 1,"A4",m_killCmd, 0,0,0,0,1);
      ::upic_set_param(m_errorCmd,2,"A5",m_errorCmd,0,0,0,0,1);
      ::upic_set_param(m_tmoCmd,  3,"A7",m_tmoCmd,0,0,0,0,1);
      ::sprintf(text,"%-24s  ^^^^ Send to ^^^^^  Force ^^^^^^^",tasks[i].c_str());
      ::upic_insert_command(m_id,COM_LINE_6,CMD_KILL_SLAVE+2*i,  text,""); 
      ::upic_insert_comment(m_id,COM_LINE_6,CMD_KILL_SLAVE+2*i+1,"          ------------------",""); 
      ::sprintf(text,"%s/status",tasks[i].c_str());
      m_dim[i] = SlaveTag(this,tasks[i],CMD_KILL_SLAVE+2*i+1,0);
      m_dim[i].svc = ::dic_info_service(text,MONITORED,0,0,0,slave_handler,(long)&m_dim[i],0,0);   
    }
  }
}

/// Start the slave IO pump
void ControlMenu::startSlaveIO()   {
  if ( 0 == m_thread )  {
    int ret = ::lib_rtl_start_thread(ctrl_thread_routine,m_slave,&m_thread);
    if ( lib_rtl_is_success(ret) )   {
      ::upic_write_message("Started the IO thread....","");
      //m_slave->start(0);
    }
    else   {
      string msg = "Failed to start the IO thread:";
      msg += RTL::errorString();
      ::upic_write_message(msg.c_str(),"");
    }
  }
}

/// Stop the slave IO pump
void ControlMenu::stopSlaveIO()   {
  if ( 0 != m_thread )  {
    ::lib_rtl_delete_thread(m_thread);
    m_thread = 0;
    ::upic_write_message("Stopped the slave IO pump....","");
  }
}

/// Kill the controller task
void ControlMenu::killController()   {
  if ( m_slave )  {
    ::upic_write_message("Killing the controller task....","");
    m_slave->kill();
  }
}

/// Update the display with the new FSM machine state
void ControlMenu::updateMachineState()   {
  char text[128];
  ::sprintf(text,"FSM Machine State:%s",m_machine->c_state());
  ::upic_replace_comment(m_id,COM_MACH_STATE,text,"");
  ::sprintf(text,"FSM Machine Meta: %s",m_machine->currentMetaName());
  ::upic_replace_comment(m_id,COM_MACH_METASTATE,text,"");
}

/// Update the display with the new slave state
void ControlMenu::updateSlaveState()   {
  char text[128];
  ::sprintf(text,"CTRL FSM State:   %s",m_slave->c_state());
  ::upic_replace_comment(m_id,COM_CTRL_STATE,text,"");
  ::sprintf(text,"CTRL FSM Meta:    %s",m_slave->metaStateName());
  ::upic_replace_comment(m_id,COM_CTRL_METASTATE,text,"");
  updateMachineState();
}

/// Invoke transition on FSM machine
void ControlMenu::executeTransition(const string& transition)   {
  const State*      st = m_machine->state();
  const Transition* tr = st->findTransByName(transition);
  if ( !tr )  {
    ::upic_write_message((m_machine->name()+"> There is no valid transition "+transition+
			  " from state "+st->name()).c_str(),"");
    return;
  }
  m_machine->invokeTransition(tr);
}

/// Invoke transition on FSM machine to given targte state
void ControlMenu::gotoState(const string& state)   {
  const State*      st = m_machine->state();
  const Transition* tr = st->findTrans(state);
  if ( !tr )  {
    ::upic_write_message((m_machine->name()+"> There is no valid transition from state "+
			  st->name()+" to state "+state).c_str(),"");
    return;
  }
  m_machine->invokeTransition(tr,Rule::SLAVE2MASTER);
}

/// Force local machine state
void ControlMenu::setMachineState(const string& state_name)   {
  const Type*  type  = m_machine->type();
  const State* state = type->state(state_name);
  if ( !state )  {
    ::upic_write_message((m_machine->name()+"> There is no valid state "+
			  state_name+" in FSM type "+type->name()).c_str(),"");
    return;
  }
  m_machine->setState(state).setTarget(0);
}

/// Exit slave via DIM command
void ControlMenu::killSlave(int which)   {
  char cmd[128];
  int  data = 1;
  const SlaveTag& tag = m_dim[which];
  ::sprintf(cmd,"%s/EXIT",tag.name.c_str());
  string msg = cmd;
  int ret = ::dic_cmnd_service(cmd,&data,sizeof(data));
  if ( ret == 1 )
    msg += "> Successfully sent command to slave's EXIT datapoint.";
  else
    msg += "> Failed to send command to slave's EXIT datapoint.";
  ::upic_write_message(msg.c_str(),"");
}

/// Send slave to state error via DIM command
void ControlMenu::errorSlave(int which)   {
  char cmd[128];
  char data[] = {"ERROR"};
  const SlaveTag& tag = m_dim[which];
  ::sprintf(cmd,"%s",tag.name.c_str());
  string msg = cmd;
  int ret = ::dic_cmnd_service(cmd,data,strlen(data)+1);
  if ( ret == 1 )
    msg += "> Successfully sent ERROR command to slave's command datapoint.";
  else
    msg += "> Failed to send ERROR command to slave's command datapoint.";
  ::upic_write_message(msg.c_str(),"");
}

/// Send slave to state error via triggered timeout
void ControlMenu::timeoutSlave(int which)   {
  char cmd[128];
  char data[] = {"TIMEOUT"};
  const SlaveTag& tag = m_dim[which];
  ::sprintf(cmd,"%s",tag.name.c_str());
  string msg = cmd;
  int ret = ::dic_cmnd_service(cmd,data,strlen(data)+1);
  if ( ret == 1 )
    msg += "> Successfully sent TIMEOUT command to slave's command datapoint.";
  else
    msg += "> Failed to send TIMEOUT command to slave's command datapoint.";
  ::upic_write_message(msg.c_str(),"");
}

/// Display callback handler
void ControlMenu::handle(const Event& ev)   {
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
      ::upic_write_message("Unknown request SLAVE_TRANSITION ","");
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
      return;
    case CMD_SEND_CTRL:
      clean_str(m_slaveCmd,sizeof(m_slaveCmd));
      executeTransition(m_slaveCmd);
      return;
    case CMD_SLAVE_RESET:
      executeTransition("RESET");
      return;
    case COM_SLAVE_STATE:
      ::upic_replace_comment(m_id,ev.iocPtr<SlaveTag>()->id,ev.iocPtr<SlaveTag>()->line,"");
      delete ev.iocPtr<SlaveTag>();
      return;
    case CMD_UPI_WRITE_MESSAGE:
      ::upic_write_message((const char*)ev.data,"");
      ::free(ev.data);
      return;
    case CMD_CLOSE:
      ::lib_rtl_sleep(1000);
      ::exit(2);
    case CMD_KILL_SLAVE:
      if      ( ev.iocData()<100 ) killSlave(ev.iocData()%100);
      else if ( ev.iocData()<200 ) errorSlave(ev.iocData()%100);
      else timeoutSlave(ev.iocData()%100);
      return;

    case CMD_CONFIG_FSM:
      startController();
      return;
    default:
      return;
    }
    return;
  case UpiEvent:
    switch(ev.command_id) {
    case CMD_SETSTATE_CTRL:
    case CMD_SEND_CTRL:
    case CMD_CONFIG_FSM:
    case CMD_KILL_CTRL:
    case CMD_CLOSE:
    case CMD_SLAVE_RESET:
    case CMD_SET_SLAVETYPE:
    case CMD_SET_PARTITION:
    case CMD_SET_RUNINFO:
    case CMD_SET_CONFIG:
    case CMD_SET_MODE:
      ioc().send(this,ev.command_id,this);
      return;
    case CMD_BACKSPACE:
      //ioc.send(this,CMD_CLOSE,this);
      return;
    default:
      if ( ev.command_id>=CMD_KILL_SLAVE && ev.command_id<=CMD_KILL_SLAVE_END)
	ioc().send(this,CMD_KILL_SLAVE,(ev.command_id-CMD_KILL_SLAVE)/2+100*(ev.param_id-1));
      return;
    }
    return;
  default:
    return;
  }
}
static void help_menu() {}

extern "C" int fsm_upi_controlmenu(int argc, char** argv)  {
  string config;
  int srows, scols, cols, rows, col, row;
  RTL::CLI cli(argc, argv, help_menu);
  UpiSensor& s = UpiSensor::instance();
  cli.getopt("config",2,config);
  UPI::ControlMenu ctrl(config);

  ::dis_start_serving((char*)RTL::processName().c_str());
  ::upic_get_screen_size(&srows, &scols);
  ::upic_get_message_window(&rows, &cols,&row, &col);
  ::upic_set_message_window (srows-15, scols-2, 16, 2);
  s.run();
  return 0;
}
