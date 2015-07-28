// $Id: FarmStatSrv.cpp,v 1.6 2010-10-15 07:42:00 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/FarmStatSrv.cpp,v 1.6 2010-10-15 07:42:00 frankb Exp $

#define MBM_IMPLEMENTATION
#include "ROMon/ROMon.h"
#include "ROMon/CPUMon.h"
#include "ROMon/Constants.h"
#include "ROMon/FarmStatSrv.h"
#include "ROMonDefs.h"
#include "CPP/IocSensor.h"
#include "CPP/TimeSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"
#include "RTL/strdef.h"
#include "RTL/Lock.h"
extern "C" {
#include "dic.h"
#include "dis.h"
}

// C++ include files
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <iostream>
#include <algorithm>

// Max. 15 seconds without update allowed
static int UPDATE_TIME_MAX = 15;

using namespace ROMon;
using namespace std;

typedef vector<string> StringV;

static string s_svcPrefix = "/";

static void help() {
  cout << "  romon_farmSummary -option [-option]" << endl
       << "       -all                         Show all subfarms." << endl
       << "       -p[artition]=<name>          Partition name providing monitoring information." << endl
       << endl;
}

SubfarmStatCollector::SubfarmStatCollector(FarmStatSrv* parent, const string& title) 
  : m_parent(parent), m_name(title)
{
  m_mbm = m_cpu = 0;
  m_mbmSvc = m_cpuSvc = 0;
  m_lastUpdate  = time(0);
  m_cpuChanged  = false;
  m_mbmChanged  = false;
  m_mbmData = m_name+":()";
  m_cpuData = m_name+":()";
}

SubfarmStatCollector::~SubfarmStatCollector() {
  disconnect();
}

/// Connect to publishing dim service
void SubfarmStatCollector::connect() {
  string svc, svc0 = s_svcPrefix+strlower(m_name);
  if ( m_parent->haveMBM() ) {
    svc = svc0 + "/ROpublish";
    m_mbm = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,mbmHandler,(long)this,0,0);
    svc = m_parent->name() + "/" + strlower(m_name) + "/MBM";
    m_mbmSvc = ::dis_add_service((char*)svc.c_str(),(char*)"C",0,0,feedString,(long)&m_mbmData);
  }
  if ( m_parent->haveCPU() ) {
    svc = svc0 + "/ROpublish/CPU";
    m_cpu = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,cpuHandler,(long)this,0,0);
    svc = m_parent->name() + "/" +strlower(m_name) + "/CPU";
    m_cpuSvc = ::dis_add_service((char*)svc.c_str(),(char*)"C",0,0,feedString,(long)&m_cpuData);
  }
  ::lib_rtl_output(LIB_RTL_DEBUG,"FarmStatSrv> Connected to subfarm: %s",name().c_str());
}

void SubfarmStatCollector::disconnect() {
  if ( m_mbm != 0 ) {
    ::dic_release_service(m_mbm);
    m_mbm = 0;
  }
  if ( m_mbmSvc != 0 ) {
    ::dis_remove_service(m_mbmSvc);
    m_mbmSvc = 0;
  }
  if ( m_cpu != 0 ) {
    ::dic_release_service(m_cpu);
    m_cpu = 0;
  }
  if ( m_cpuSvc != 0 ) {
    ::dis_remove_service(m_cpuSvc);
    m_cpuSvc = 0;
  }
}

/// Check monitor for errors
void SubfarmStatCollector::check(time_t now) {
  if ( now - lastUpdate() > 5*UPDATE_TIME_MAX ) {
    m_mbmData = m_name+":Timeout";
    m_cpuData = m_name+":Timeout";
  }
}

/// Update monitoring content
void SubfarmStatCollector::updateCPU(const CPUfarm& f) {
  int cnt;
  char text[1024];
  CPUset::Cores::const_iterator ic;
  string result = m_name+":(";

  for(CPUfarm::Nodes::const_iterator i=f.nodes.begin(); i!=f.nodes.end(); i=f.nodes.next(i)) {
    const CPUset& cs = (*i);
    const CPU::Stat& avg = cs.averages;
    ::sprintf(text,"[%s:{Avg:%.2f#%.2f#%.2f#%.2f#%.2f}",cs.name,avg.user,avg.system,avg.nice,avg.idle,avg.iowait);
    result += text;
    for(ic=cs.cores.begin(),cnt=0; ic!=cs.cores.end(); ic=cs.cores.next(ic),++cnt) {
      const CPU& c = *ic;
      ::sprintf(text,"{Core%02d:%.2f#%.2f#%.2f#%.2f#%.2f}",cnt,c.stats.user,c.stats.system,c.stats.nice,c.stats.idle,c.stats.iowait);
      result += text;
    }
    result += "}]";
  }
  result += ")";
  m_lastUpdate = time(0);
  m_cpuChanged = result != m_cpuData;
  if ( m_cpuChanged ) m_cpuData = result;
  ::lib_rtl_output(LIB_RTL_DEBUG,"FarmStatSrv> Updated CPU information of subfarm:%s",name().c_str());
  IocSensor::instance().send(m_parent,CMD_CHECK,this);
}

