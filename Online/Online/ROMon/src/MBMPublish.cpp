// $Id: MBMPublish.cpp,v 1.1 2010-10-06 11:31:23 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/MBMPublish.cpp,v 1.1 2010-10-06 11:31:23 frankb Exp $

#define MBM_IMPLEMENTATION
#include "ROMon/ROMon.h"
#include "ROMon/CPUMon.h"
#include "ROMon/MBMPublish.h"
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

static lib_rtl_lock_t    s_lock;

static void help() {
  cout << "  romon_farmSummary -option [-option]" << endl
       << "       -all                         Show all subfarms." << endl
       << "       -p[artition]=<name>          Partition name providing monitoring information." << endl
       << endl;
}

namespace {
  /// Extract node/service name from DNS info
  void getServiceNode(char* s, string& svc, string& node) {
    char* at = strchr(s,'@');
    *at = 0;
    svc = s;
    node = at+1;
  }
}

MBMInfoSource::MBMInfoSource(MBMPublish* parent, const string& title) 
  : m_parent(parent), m_name(title)
{
  m_mbm = m_cpu = 0;
  m_mbmSvc = m_cpuSvc = 0;
  m_lastUpdate  = time(0);
  m_cpuChanged  = false;
  m_mbmChanged  = false;
  m_mbmData = m_name+":[]";
  m_cpuData = m_name+":[]";
}

MBMInfoSource::~MBMInfoSource() {
  disconnect();
}

/// Connect to publishing dim service
void MBMInfoSource::connect() {
  string svc, svc0 = "/";
  for(size_t i=0; i<m_name.length();++i) svc0 += ::tolower(m_name[i]);
  svc = svc0 + "/ROpublish";
  m_mbm = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,mbmHandler,(long)this,0,0);
  svc = svc0 + "/ROpublish/CPU";
  m_cpu = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,cpuHandler,(long)this,0,0);
  svc = m_parent->name() + svc0 + "/MBM";
  m_mbmSvc = ::dis_add_service((char*)svc.c_str(),(char*)"C",0,0,feedString,(long)&m_mbmData);
  svc = m_parent->name() + svc0 + "/CPU";
  m_cpuSvc = ::dis_add_service((char*)svc.c_str(),(char*)"C",0,0,feedString,(long)&m_cpuData);
}

/// Log message with tag
ostream& MBMInfoSource::log(const string& tag,const string& node) {
  char txt[32];
  time_t when = time(0);
  ::strftime(txt,sizeof(txt),"%H:%M:%S",::localtime(&when));
  if ( node.empty() ) {
    cout << left << setw(9) << tag << setw(13) << left << m_name << left << setw(12) << txt << " ";
  }
  else {
    cout << left << setw(9) << tag << setw(13) << left << node << left << setw(12) << txt << " ";
  }
  return cout;
}

