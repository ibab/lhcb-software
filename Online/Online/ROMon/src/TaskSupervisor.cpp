
#include "ROMon/TaskSupervisor.h"
#include "ROMon/CPUMon.h"
#include "CPP/TimeSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"
#include "ROMonDefs.h"

#include <iomanip>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <sstream>
#include <cerrno>
#include "rpc/PyRPC.h"
extern "C" {
#include "dis.h"
#include "dic.h"
}

using namespace std;
using namespace ROMon;
using namespace PyRPC;

#define CONNECTION_UPDATE_TIMEDIFF 30
#define TASK_UPDATE_TIMEDIFF       60
#define PUBLISH_TIMEDIFF           10
#define TIME_FORMAT                "%Y-%m-%d %H:%M:%S %Z"
#define CMD_DATA                   12345
static string PUBLISHING_NODE = "ECS03";
static bool s_debug = false;

namespace {
  template <class T> struct _Delete { void operator()(T*& p) { if ( p ) delete p; } };
  template <class K, class T> struct _DeleteMap {
    void operator()(T*& p) { if ( p ) delete p; }
    void operator()(pair<const K,T*>& p) { this->operator()(p.second); }
  };
  template <class K, class T> void clear(vector<K,T*>& m) {   for_each(m.begin(),m.end(),_Delete<T>()); m.clear(); }
  template <class K, class T> void clear(list<K,T*>& m) {   for_each(m.begin(),m.end(),_Delete<T>()); m.clear(); }
  template <class K, class T> void clear(map<K,T*>& m) {   for_each(m.begin(),m.end(),_DeleteMap<K,T>()); m.clear(); }

  struct ManipTaskMon {
    typedef vector<NodeTaskMon*> Monitors;
    const string&    m_tag;
    Monitors         m_errors;
    int (NodeTaskMon::*m_pmf)();
    ManipTaskMon(const string& tag, int (NodeTaskMon::*pmf)()) : m_tag(tag), m_pmf(pmf) {}
    bool ok() const { return m_errors.empty(); }
    string error() const;
    void operator()(pair<const string,NodeTaskMon*>& mon) { (*this)(mon.second); }
    bool operator()(NodeTaskMon* mon);
    int check(bool throw_if_error=true);
  };
}

string ManipTaskMon::error() const {
  stringstream err;
  for(Monitors::const_iterator i=m_errors.begin(); i != m_errors.end(); ++i)
    err << m_tag+" for " << (*i)->name() << " of type " << (*i)->type() 
        << " Error:" << (*i)->error() << endl;
  return err.str();
}

bool ManipTaskMon::operator()(NodeTaskMon* mon) {
  try {
    int res = (mon->*m_pmf)();
    if ( 1 != res ) m_errors.push_back(mon); 
    return res == 1;
  }
  catch(const exception& e) {
    mon->setError(e.what());
  }
  catch(...) {
    mon->setError("Unknown Exception");
  }
  return false;
}

int ManipTaskMon::check(bool throw_if_error) {
  if ( !ok() )  {
    string err = error();
    ::lib_rtl_output(LIB_RTL_FATAL,err.c_str());
    if ( throw_if_error ) throw runtime_error(err);
  }
  return 1;
}

string Cluster::PVSSProject::projectName(const string& n) {
  size_t idq, idx = n.find("-PROJ");
  if ( idx != string::npos ) {
    idx = n.find(' ',idx);
    if ( idx != string::npos ) {
      ++idx;
      while(::isspace(n[idx]) )++idx;
      idq = n.find(' ',idx);
      return n.substr(idx,idq-idx);
    }
  }
  return "";
}

ostream& ROMon::operator<<(ostream& os, const vector<Cluster::Node::Item>& t) {
  for(Cluster::Node::Tasks::const_iterator i=t.begin(); i!=t.end();++i)
    os << setw(12) << left << (*i).first << " " << (const char*)((*i).second?"/OK":"/NOT OK");
  os << right;
  return os;
}