/// Update monitoring content
void SubfarmStatCollector::updateMBM(const Nodeset& ns) {
  char text[1024];
  string result = m_name+":(";
  for(Nodeset::Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    const Node::Buffers& buffs = *(*n).buffers();
    result += "[";
    result += (*n).name;
    result += ":";
    for(Node::Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      const MBMBuffer::Control& ctrl = (*ib).ctrl;
      ::sprintf(text,"{%s:%d#%d#%ld#%ld#%ld#%ld}",(*ib).name,
                int((ctrl.bm_size*ctrl.bytes_p_Bit)/1024),int((ctrl.i_space*ctrl.bytes_p_Bit)/1024),
                long(ctrl.p_emax),long(ctrl.i_events),
                long(ctrl.p_umax),long(ctrl.i_users));
      result += text;
    }
    result += "]";
  }
  result += ")";
  m_lastUpdate = time(0);
  m_mbmChanged = result != m_mbmData;
  if ( m_mbmChanged ) m_mbmData = result;
  ::lib_rtl_output(LIB_RTL_DEBUG,"FarmStatSrv> Updated MBM information of subfarm:%s",name().c_str());
  IocSensor::instance().send(m_parent,CMD_CHECK,this);
}

/// DIM command service callback
void SubfarmStatCollector::mbmHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    SubfarmStatCollector* disp = *(SubfarmStatCollector**)tag;
    const Nodeset* ns = (const Nodeset*)address;
    if ( ns->type == Nodeset::TYPE ) disp->updateMBM(*ns);
  }
}

/// DIM command service callback
void SubfarmStatCollector::cpuHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    SubfarmStatCollector* disp = *(SubfarmStatCollector**)tag;
    const CPUfarm* cs = (const CPUfarm*)address;
    if ( cs->type == CPUfarm::TYPE ) disp->updateCPU(*cs);
  }
}

/// DIM callback on dis_update_service
void SubfarmStatCollector::feedString(void* tag, void** buf, int* size, int* ) {
  string* s = *(string**)tag;
  *size = s->length()+1;
  *buf = (void*)s->c_str();
}

/// Publish summary information
void SubfarmStatCollector::publish() {
  ::lib_rtl_output(LIB_RTL_DEBUG,"FarmStatSrv> Publishing data to DIM. MBM:%s CPU:%s",
                   m_mbmChanged ? "YES" : "NO",m_cpuChanged ? "YES" : "NO");
  if ( m_mbmChanged ) ::dis_update_service(m_mbmSvc);
  if ( m_cpuChanged ) ::dis_update_service(m_cpuSvc);
}

/// Interactor overload: Monitor callback handler
void SubfarmStatCollector::handle(const Event& ev)    {
  switch(ev.eventtype) {
  case IocEvent: {
    switch(ev.type) {
    case CMD_UPDATE:
      publish();
      break;
    default:
      break;
    }
    break;
  }
  default:
    break;
  }
}

/// Standard constructor
FarmStatSrv::FarmStatSrv(int argc, char** argv) : m_dns(0), m_lock(0), m_name("/FarmStats")
{
  string service, in_node="ecs03", out_node="ecs03";
  RTL::CLI cli(argc,argv,help);
  cli.getopt("source",    2, in_node);
  cli.getopt("target",    2, out_node);
  cli.getopt("match",     2, m_match = "*");
  cli.getopt("service",   2, m_name);
  cli.getopt("prefix",    2, s_svcPrefix);
  m_haveMBM = cli.getopt("mbm",2) != 0;
  m_haveCPU = cli.getopt("cpu",2) != 0;

  ::dic_set_dns_node((char*)in_node.c_str());
  ::dis_set_dns_node((char*)out_node.c_str());
  ::lib_rtl_create_lock(0,&m_lock);
  print_startup("FarmStatSrv");
  if ( m_match == "*" )
    ::lib_rtl_output(LIB_RTL_INFO,"FarmStatSrv> MBM/CPU monitoring enabled on all known subfarms ");
  else
    ::lib_rtl_output(LIB_RTL_INFO,"FarmStatSrv> MBM/CPU monitoring enabled on all known subfarms matching '%s'",m_match.c_str());
  m_dns = ::dic_info_service((char*)"DIS_DNS/SERVER_LIST",MONITORED,0,0,0,dnsDataHandler,(long)this,0,0);

  ::dis_start_serving((char*)m_name.c_str());
  ::lib_rtl_sleep(1000);
  TimeSensor::instance().add(this,10,(void*)CMD_SHOW);
}

/// Standard destructor
FarmStatSrv::~FarmStatSrv()  {  
  if ( 0 != m_dns ) ::dic_release_service(m_dns);
  m_dns = 0;
  for(SubfarmMonitors::iterator k=m_farmMonitors.begin();k!=m_farmMonitors.end();++k)
    delete (*k).second;
  m_farmMonitors.clear();
  ::lib_rtl_output(LIB_RTL_INFO,"FarmStatSrv> Farm stat collectors deleted and resources freed...");
}