void MBMInfoSource::disconnect() {
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
void MBMInfoSource::check(time_t now) {
  if ( now - lastUpdate() > 5*UPDATE_TIME_MAX ) {
    m_mbmData = m_name+":Timeout";
    m_cpuData = m_name+":Timeout";
  }
}

/// Update monitoring content
void MBMInfoSource::updateCPU(const CPUfarm& f) {
  int cnt;
  char text[1024];
  CPUset::Cores::const_iterator ic;
  string result = m_name+":(";

  for(CPUfarm::Nodes::const_iterator i=f.nodes.begin(); i!=f.nodes.end(); i=f.nodes.next(i)) {
    const CPUset& cs = (*i);
    const CPU::Stat& avg = cs.averages;
    ::sprintf(text,"[%s:{Avg:%.2f#%.2f#%.2f#%.2f}",cs.name,avg.user,avg.system,avg.idle,avg.iowait);
    result += text;
    for(ic=cs.cores.begin(),cnt=0; ic!=cs.cores.end(); ic=cs.cores.next(ic),++cnt) {
      const CPU& c = *ic;
      ::sprintf(text,"{Core%02d:%.2f#%.2f#%.2f#%.2f}",cnt,c.stats.user,c.stats.system,c.stats.idle,c.stats.iowait);
      result += text;
    }
    result += "}]";
  }
  result += ")";
  m_lastUpdate = time(0);
  m_cpuChanged = result != m_cpuData;
  if ( m_cpuChanged ) m_cpuData = result;
  IocSensor::instance().send(m_parent,CMD_CHECK,this);
}

/// Update monitoring content
void MBMInfoSource::updateMBM(const Nodeset& ns) {
  char text[1024];
  string result = m_name+":(";
  for(Nodeset::Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    const Node::Buffers& buffs = *(*n).buffers();
    result += "[";
    result += (*n).name;
    result += ":";
    for(Node::Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      const MBMBuffer::Control& ctrl = (*ib).ctrl;
      ::sprintf(text,"{%s:%d#%d#%d#%d#%d#%d}",(*ib).name,
		int((ctrl.bm_size*ctrl.bytes_p_Bit)/1024),int((ctrl.i_space*ctrl.bytes_p_Bit)/1024),
		ctrl.p_emax,ctrl.i_events,
		ctrl.p_umax,ctrl.i_users);
      result += text;
    }
    result += "]";
  }
  result += ")";
  m_lastUpdate = time(0);
  m_mbmChanged = result != m_mbmData;
  if ( m_mbmChanged ) m_mbmData = result;
  IocSensor::instance().send(m_parent,CMD_CHECK,this);
}

/// DIM command service callback
void MBMInfoSource::mbmHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    MBMInfoSource* disp = *(MBMInfoSource**)tag;
    const Nodeset* ns = (const Nodeset*)address;
    if ( ns->type == Nodeset::TYPE ) disp->updateMBM(*ns);
  }
}

/// DIM command service callback
void MBMInfoSource::cpuHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    MBMInfoSource* disp = *(MBMInfoSource**)tag;
    const CPUfarm* cs = (const CPUfarm*)address;
    if ( cs->type == CPUfarm::TYPE ) disp->updateCPU(*cs);
  }
}

/// DIM callback on dis_update_service
void MBMInfoSource::feedString(void* tag, void** buf, int* size, int* ) {
  string* s = *(string**)tag;
  *size = s->length()+1;
  *buf = (void*)s->c_str();
}

/// Publish summary information
void MBMInfoSource::publish() {
  if ( m_cpuChanged ) ::dis_update_service(m_cpuSvc);
  if ( m_mbmChanged ) ::dis_update_service(m_mbmSvc);
  //cout << "Update data for " << m_name << " [" << m_cpuSvc << "," << m_mbmSvc << "]  " 
  //     << m_mbmData.substr(0,55) << " ..." << endl;
}

/// Interactor overload: Monitor callback handler
void MBMInfoSource::handle(const Event& ev)    {
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
MBMPublish::MBMPublish(int argc, char** argv) 
  : MBMInfoSource(0,"/FarmStats"), m_current(0), m_mode(HLT_MODE), m_update_time_max(UPDATE_TIME_MAX)
{
  char txt[128];
  string service, in_node="ecs03", out_node="ecs03";
  RTL::CLI cli(argc,argv,help);
  cli.getopt("source",    2, in_node);
  cli.getopt("target",    2, out_node);
  cli.getopt("match",     2, m_match = "*");
  cli.getopt("update",    2, m_update_time_max);
  cli.getopt("service",   2, m_name);

  ::dic_set_dns_node((char*)in_node.c_str());
  ::dis_set_dns_node((char*)out_node.c_str());

  m_mode = cli.getopt("reconstruction",2) == 0 ? HLT_MODE : RECO_MODE;
  if ( cli.getopt("taskmonitor",2) != 0 ) m_mode = CTRL_MODE;

  ::lib_rtl_create_lock(0,&s_lock);
  if ( m_match == "*" )
    ::sprintf(txt,"MBM/CPU monitoring enabled on all known subfarms ");
  else
    ::sprintf(txt,"MBM/CPU monitoring enabled on all known subfarms matching '%s'",m_match.c_str());
  m_time = time(0);
  log("INFO") << txt << endl;
  m_mbm = ::dic_info_service((char*)"DIS_DNS/SERVER_LIST",MONITORED,0,0,0,dnsDataHandler,(long)this,0,0);

  ::dis_start_serving((char*)m_name.c_str());
  ::lib_rtl_sleep(1000);
  TimeSensor::instance().add(this,10,(void*)CMD_SHOW);
}

/// Standard destructor
MBMPublish::~MBMPublish()  {  
  disconnect();
  subMonitors().clear();
  ::printf("Farm summary monitor deleted and resources freed......\n");
}

/// DIM command service callback
void MBMPublish::dnsDataHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    MBMPublish* disp = *(MBMPublish**)tag;
    disp->update(address);
  }
}