ostream& ROMon::operator<<(ostream& os, const Cluster::Node& n) {
  os << "    Node:" << setw(12) << left << n.name << "   [" << n.status << "]" << endl
     << "       Tasks:      " << n.tasks << endl
     << "       Connections:" << n.conns << endl << right;
  return os;
}

ostream& ROMon::operator<<(ostream& os, const Cluster& c) {
  os << "  Cluster:" << setw(12) << left << c.name << "  [" << c.status << "]" << endl;
  for(Cluster::Nodes::const_iterator i=c.nodes.begin(); i!=c.nodes.end();++i)
    os << (*i).second;
  os << right;
  return os;
}

ostream& ROMon::operator<<(ostream& os, const list<Cluster>& c) {
  for(list<Cluster>::const_iterator i=c.begin(); i!=c.end();++i)
    os << "-------------------------------------------------------------" << endl << *i;
  return os;
}

ostream& ROMon::operator<<(ostream& os, const Inventory::NodeCollection& nc) {
  os << "     -->Node Collection:" << nc.name << endl;
  for(Inventory::NodeCollection::NodeList::const_iterator j=nc.nodes.begin(); j != nc.nodes.end();++j)
    os << "         -->Node:      " << setw(18) << left << (*j).first << (*j).second << endl;
  os << right;
  return os;
}

ostream& ROMon::operator<<(ostream& os, const Inventory::NodeCollectionMap& n) {
  for(Inventory::NodeCollectionMap::const_iterator i=n.begin(); i != n.end();++i)
    os << (*i).second;
  return os;
}

ostream& ROMon::operator<<(ostream& os, const Inventory::NodeType& nt) {
  os << "Node Type:" << nt.name << endl;
  for(Inventory::TaskList::const_iterator j=nt.tasks.begin();j!=nt.tasks.end();++j)
    os << "    Task:" << (*j) << endl;
  for(Inventory::ConnectionList::const_iterator k=nt.connections.begin(); k != nt.connections.end();++k)
    os << "    Connection:" << (*k) << endl;
  return os;
}

ostream& ROMon::operator<<(ostream& os, const Inventory::NodeTypeMap& n) {
  for(Inventory::NodeTypeMap::const_iterator i=n.begin(); i != n.end();++i)
    os << (*i).second;
  return os;
}

ostream& ROMon::operator<<(ostream& os, const Inventory& inv)  {  return inv.print(os);    }
ostream& Inventory::print(ostream& os)   const  {
  os << "-------------------------------  Task Inventory ------------------------------" << endl;
  os << "-->List of all tasks:" << endl;
  for(TaskMap::const_iterator i=tasks.begin();i!=tasks.end();++i)
    os << "     -->Task: " << (*i).first << endl;

  os << "-->List of all tasks lists:" << endl;
  for(map<string,TaskList>::const_iterator i=connlists.begin();i!=connlists.end();++i) {
    os << "     -->TaskList: " << (*i).first << endl;
    for(TaskList::const_iterator j=(*i).second.begin();j!=(*i).second.end();++j)
      os << "       -->Task: " << (*j) << endl;
  }
  os << "-->List of all connections lists:" << endl;
  for(ConnectionListMap::const_iterator m=connlists.begin();m!=connlists.end();++m) {
    os << "     -->ConnectionList: " << (*m).first << endl;
    for(ConnectionList::const_iterator j=(*m).second.begin(); j != (*m).second.end();++j)
      os << "       -->Connection: " << (*j) << endl;
  }
  os << "-->List of node types:" << endl << nodetypes;
  os << "-->List of node collections:" << endl << nodecollections;
  os << right;
  return os;
}

/// Initializing constructor
NodeTaskMon::NodeTaskMon(Interactor* parent, const string& nam, const string& typ, Inventory* inv) 
  : InventoryClient(nam,inv), m_parent(parent), m_type(typ), m_infoId(0), 
    m_changed(true), m_taskUpdate(0), m_connUpdate(0), m_numBadTasks(1), m_numBadConnections(1)
{
}

