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
#include "ControlMenu.h"
#include "Controller/XmlTaskConfiguration.h"
#include "UPI/UpiSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"

using namespace std;
using namespace FiniteStateMachine;

enum  _commands {
  CMD_BACKSPACE = -1,
  COM_FIRST_MENU=FiniteStateMachine::CMD_LAST+1,

  COM_LINE_0,
  COM_LINE_1,
  COM_LINE_2,
  COM_LINE_3,
  COM_LINE_4,
  COM_LINE_5,
  COM_LINE_6
};

static void backSpaceCallBack (int menu,int /* cmd */,int par,const void* param)  {
  Event ev;
  ev.target     = (Interactor*)param;
  ev.eventtype  = UpiEvent;
  ev.menu_id    = menu;
  ev.command_id = CMD_BACKSPACE;
  ev.param_id   = par;
  ev.index_id   = 0;
  ev.target->handle(ev);
}

static const char* s_slvList[]    = {"CtrlNativeSlave","CtrlFmcSlave"};
static const char* s_partList[]   = {"LHCb","LHCb1","LHCb2","FEST","RICH","VELO"};
static const char* s_modeList[]   = {"NORMAL","FORKING","CHECKPOINTING"};
static const char* s_cmdList[]    = {"create","load","configure","start","stop","reset","unload","destroy","recover"};
static const char* s_stateList[]  = {"OFFLINE","NOT_READY","READY","RUNNING","ERROR","PAUSED","UNKNOWN"};

/// Standard constructor with object setup through parameters
UPI::ControlMenu::ControlMenu(const std::string& config) 
  : Control(config), m_id(0)
{
  string line = "------------------------------------------------------------";
  m_id = UpiSensor::instance().newID();
  ::strcpy(m_killCmd,"Kill");
  ::strcpy(m_errorCmd,"ERROR");
  ::strcpy(m_tmoCmd,"TIMEOUT");
  ::strcpy(m_modeCmd,s_modeList[0]);
  ::strcpy(m_slaveCmd,s_cmdList[0]);
  ::strcpy(m_stateCmd,s_stateList[0]);
  ::strcpy(m_slvTypeCmd,s_slvList[m_config_exists ? 1 : 0]);
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
  ::upic_set_cursor(m_id,CMD_CONFIG_FSM,0);
  UpiSensor::instance().add(this,m_id);
  ::upic_disable_command(m_id,CMD_SET_CONFIG);
  write_message("Opened window.");
}

/// Standard destructor
UPI::ControlMenu::~ControlMenu()  {
  UpiSensor::instance().remove(this,m_id);
  ::upic_delete_menu(m_id);
  ::upic_write_message("Close window.","");
}

/// Output and printing helper function
void UPI::ControlMenu::write_message(const char* fmt,...)   {
  va_list args;                                // - the log level
  va_start( args, fmt);                        // - a standard C formatted 
  char str[4096];                              //   string (like printf)
  size_t il=0, len = vsnprintf(str+il,sizeof(str)-il-2,fmt,args);
  va_end (args);
  str[il+len]   = '\0';
  str[il+len+1] = '\0';
  ::upic_write_message(str,"");
}

/// Start the controller task
void UPI::ControlMenu::startControllerNoConfig()   {
  char text[256];
  Control::startControllerNoConfig();
  for(int i=0; i<m_numSlaves; ++i)  {
    ::upic_set_param(m_killCmd, 1,"A4",m_killCmd, 0,0,0,0,1);
    ::upic_set_param(m_errorCmd,2,"A5",m_errorCmd,0,0,0,0,1);
    ::upic_set_param(m_tmoCmd,  3,"A7",m_tmoCmd,0,0,0,0,1);
    ::sprintf(text,"SLAVE_%d  ^^^^ Send to ^^^^^  Force ^^^^^^^",i);
    ::upic_insert_command(m_id,COM_LINE_6,CMD_KILL_SLAVE+2*i,  text,""); 
    ::upic_insert_comment(m_id,COM_LINE_6,CMD_KILL_SLAVE+2*i+1,"          ------------------",""); 
  }
}

/// Start the controller task
void UPI::ControlMenu::startControllerConfig()   {
  vector<string> tasks;
  XmlTaskConfiguration cfg(m_partitionCmd,m_configCmd,m_runinfoCmd,m_modeCmd,m_numSlaves);
  Control::startControllerConfig();
  if ( cfg.getTasks(tasks) )  {
    char text[256];
    for(size_t i=0; i<tasks.size(); ++i)  {
      ::upic_set_param(m_killCmd, 1,"A4",m_killCmd, 0,0,0,0,1);
      ::upic_set_param(m_errorCmd,2,"A5",m_errorCmd,0,0,0,0,1);
      ::upic_set_param(m_tmoCmd,  3,"A7",m_tmoCmd,0,0,0,0,1);
      ::sprintf(text,"%-24s  ^^^^ Send to ^^^^^  Force ^^^^^^^",tasks[i].c_str());
      ::upic_insert_command(m_id,COM_LINE_6,CMD_KILL_SLAVE+2*i,  text,""); 
      ::upic_insert_comment(m_id,COM_LINE_6,CMD_KILL_SLAVE+2*i+1,"          ------------------",""); 
    }
  }
}

/// Display callback handler
void UPI::ControlMenu::handle(const Event& ev)   {
  switch(ev.eventtype) {
  case IocEvent:
    Control::handle(ev);
    return;
  case UpiEvent:
    if ( ev.command_id>=CMD_KILL_SLAVE && ev.command_id<=CMD_KILL_SLAVE_END)
      IocSensor::instance().send(this,CMD_KILL_SLAVE,(ev.command_id-CMD_KILL_SLAVE)/2+100*(ev.param_id-1));
    else if ( ev.command_id != CMD_BACKSPACE )
      IocSensor::instance().send(this,ev.command_id,this);
    return;
  default:
    return;
  }
}

#include "dis.hxx"
extern "C" int fsm_upi_controlmenu(int argc, char** argv)  {
  string config;
  int srows, scols, cols, rows, col, row;
  RTL::CLI cli(argc, argv, 0);
  cli.getopt("config",2,config);

  UPI::ControlMenu ctrl(config);
  ::upic_get_screen_size(&srows, &scols);
  ::upic_get_message_window(&rows, &cols,&row, &col);
  ::upic_set_message_window (srows-15, scols-2, 16, 2);
  ::dis_start_serving((char*)RTL::processName().c_str());
  UpiSensor::instance().run();
  return 0;
}
