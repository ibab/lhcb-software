#include "ROMon/FarmMonitor.h"
#include "CPP/IocSensor.h"
extern "C" {
#include "dic.h"
}

// C++ include files
#include <cstdio>
#include <cstdlib>

using namespace ROMon;
using namespace std;

#include "ROMon/TaskSupervisor.h"
#include "ROMon/TaskSupervisorParser.h"

namespace ROMon {

  /**@class CtrlSubfarmMonitor ROMon.h GaudiOnline/FarmMonitor.h
   *
   *   Monitor summarizing one single subfarm. Showed as an array on the
   *   main display.
   *
   *   @author M.Frank
   */
  class CtrlSubfarmMonitor : public InternalMonitor {
    /// Extracted cluster information for all contained nodes
    Cluster             m_cluster;
  public:
    /// Initializing constructor
    CtrlSubfarmMonitor(FarmMonitor* parent, const string& title);
    /// Standard destructor
    virtual ~CtrlSubfarmMonitor();
    /// Update display content
    virtual void update(const void* data);
    /// Update monitor content
    virtual void update(const void* data, size_t)  { update(data); }
    /// Command service callback after already parsed data
    virtual void updateTs(XML::TaskSupervisorParser& ts);
  };
  InternalMonitor* createCtrlSubfarmMonitor(FarmMonitor* parent, const string& title) {
    return new CtrlSubfarmMonitor(parent,title);
  }
}

#include "ROMonDefs.h"

/// Initializing constructor
CtrlSubfarmMonitor::CtrlSubfarmMonitor(FarmMonitor* parent, const string& title)
  : InternalMonitor(parent, title)
{
  m_lastUpdate = time(0);
  string svc = svcPrefix()+strupper(title)+"/TaskSupervisor/Status";
  log("INFO") << "Connecting to service " << svc << endl;
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
}

/// Standard destructor
CtrlSubfarmMonitor::~CtrlSubfarmMonitor() {
}

/// DIM command service callback
void CtrlSubfarmMonitor::update(const void* address) {
  const char* data = (const char*)address;
  try {
    XML::TaskSupervisorParser ts;
    if ( ts.parseBuffer(m_name, data,::strlen(data)+1) ) {
      updateTs(ts);
    }
  }
  catch(...) {
  }
}

/// DIM command service callback
void CtrlSubfarmMonitor::updateTs(XML::TaskSupervisorParser& ts) {
  auto_ptr<AlarmInfo> alarms(new AlarmInfo(m_name,Alarms()));
  m_cluster.nodes.clear();
  ts.getClusterNodes(m_cluster);
  // log("INFO") << "Update data content of " << m_cluster.name << endl;
  for(Cluster::Nodes::const_iterator i=m_cluster.nodes.begin(); i!=m_cluster.nodes.end();++i) {
    const Cluster::Node& n = (*i).second;
    time_t when = time(0);
    if ( n.status == "DEAD" ) {
      setAlarm(alarms->second,n.name,ERR_NODE_DEAD,when,n.name);
    }
    else {
      for(Cluster::Projects::const_iterator q=n.projects.begin(); q != n.projects.end(); ++q) {
        bool pvss_ok = (*q).eventMgr && (*q).dataMgr && (*q).distMgr;
        std::string nam = (*q).name;
        nam += "@";
        nam += n.name;
        if ( !pvss_ok ) setAlarm(alarms->second,n.name,ERR_NO_PVSS_PROJECT,when,nam);
      }
      if ( n.taskCount > 0 ) {
        for(Cluster::Node::Tasks::const_iterator j=n.tasks.begin(); j != n.tasks.end(); ++j) {
          std::string nam = (*j).first;
          nam += "@";
          nam += n.name;
          if ( !(*j).second ) setAlarm(alarms->second,n.name,ERR_TASK_MISSING,when,nam);
        }
      }
      else {
        setAlarm(alarms->second,n.name,ERR_NO_TASKS,when,n.name);
      }
      if ( n.connCount > 0 ) {
        for(Cluster::Node::Connections::const_iterator j=n.conns.begin(); j != n.conns.end(); ++j)
          if ( !(*j).second ) setAlarm(alarms->second,n.name,ERR_CONNECTION_MISSING,when,(*j).first);
      }
      else {
        setAlarm(alarms->second,n.name,ERR_NO_CONNECTIONS,when,n.name);
      }
    }
  }
  IocSensor::instance().send(m_parent,CMD_CHECK_CTRL,alarms.release());
}