NodeTaskMon::~NodeTaskMon() {
}

/// DimInfo overload to process messages
void NodeTaskMon::infoHandler(void* tag, void* address, int* size) {
  if ( address && tag && size && *size>0 ) {
    try {
      NodeTaskMon* it = *(NodeTaskMon**)tag;
      it->updateTaskInfo((char*)address,*size);
    }
    catch(const exception& e) {
      cout << "Exception in DIM callback processing:" << e.what() << endl;
    }
    catch(...) {
      cout << "UNKNOWN exception in DIM callback processing." << endl;
    }
  }
}

/// DimInfo overload to process messages
void NodeTaskMon::pingHandler(void* tag, void* address, int* size) {
  if ( address && tag && size ) {
    try {
      NodeTaskMon* it = *(NodeTaskMon**)tag;
      it->updatePingInfo((char*)address,*size);
    }
    catch(const exception& e) {
      cout << "Exception in DIM callback processing:" << e.what() << endl;
    }
    catch(...) {
      cout << "UNKNOWN exception in DIM callback processing." << endl;
    }
  }
}

/// Update task information
void NodeTaskMon::updatePingInfo(const char* ptr, size_t len) {
  if ( len > 0 ) {
    const Connectionset* cs = (Connectionset*)ptr;
    const Connectionset::Connections& c=cs->connections;
    for(Connectionset::Connections::const_iterator ic=c.begin(); ic!=c.end();++ic)   {
      m_connections[(*ic).node] = (*ic).status>0 ? 0 : 5;
    }
    return;
  }
  cout << "Mark connections list of " << name() << " as BAD." << endl;
  for(ItemsI::iterator j=m_connections.begin(); j != m_connections.end(); ++j)
    (*j).second = 5;
}