/// DIM command service callback
void MBMPublish::update(const void* address) {
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
  case '!':
    //getServiceNode(++msg,svc,node);
    //log() << "Service " << msg << " in ERROR." << endl;
    break;
  default:
    if ( *(int*)msg != *(int*)"DEAD" )  {
      char *at, *p = msg, *last = msg;
      auto_ptr<Farms> farms(new Farms);
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
void MBMPublish::handle(const Event& ev) {
  RTL::Lock lock(s_lock);
  switch(ev.eventtype) {
  case TimeEvent:
    if(ev.timer_data == (void*)CMD_SHOW)IocSensor::instance().send(this,CMD_SHOW,(void*)0);
    return;
  case IocEvent:
    switch(ev.type) {
    case CMD_SHOW: {
      for(MBMMonitors::iterator k=m_farmMonitors.begin();k!=m_farmMonitors.end();++k)
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
      m_farms = *auto_ptr<StringV>(ev.iocPtr<StringV>()).get();
      connect(m_farms);
      return;
    }
    case CMD_CHECK: {
      time_t now = ::time(0);
      for(MBMMonitors::iterator k=m_farmMonitors.begin();k!=m_farmMonitors.end();++k)
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

void MBMPublish::connect(const vector<string>& farms) {
  MBMMonitors::iterator k;
  MBMMonitors copy;
  char txt[128];

  ::sprintf(txt,"Total number of subfarms:%d",int(farms.size()));
  log("INFO") << txt << endl;

  for (Farms::const_iterator i=farms.begin(); i != farms.end(); ++i) {
    k = m_farmMonitors.find(*i);
    if ( k == m_farmMonitors.end() ) {
      MBMInfoSource* s = new MBMInfoSource(this,*i);
      copy.insert(make_pair(*i,s));
      s->connect();
    }
    else {
      copy.insert(*k);
      m_farmMonitors.erase(k);
    }
  }
  for (k=m_farmMonitors.begin(); k != m_farmMonitors.end(); ++k)
    delete (*k).second;
  m_farmMonitors = copy;
  ::dis_start_serving((char*)name().c_str());
}

static size_t do_output(void*,int level,const char* fmt, va_list args) {
  char buffer[1024];
  size_t len = ::vsnprintf(buffer, sizeof(buffer), fmt, args);
  //::upic_write_message(buffer,"");
  switch(level) {
  case LIB_RTL_VERBOSE:
    cout << left << setw(8) << "VERBOSE" << setw(12) << left << "RTL" << ":" << buffer << endl;
    break;
  case LIB_RTL_DEBUG:
    cout << left << setw(8) << "DEBUG" << setw(12) << left << "RTL" << ":" << buffer << endl;
    break;
  case LIB_RTL_INFO:
    cout << left << setw(8) << "INFO" << setw(12) << left << "RTL" << ":" << buffer << endl;
    break;
  case LIB_RTL_WARNING:
    cout << left << setw(8) << "WARNING" << setw(12) << left << "RTL" << ":" << buffer << endl;
    break;
  case LIB_RTL_ERROR:
    cout << left << setw(8) << "ERROR" << setw(12) << left << "RTL" << ":" << buffer << endl;
    break;
  case LIB_RTL_FATAL:
    cout << left << setw(8) << "FATAL" << setw(12) << left << "RTL" << ":" << buffer << endl;
    break;
  case LIB_RTL_ALWAYS:
    cout << left << setw(8) << "ALWAYS" << setw(12) << left << "RTL" << ":" << buffer << endl;
    break;
  }
  return len;
}

extern "C" int romon_farmSummary(int argc,char** argv) {
  int level = 3;
  RTL::CLI cli(argc,argv,help);
  cli.getopt("printlevel", 2, level);
  MBMPublish* disp = new MBMPublish(argc,argv);
  ::lib_rtl_install_printer(do_output,0);
  IocSensor::instance().run();
  delete disp;
  return 1;
}