/// DIM command service callback
void FarmStatSrv::dnsDataHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    FarmStatSrv* disp = *(FarmStatSrv**)tag;
    disp->updateDns(address);
  }
}

/// DIM command service callback
void FarmStatSrv::updateDns(const void* address) {
  char *msg = (char*)address;
  string svc, node;
  size_t idx, idq;
  switch(msg[0]) {
  case '+':
    getServiceNode(++msg,svc,node);
    idx = svc.find("/ROpublish");
    idq = svc.find("/hlt");
    //if ( idq == string::npos ) idq = svc.find("/mona");
    //if ( idq == string::npos ) idq = svc.find("/store");
    if ( idx != string::npos && idq == 0 ) {
      string f = svc.substr(1,idx-1);
      if ( ::strcase_match_wild(f.c_str(),m_match.c_str()) ) {
        IocSensor::instance().send(this,CMD_ADD,new string(f));
      }
    }
    break;
  case '-':
    break;
  case '!':  // Service in error
    break;
  default:
    if ( *(int*)msg != *(int*)"DEAD" )  {
      char *at, *p = msg, *last = msg;
      auto_ptr<Farms> farms(new Farms);
      farms->push_back("");
      while ( last != 0 && (at=strchr(p,'@')) != 0 )  {
        last = strchr(at,'|');
        if ( last ) *last = 0;
        getServiceNode(p,svc,node);
        idx = svc.find("/ROpublish");
        idq = svc.find("/hlt");
        //if ( idq == string::npos ) idq = svc.find("/mona");
        //if ( idq == string::npos ) idq = svc.find("/store");
        if ( idx != string::npos && idq == 0 ) {
          string f = svc.substr(1,idx-1);
          if ( ::strcase_match_wild(f.c_str(),m_match.c_str()) ) {
            farms->push_back(f);
          }
        }
        p = last+1;
      }
      if ( !farms->empty() ) {
        IocSensor::instance().send(this,CMD_CONNECT,farms.release());
      }
    }
    break;
  }
}

/// Interactor overload: Monitor callback handler
void FarmStatSrv::handle(const Event& ev) {
  RTL::Lock lock(m_lock);
  switch(ev.eventtype) {
  case TimeEvent:
    if(ev.timer_data == (void*)CMD_SHOW)IocSensor::instance().send(this,CMD_SHOW,(void*)0);
    return;
  case IocEvent:
    switch(ev.type) {
    case CMD_SHOW: {
      for(SubfarmMonitors::iterator k=m_farmMonitors.begin();k!=m_farmMonitors.end();++k)
        (*k).second->publish();
      TimeSensor::instance().add(this,10,(void*)CMD_SHOW);
      return;
    }
    case CMD_ADD: {
      auto_ptr<string> val(ev.iocPtr<string>());
      if ( find(m_farms.begin(),m_farms.end(),*val.get()) == m_farms.end() ) {
        m_farms.push_back(*ev.iocPtr<string>());
        connect(m_farms);
      }
      return;
    }
    case CMD_CONNECT: {
      m_farms.clear();
      m_farms.assign(ev.iocPtr<StringV>()->begin()+1,ev.iocPtr<StringV>()->end());
      //m_farms = *ev.iocPtr<vector<string> >();
      connect(m_farms);
      delete ev.iocPtr<vector<string> >();
      return;
    }
    case CMD_CHECK: {
      time_t now = ::time(0);
      for(SubfarmMonitors::iterator k=m_farmMonitors.begin();k!=m_farmMonitors.end();++k)
        (*k).second->check(now);
      return;
    }
    case CMD_DELETE:
      delete this;
      ::lib_rtl_sleep(200);
      ::exit(0);
      return;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

void FarmStatSrv::connect(const vector<string>& farms) {
  SubfarmMonitors copy;
  ::lib_rtl_output(LIB_RTL_INFO,"FarmStatSrv> Total number of subfarms:%d",int(farms.size()));
  for(Farms::const_iterator i=farms.begin(); i != farms.end(); ++i) {
    SubfarmMonitors::iterator k = m_farmMonitors.find(*i);
    if ( k == m_farmMonitors.end() ) {
      SubfarmStatCollector* s = new SubfarmStatCollector(this,*i);
      copy.insert(make_pair(*i,s));
      s->connect();
    }
    else {
      copy.insert(*k);
      m_farmMonitors.erase(k);
    }
  }
  for(SubfarmMonitors::iterator k=m_farmMonitors.begin(); k != m_farmMonitors.end(); ++k)
    delete (*k).second;
  m_farmMonitors = copy;
  ::dis_start_serving((char*)name().c_str());
}

extern "C" int romon_farmstatsrv(int argc,char** argv) {
  int level = LIB_RTL_INFO;
  RTL::CLI cli(argc,argv,help);
  cli.getopt("printlevel", 2, level);
  ::lib_rtl_install_printer(ro_rtl_print,(void*)long(level));
  FarmStatSrv* disp = new FarmStatSrv(argc,argv);
  IocSensor::instance().run();
  delete disp;
  return 1;
}