/// Update task information
void NodeTaskMon::updateTaskInfo(const char* ptr, size_t /* len */) {
  typedef Cluster::PVSSProject _P;
  double rss = 0, vsize = 0, data = 0, stack = 0, mem = 0.0, cpu = 0.0;
  long bad = 0, good = 0;
  Items& t = m_tasks;
  Items& p = m_projects;
  Items::iterator i;
  stringstream xml;
  string pnam;
  size_t idx;
  map<string,_P> pvss;

  //cout << "DIM: Got update information:" << len << " bytes." << endl;
  for(i=t.begin(); i!=t.end();++i) (*i).second=false;
  for(i=p.begin(); i!=p.end();++i) (*i).second=false;

  NodeStats* ns = (NodeStats*)ptr;
  Procset::Processes& ps = ns->procs()->processes;
  m_numBadTasks = 0;
  for(Procset::Processes::iterator pi=ps.begin();pi!=ps.end();pi=ps.next(pi)) {
    //vector<string> res = psItems(ptr);
    Process& pr = (*pi);
    string utgid = pr.utgid;
    string cmd   = pr.cmd;
    string command = pr.cmd;
    if ( (idx=cmd.find(' ')) != string::npos ) cmd[idx]=0;
    if ( ::strcmp(cmd.c_str(),"init")==0 ) {
      xml << "\t\t<Boot time=\"" << pr.start << "\"/>" << endl;
    }
    if ( cmd.substr(0,6)=="PVSS00" || cmd.substr(0,3)=="WCC" )  {
      pnam = _P::projectName(pr.cmd);
      if ( !pnam.empty() ) {
        pvss[pnam].name=pnam;
        if ( ::strcmp(cmd.c_str(),"WCCILevent")==0 ) 
          pvss[pnam].eventMgr = true;
        else if (::strcmp(cmd.c_str(),"WCCILdata")==0 )
          pvss[pnam].dataMgr = true;
        else if (::strcmp(cmd.c_str(),"WCCILdist")==0 )
          pvss[pnam].distMgr = true;
        else if (::strcmp(cmd.c_str(),"WCCOActrl")==0 ) {
          if ( command.find("fwFsmSrvr") != string::npos ) pvss[pnam].fsmSrv = true;
          else if ( command.find("fwFsmDeviceHandler") != string::npos ) pvss[pnam].devHdlr = true;
        }
        else if ( ::strcmp(cmd.c_str(),"PVSS00event")==0 ) 
          pvss[pnam].eventMgr = true;
        else if (::strcmp(cmd.c_str(),"PVSS00data")==0 )
          pvss[pnam].dataMgr = true;
        else if (::strcmp(cmd.c_str(),"PVSS00dist")==0 )
          pvss[pnam].distMgr = true;
        else if (::strcmp(cmd.c_str(),"PVSS00ctrl")==0 ) {
          if ( command.find("fwFsmSrvr") != string::npos ) pvss[pnam].fsmSrv = true;
          else if ( command.find("fwFsmDeviceHandler") != string::npos ) pvss[pnam].devHdlr = true;
        }
      }
    }
    i=t.find(utgid);
    if ( !utgid.empty() && utgid.substr(0,3)!="N/A" && i != t.end() ) {
      mem += pr.mem;
      rss += pr.rss;
      cpu += pr.cpu;
      vsize += pr.vsize;
      stack += pr.stack;
      if ( s_debug ) {
        cout << "Check task: '" << utgid << "' " << pr.pid << " " << (void*)&pr << endl;
      }
      if ( !(*i).second ) ++good;
      (*i).second = true;
    }
  }
  bad = t.size()-good;
  m_numBadTasks   = bad;
  m_diskSize      = float(ns->localdisk.numBlocks)*float(ns->localdisk.blockSize)/float(1024*1024*1024);
  m_diskAvailible = float(ns->localdisk.freeBlocks)*float(ns->localdisk.blockSize)/float(1024*1024*1024);
  xml << "\t\t<System perc_cpu=\"" << cpu 
      << "\" perc_mem=\"" << mem 
      << "\" vsize=\"" << vsize 
      << "\" rss=\"" << rss 
      << "\" data=\"" << data
      << "\" stack=\"" << stack
      << "\"/>" << endl;
  xml << "\t\t<Localdisk blk_size=\"" << ns->localdisk.blockSize
      << "\" total=\"" << ns->localdisk.numBlocks
      << "\" availible=\"" << ns->localdisk.freeBlocks
      << "\"/>" << endl;
  xml << "\t\t<Tasks count=\"" << t.size() 
      << "\" ok=\"" << good 
      << "\" missing=\"" << bad
      << "\">" << endl;
  for(i=t.begin(); i!=t.end();++i) {
    xml << "\t\t\t<Task name=\"" << (*i).first 
        << "\" status=\"" << ((*i).second ? "OK" : "Not OK") << "\"/>" << endl;
  }
  xml << "\t\t</Tasks>";

  if ( p.size() > 0 ) {
    good = bad = 0;
    for(i=p.begin(); i!=p.end();++i) {
      const string& n = (*i).first;
      map<string,_P>::const_iterator q=pvss.find(n);
      if ( q != pvss.end() ) {
        const _P& pro = (*q).second;
        (*i).second = pro.eventMgr && pro.dataMgr && pro.distMgr;
      }
      (*i).second ? ++good : ++bad;
    }
    xml << endl
        << "\t\t<Projects count=\"" << p.size() 
        << "\" ok=\"" << good 
        << "\" missing=\"" << bad 
        << "\">" << endl;
    for(i=p.begin(); i!=p.end();++i) {
      const string& n = (*i).first;
      map<string,_P>::const_iterator q=pvss.find(n);
      if ( q == pvss.end() ) {
        xml << "\t\t\t<Project name=\"" << n
            << "\" status=\"" << "Not OK"
            << "\" event=\""  << "DEAD"
            << "\" data=\""   << "DEAD"
            << "\" dist=\""   << "DEAD"
            << "\"/>" << endl;
        continue;
      }
      const _P& pro = (*q).second;
      bool ok = pro.eventMgr && pro.dataMgr && pro.distMgr;
      xml << "\t\t\t<Project name=\"" << n
          << "\" status=\""      << (ok ? "OK" : "Not OK")
          << "\" event=\""       << (pro.eventMgr ? "RUNNING" : "DEAD")
          << "\" data=\""        << (pro.dataMgr  ? "RUNNING" : "DEAD")
          << "\" dist=\""        << (pro.distMgr  ? "RUNNING" : "DEAD")
          << "\" fsmSrv=\""      << (pro.fsmSrv   ? "RUNNING" : "DEAD")
          << "\" devHandler=\""  << (pro.devHdlr  ? "RUNNING" : "DEAD")
          << "\"/>" << endl;
    }
    xml << "\t\t</Projects>";
  }
  m_taskStatus = xml.str();
  m_taskUpdate = ::time(0);
  m_changed = true;
  IocSensor::instance().send(parent(),CMD_DATA,this);
}

