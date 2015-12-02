/*============================================================
**
**      Finite state machine implementation to control 
**      and manipulate process groups
**      FOR HLT2 monitoring purposes
**
**  AUTHORS:
**
**      M.Frank  CERN/LHCb
**
**==========================================================*/

// Framework include files
#include "Controller/RunMonitor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "dis.hxx"

// C/C++ include files
#include <sstream>

using namespace std;
using namespace FiniteStateMachine;

/// Standard constructor
RunMonitorMessenger::RunMonitorMessenger(RunMonitorCoordinator* coord, const RunMonitorConfig* cfg)
  : CPP::Interactor(), m_coordinator(coord), m_config(cfg)
{
  string nam = cfg->name;
  m_run          = make_pair(0, make_pair(string(), &coord->m_running));
  m_run.first    = ::dis_add_service((nam+"/run").c_str(),"C",0,0,dataFeed,(long)&m_run);
  m_todo         = make_pair(0, make_pair(string(), &coord->m_todo));
  m_todo.first   = ::dis_add_service((nam+"/todo").c_str(),"C",0,0,dataFeed,(long)&m_todo);
  m_done         = make_pair(0, make_pair(string(), &coord->m_done));
  m_done.first   = ::dis_add_service((nam+"/done").c_str(),"C",0,0,dataFeed,(long)&m_done);
  m_failed       = make_pair(0, make_pair(string(), &coord->m_failed));
  m_failed.first = ::dis_add_service((nam+"/failed").c_str(),"C",0,0,dataFeed,(long)&m_failed);

  m_cmd = ::dis_add_cmnd((char*)(nam+"/cmd").c_str(),"C",commandHandler,(long)this);
}

/// Standard destructor
RunMonitorMessenger::~RunMonitorMessenger()   {
  ::dis_remove_service(m_cmd);
  ::dis_remove_service(m_run.first);
  ::dis_remove_service(m_todo.first);
  ::dis_remove_service(m_done.first);
  ::dis_remove_service(m_failed.first);
}

/// DIM callback on dis_update_service
void RunMonitorMessenger::dataFeed(void* tag, void** buf, int* size, int* ) {
  stringstream str;
  PubItem* p = *(PubItem**)tag;
  const RunMonitorItems* m = p->second.second;
  str << "[ ";
  for(RunMonitorItems::const_iterator j,i=m->begin(); i!= m->end(); ++i) {
    j = i;
    str << (*i)->runNumber << (char*)((++j == m->end()) ? "" : " | ");
  }
  str << " ]";
  p->second.first = str.str();
  *size = p->second.first.length()+1;
  *buf = (void*)p->second.first.c_str();
}

/// DIM acllback to handle commands
void RunMonitorMessenger::commandHandler(void* tag, void* address, int* size)    {
  RunMonitorMessenger* p = *(RunMonitorMessenger**)tag;
  if ( *size && address )  {
    const char* param = (const char*)address;
    TypedObject::display(TypedObject::ALWAYS,"COMMAND","Received request:%s",param);
    IocSensor::instance().send(p,CMD_EXECUTE,new string(param));
  }
}

/// Interactor callback to handle external interrupts
void RunMonitorMessenger::handle(const CPP::Event& event)   {
  switch(event.eventtype) {
  case IocEvent:
    switch(event.type)  {
    case CMD_EXECUTE:   {
      const string& cmd = *event.iocPtr<string>();
      if ( cmd == "scan" )
	IocSensor::instance().send(this,CMD_SCAN);
      else if ( cmd.substr(0,3) == "pub" )
	IocSensor::instance().send(this,CMD_PUBLISH);
      delete event.iocPtr<string>();
      break;
    }
    case CMD_EXIT:
      break;

    case CMD_PUBLISH:
      ::dis_update_service(m_run.first);
      ::dis_update_service(m_todo.first);
      ::dis_update_service(m_done.first);
      ::dis_update_service(m_failed.first);
      break;

    case CMD_SCAN:
      IocSensor::instance().send(m_coordinator,CMD_SCAN);
      break;

    default:
      break;
    }
    break;
  default:
    break;
  }
}
