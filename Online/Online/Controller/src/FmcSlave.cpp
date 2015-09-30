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
#include "Controller/FmcSlave.h"
#include "Controller/TaskManager.h"
#include "FiniteStateMachine/Type.h"
#include "RTL/rtl.h"
extern "C" {
#include "dic.h"
#include "dim.h"
}

using namespace FiniteStateMachine;
using namespace std;

/// Class Constructor
FmcSlave::FmcSlave(const Type* typ, const string& nam, Machine* machine, bool internal) 
  : DimSlave(typ,nam,machine,internal)
{
}

/// Standatrd destructor
FmcSlave::~FmcSlave() {
}

/// Set the FMC arguments from single string
DimSlave& FmcSlave::setFmcArgs(const string& args)  {
  m_fmcArgs.clear();
  return addFmcArgs(args);
}

/// Add the FMC arguments from single string
DimSlave& FmcSlave::addFmcArgs(const string& args)  {
  std::string copy = "--";
  copy = args.c_str();
  for( char* p = (char*)copy.c_str(), *savePtr=0;; p=0)  {
    char* token = ::strtok_r(p," ",&savePtr);
    if ( !token ) break;
    m_fmcArgs.push_back(token);
  }
  return *this;
}

/// Start slave process
FSM::ErrCond FmcSlave::start()  {
  string fmc_args, cmd_args;
  for(size_t i=0; i<m_fmcArgs.size();++i)
    fmc_args += m_fmcArgs[i] + " ";
  for(size_t i=0; i<m_argv.size();++i)
    cmd_args += m_argv[i] + " ";

  // Start the process using the FMC task manager
  TaskManager::instance(RTL::nodeNameShort()).start(name(),fmc_args,m_cmd,cmd_args);
  return FSM::SUCCESS;
}

/// Handle timeout on unload transition according to timer ID
void FmcSlave::handleUnloadTimeout()  {
  forceKill();
}

/// Kill slave process
FSM::ErrCond FmcSlave::kill()  {
  DimSlave::kill();
  //TaskManager::instance(RTL::nodeNameShort()).stop(name(),SIGKILL,5);
  return FSM::WAIT_ACTION;
}

/// Force slave killing
FSM::ErrCond FmcSlave::forceKill()  {
  TaskManager::instance(RTL::nodeNameShort()).kill(name(),SIGKILL);
  iamDead();
  return FSM::SUCCESS;
}