/// Start the monitoring object
int NodeTaskMon::start()   {
  Inventory* inv = inventory();
  Inventory::NodeTypeMap::iterator i = inv->nodetypes.find(type());
  if ( i != inv->nodetypes.end() ) {
    size_t idx;
    string nam, nodU = strupper(m_name), nodL=strlower(m_name);
    nam = "/"+nodL+"/ROcollect/Statistics";
    m_nodeType = (*i).second;
    for(TaskList::iterator t=m_nodeType.tasks.begin(); t!=m_nodeType.tasks.end();++t) {
      string& utgid = *t;
      if ( (idx=utgid.find("<Node>")) != string::npos ) utgid.replace(idx,idx+6,nodL);
      m_tasks[utgid] = false;
    }
    for(ProjectList::iterator p=m_nodeType.projects.begin(); p!=m_nodeType.projects.end();++p) {
      string& n = *p;
      if ( (idx=n.find("<NODE>")) != string::npos ) n.replace(idx,idx+6,nodU);
      m_projects[n] = false;
    }
    for(ConnectionList::iterator c=m_nodeType.connections.begin(); c!=m_nodeType.connections.end();++c) {
      string& n = *c;
      if ( (idx=n.find("<DIM_DNS_NODE>")) != string::npos )
        n.replace(idx,idx+14,::getenv("DIM_DNS_NODE"));
      else if ( (idx=n.find("<NODE>")) != string::npos )
        n.replace(idx,idx+6,nodL);
      else if ( (idx=n.find("<DATAINTERFACE>")) != string::npos )
        n.replace(idx,idx+15,nodL+"-d1");
      m_connections[n] = 0;
    }
    ::lib_rtl_output(LIB_RTL_INFO,"Subscribing to service:%s.",nam.c_str());
    m_infoId = ::dic_info_service((char*)nam.c_str(),MONITORED,0,0,0,infoHandler,(long)this,0,0);
    nam = "/"+nodL+"/PingSrv";
    ::lib_rtl_output(LIB_RTL_INFO,"Subscribing to service:%s.",nam.c_str());
    m_pingId = ::dic_info_service((char*)nam.c_str(),MONITORED,0,0,0,pingHandler,(long)this,0,0);
    TimeSensor::instance().add(this,10,(void*)CMD_DATA);
    return 1;
  }
  m_error = "No setup information present for node monitor "+name()+" of type "+type();
  return 0;
}
 
/// Stop the monitoring object
int NodeTaskMon::stop()   {
  if ( m_pingId ) {
    ::dic_release_service(m_pingId);
    m_pingId = 0;
  }
  if ( m_infoId ) {
    ::dic_release_service(m_infoId);
    m_infoId = 0;
  }
  TimeSensor::instance().remove(this);
  return 1;
}
 
