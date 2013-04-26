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
#include "FiniteStateMachine/FmcSlave.h"
#include "FiniteStateMachine/Type.h"
#include "FiniteStateMachine/TaskManager.h"
#include "RTL/rtl.h"
extern "C" {
#include "dic.h"
#include "dim.h"
}

using namespace FiniteStateMachine;
using namespace std;

/// Class Constructor
FmcSlave::FmcSlave(const Type* typ, const string& nam, Machine* machine, bool internal) 
  : DimSlave(typ,nam,machine), m_internal(internal)
{
}

/// Standatrd destructor
FmcSlave::~FmcSlave() {
}

/// Start slave process
FSM::ErrCond FmcSlave::start()  {
  if ( isInternal() )  {
    send(SLAVE_ALIVE,0);
    return FSM::WAIT_ACTION;
  }
  TaskManager::instance(RTL::nodeNameShort()).start(name(),fmc_args,m_cmd,cmd_args);
  return FSM::SUCCESS;
}

/// Send transition request to the slave
FSM::ErrCond FmcSlave::sendRequest(const Transition* tr)  {
  if ( tr )  {
    return isInternal() ? send(SLAVE_FINISHED,tr->to()) : DimSlave::sendRequest(tr);
  }
  return FSM::FAIL;
}

/// Handle timeout on unload transition according to timer ID
void FmcSlave::handleUnloadTimeout()  {
  TaskManager::instance(RTL::nodeNameShort()).stop(name(),SIGTERM,15);
}