/// Encode connection status information in XML
const string& NodeTaskMon::updateConnections() {
  ItemsI::iterator  j;
  stringstream  xml;
  int good=0, bad=0;
  time_t now = time(0);
  bool   recheck = false;
  bool   force = now-m_connUpdate > CONNECTION_UPDATE_TIMEDIFF;
  
  m_numBadConnections = 0;
  recheck = true;
  
#if 0
  for(j=m_connections.begin(); j != m_connections.end(); ++j)
    if ( (*j).second > 0 ) recheck = true;
#endif
  if ( recheck || force ) {
    // cout << "Updating connection status of " << name() << endl;

#if 0
    for(j=m_connections.begin(); j != m_connections.end(); ++j) {
      if ( force || (*j).second > 0 ) {
        MethodCall c("ping",Args((*j).first,2,9000));
        //cout << " ... PING " << (*j).first << " from " << name() << endl;
        MethodResponse r(Server(name(),8088)(c));
        //r.print();
        (*j).second++;
        if ( !r.hasError() ) {
          list<Arg>* l = (list<Arg>*)r.result().data;
          if ( l->size() >= 1 ) {
            l = (list<Arg>*)(*l->begin()).data.tuple->data;
            for(list<Arg>::const_iterator i=l->begin(); i != l->end(); ++i) {
              if ( (*i).type==Arg::STRING && ::strstr((*i).data.str," 100% packet loss")==0) 
                (*j).second = 0;
            }
          }
        }
      }
    }
    for(j=m_connections.begin(); j != m_connections.end(); ++j) 
      (*j).second<5 ? ++good : ++bad;
#endif
    for(j=m_connections.begin(); j != m_connections.end(); ++j) 
      (*j).second==0 ? ++good : ++bad;
    m_numBadConnections = bad;
    xml << "\t\t<Connections count=\"" << good
        << "\" ok=\"" << good
        << "\" missing=\"" << bad
        << "\">" << endl;
    for(j=m_connections.begin(); j != m_connections.end(); ++j) {
      xml << "\t\t\t<Connection name=\"" << (*j).first << "\" status=\""
          << ( (*j).second<5 ? "OK" : "Not OK") << "\"/>" << endl;
    }
    xml << "\t\t</Connections>";
    DimLock lock;
    m_connUpdate = time(0);
    m_connStatus = xml.str();
    return m_connStatus;
  }
  return m_connStatus;
}

/// Publish monitoring information
int NodeTaskMon::publish()   {
  m_state = ::time(0)-m_taskUpdate > TASK_UPDATE_TIMEDIFF ? DEAD : ALIVE;
  return 1;
}

/// Handle interaction event
void NodeTaskMon::handle(const Event& ev) {
  switch(ev.eventtype) {
  case TimeEvent:
    if (ev.timer_data == (void*)CMD_DATA ) {
      updateConnections();
      publish();
      TimeSensor::instance().add(this,10,(void*)CMD_DATA);
    }
    break;
  case IocEvent:
    break;
  default:
    break;
  }
}

/// Initializing constructor
SubfarmTaskMon::SubfarmTaskMon(const string& nam, Inventory* inv)
  : InventoryClient(nam,inv), m_serviceID(0), m_summaryID(0), m_summary(0)
{
  Inventory::NodeCollectionMap::const_iterator i = inventory()->nodecollections.find(name());
  if ( i != inv->nodecollections.end() ) {    
    m_nodeList = (*i).second.nodes;
    m_data = "";
    return;
  }
  string msg = "No setup information present for subfarm task monitor "+name();
  ::lib_rtl_output(LIB_RTL_FATAL,msg.c_str());
  throw runtime_error(msg);
}

/// Start the monitoring object
int SubfarmTaskMon::start() {
  string svc = "/"+strupper(name())+"/TaskSupervisor/Status";
  ::dic_set_dns_node((char*)name().c_str());

  m_summary = (SubfarmSummary*)(new char[sizeof(SubfarmSummary)+(m_nodeList.size()+1)*sizeof(NodeSummary)]);
  m_summary = new(m_summary) SubfarmSummary(name());

  SubfarmSummary::Nodes::iterator ni=m_summary->nodes.begin();
  for(NodeList::const_iterator j=m_nodeList.begin(); j != m_nodeList.end(); ++j) {
    m_nodes.insert(make_pair((*j).first,new NodeTaskMon(this,(*j).first,(*j).second,inventory())));
    ni = ::new(ni) NodeSummary((*j).first);
    ni = m_summary->nodes.add(ni);
  }

  ManipTaskMon m("Failed to start node monitor",&NodeTaskMon::start);
  for_each(m_nodes.begin(),m_nodes.end(),m);
  if ( 0 != m_serviceID ) ::dis_remove_service(m_serviceID);
  ::dis_set_dns_node((char*)PUBLISHING_NODE.c_str());
  m_serviceID = ::dis_add_service((char*)svc.c_str(),(char*)"C",0,0,feedData,(long)this);
  cout << "Service " << svc << " [" << PUBLISHING_NODE 
       << "] with monitoring information of " << name() << " started." << endl;
  svc = "/"+strupper(name())+"/TaskSupervisor/Summary";
  m_summaryID = ::dis_add_service((char*)svc.c_str(),(char*)"C",0,0,feedSummary,(long)this);
  return m.check();
}

/// Stop the monitoring object
int SubfarmTaskMon::stop() {
  if ( 0 != m_serviceID ) ::dis_remove_service(m_serviceID);
  if ( 0 != m_summaryID ) ::dis_remove_service(m_summaryID);
  ManipTaskMon m("Failed to stop node monitor",&NodeTaskMon::stop);
  m_serviceID = 0;
  m_summaryID = 0;
  if ( m_summary ) delete [] (char*)m_summary;
  m_summary = 0;
  for_each(m_nodes.begin(),m_nodes.end(),m);
  int res = m.check(false);
  clear(m_nodes);
  return res;
}

/// Start the monitoring object
int SubfarmTaskMon::publish() {
  if ( m_serviceID ) {
    char time_buf[128];
    stringstream xml;
    size_t good=0, bad=0;
    float disk_free = 0.0, disk_total = 0.0;
    Monitors::const_iterator i;
    SubfarmSummary::Nodes::iterator ni;
    string status="DEAD", txt = ::lib_rtl_timestr(TIME_FORMAT);

    for(i=m_nodes.begin(); i!=m_nodes.end(); ++i) {
      NodeTaskMon* n=(*i).second;
      n->state()==NodeTaskMon::ALIVE ? ++good : ++bad;
      disk_free += n->diskSize();
      disk_total += n->diskAvailible();
    }
    if ( bad == 0 && good == m_nodes.size() ) status = "ALIVE";
    else if ( bad>0 && good>0               ) status = "MIXED";
    else if ( bad == m_nodes.size()         ) status = "DEAD";
    xml << "<Cluster name=\"" << name() 
        << "\" status=\"" << status
        << "\" time=\"" << txt.c_str()+11
        << "\" nodes=\"" << m_nodes.size()
        << "\" disk_total=\"" << disk_total
        << "\" disk_availible=\"" << disk_free
        << "\">" << endl;

    { DimLock lock;
      m_summary->time = (int)::time(0);
      for(i=m_nodes.begin(), ni=m_summary->nodes.begin(); i!=m_nodes.end(); ++i, ni=m_summary->nodes.next(ni)) {
        NodeTaskMon* n=(*i).second;
        n->publish();
        ::strftime(time_buf,sizeof(time_buf),TIME_FORMAT,::localtime(&n->taskUpdate())) ;
        xml << "\t<Node name=\"" << n->name() 
            << "\" status=\"" << (char*)(n->state()==NodeTaskMon::ALIVE ? "ALIVE" : "DEAD")
            << "\" time=\"" << time_buf
            << "\">" << endl;
        if ( n->state() == NodeTaskMon::ALIVE ) {
          xml << n->taskStatus() << endl
              << n->connectionStatus() << endl;
          (*ni).state = NodeSummary::ALIVE;
          (*ni).numBadTasks = n->numBadTasks();
          (*ni).numBadConnections = n->numBadConnections();
          (*ni).status = n->numBadTasks()>0 || n->numBadConnections()>0 ? NodeSummary::BAD : NodeSummary::OK;
          (*ni).diskSize = n->diskSize();
          (*ni).diskAvailible = n->diskAvailible();
        }
        else {
          (*ni).state = NodeSummary::DEAD;
          (*ni).status = NodeSummary::BAD;
          (*ni).numBadTasks = 1;
          (*ni).numBadConnections = 1;
          (*ni).diskSize = 0;
          (*ni).diskAvailible = 0;
        }
        (*ni).time = (int)n->taskUpdate();
        xml << "\t</Node>\n";
      }
      xml << "</Cluster>\n";
      m_data = xml.str();
    }
    if ( s_debug ) {
      cout << txt << "> Publish monitoring information for cluster " << name() << endl
           << "'" << endl << m_data  << endl << "'" << endl;
    }
    ::dis_update_service(m_serviceID);
  }
  if ( m_summaryID ) {
    ::dis_update_service(m_summaryID);
  }
  return 1;
}

/// DIM callback on dis_update_service
void SubfarmTaskMon::feedData(void* tag, void** buf, int* size, int* /* first */) {
  SubfarmTaskMon* h = *(SubfarmTaskMon**)tag;
  *size = h->m_data.length()+1;
  *buf = (void*)h->m_data.c_str();
}

/// DIM callback on dis_update_service
void SubfarmTaskMon::feedSummary(void* tag, void** buf, int* size, int* /* first */) {
  SubfarmTaskMon* h = *(SubfarmTaskMon**)tag;
  ro_gettime(&h->m_summary->time,(unsigned int*)&h->m_summary->millitm);
  *size = h->m_summary->length();
  *buf = (void*)h->m_summary;
}

/// Handle interaction event
void SubfarmTaskMon::handle(const Event& ev) {
  try {
    switch(ev.eventtype) {
    case TimeEvent:
      if (ev.timer_data == (void*)CMD_DATA ) {
        publish();
        TimeSensor::instance().add(this,PUBLISH_TIMEDIFF,(void*)CMD_DATA);
      }
      break;
    case IocEvent:
      switch(ev.type) {
      case CMD_DATA:
        //publish();
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
  }
  catch(const exception& e) {
    cout << "Exception in callback processing:" << e.what() << endl;
  }
  catch(...) {
    cout << "UNKNOWN exception in callback processing." << endl;
  }
}

static void help_TaskSupervisor() {
  ::printf("You have to supply a file name\n");
  ::exit(0);
}


#include "ROMon/TaskSupervisorParser.h"
extern "C" int run_tasksupervisor(int argc, char** argv) {
  ROMon::Inventory inv;
  string server, fname, inventory, node = RTL::nodeNameShort();
  {
    RTL::CLI cli(argc,argv,help_TaskSupervisor);
    cli.getopt("inventory",3,inventory);
    cli.getopt("input",3,fname);
    cli.getopt("node",1,node);
    cli.getopt("dns",2,PUBLISHING_NODE);
    if ( cli.getopt("debug",2) ) s_debug = true;
    if ( fname.empty() ) help_TaskSupervisor();
    ::lib_rtl_install_printer(ro_rtl_print,(void*)LIB_RTL_INFO);
    node = strupper(node);
    XML::TaskSupervisorParser ts;
    ts.parseFile(inventory);
    ts.getInventory(inv);
    ts.parseFile(fname);
    ts.getInventory(inv);
  }
  if ( s_debug ) {
    cout << "Input file is:" << fname << " Node:" << node << endl;  
    cout << inv << endl;
  }
  SubfarmTaskMon mon(node,&inv);
  mon.start();
  TimeSensor::instance().add(&mon,1,(void*)CMD_DATA);
  server = "/"+node+"/"+RTL::processName();
  ::dis_start_serving((char*)server.c_str());
  TimeSensor::instance().run();
  return 1;
}
