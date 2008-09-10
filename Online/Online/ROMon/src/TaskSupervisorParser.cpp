#include "TaskSupervisorParser.h"
#include <iostream>
#include <iomanip>

using namespace xercesc;
using namespace std;
using namespace XML;

static const XMLTag BOOT              ( "Boot");
static const XMLTag SYSTEM            ( "System");
static const XMLTag TASKS             ( "Tasks");
static const XMLTag TASKNODE          ( "Task");
static const XMLTag CONNECTIONS       ( "Connections");
static const XMLTag CONNECTIONNODE    ( "Connection");

static const XMLTag Attr_name         ( "name");
static const XMLTag Attr_status       ( "status");
static const XMLTag Attr_count        ( "count");
static const XMLTag Attr_time         ( "time");
static const XMLTag Attr_type         ( "type");
static const XMLTag Attr_missing      ( "missing");
static const XMLTag Attr_ok           ( "ok");

static const XMLTag Attr_rss          ( "rss");
static const XMLTag Attr_data         ( "data");
static const XMLTag Attr_stack        ( "stack");
static const XMLTag Attr_vsize        ( "vsize");
static const XMLTag Attr_perc_mem     ( "perc_mem");
static const XMLTag Attr_perc_cpu     ( "perc_cpu");

static const std::string Status_OK("OK");

using namespace ROMon;

ostream& operator<<(ostream& os, const std::vector<Cluster::Node::Item>& t) {
  for(Cluster::Node::Tasks::const_iterator i=t.begin(); i!=t.end();++i)
    os << setw(12) << left << (*i).first << " " << (const char*)((*i).second?"/OK":"/NOT OK");
  return os;
}

ostream& operator<<(ostream& os, const Cluster::Node& n) {
  os << "    Node:" << setw(12) << left << n.name << "   [" << n.status << "]" << endl
     << "       Tasks:      " << n.tasks << std::endl
     << "       Connections:" << n.conns << std::endl;
  return os;
}

ostream& operator<<(ostream& os, const Cluster& c) {
  os << "  Cluster:" << setw(12) << left << c.name << "  [" << c.status << "]" << endl;
  for(Cluster::Nodes::const_iterator i=c.nodes.begin(); i!=c.nodes.end();++i)
    os << (*i).second;
  return os;
}

ostream& operator<<(ostream& os, const std::list<Cluster>& c) {
  for(TaskSupervisorParser::Clusters::const_iterator i=c.begin(); i!=c.end();++i)
    os << "-------------------------------------------------------------" << endl << *i;
  return os;
}

std::ostream& operator<<(std::ostream& os, const Inventory& inv)  {
  typedef Inventory::TaskMap           _TM;
  typedef Inventory::TaskList          _TL;
  typedef Inventory::TaskListMap       _TLM;
  typedef Inventory::ConnectionList    _CL;
  typedef Inventory::ConnectionListMap _CLM;
  typedef Inventory::NodeType          _NT;
  typedef Inventory::NodeTypeMap       _NTM;

  os << "-->List of all tasks:" << std::endl;
  for(_TM::const_iterator i=inv.tasks.begin();i!=inv.tasks.end();++i)
    os << "     -->Task: " << (*i).first << std::endl;

  os << "-->List of all tasks lists:" << std::endl;
  for(_CLM::const_iterator i=inv.connlists.begin();i!=inv.connlists.end();++i) {
    os << "     -->TaskList: " << (*i).first << std::endl;
    for(_TL::const_iterator j=(*i).second.begin();j!=(*i).second.end();++j)
      os << "       -->Task: " << (*j) << std::endl;
  }
  os << "-->List of all connections lists:" << std::endl;
  for(_CLM::const_iterator m=inv.connlists.begin();m!=inv.connlists.end();++m) {
    os << "     -->ConnectionList: " << (*m).first << std::endl;
    for(_CL::const_iterator j=(*m).second.begin(); j != (*m).second.end();++j)
      os << "       -->Connection: " << (*j) << std::endl;
  }
  // Build whole Node type
  for(_NTM::const_iterator i=inv.nodetypes.begin(); i != inv.nodetypes.end();++i)  {
    const _NT& nt = (*i).second;
    os << "Node Type:" << (*i).first << std::endl;
    for(_TL::const_iterator j=nt.tasks.begin();j!=nt.tasks.end();++j)
      os << "    Task:" << (*j) << std::endl;
    for(_CL::const_iterator k=nt.connections.begin(); k != nt.connections.end();++k)
      os << "    Connection:" << (*k) << std::endl;
  }
  return os;
}

// ----------------------------------------------------------------------------
TaskSupervisorParser::TaskSupervisorParser() : XMLDocument()   {
}
// ----------------------------------------------------------------------------
TaskSupervisorParser::~TaskSupervisorParser()   {  
}
// ----------------------------------------------------------------------------
void TaskSupervisorParser::getNodes(DOMNode* fde, Cluster& cluster) const {
  typedef Cluster::Node::Tasks Tasks;
  typedef Cluster::Node::Connections Connections;
  XMLElement elt(fde);
  cluster.name   = elt.attr(Attr_name);
  cluster.time   = elt.attr(Attr_time);
  cluster.status = elt.attr(Attr_status);
  cluster.nodes.clear();
  for(XMLCollection c(child(fde,"Node"), false); c; ++c) {
    XMLElement b(child(c,BOOT));
    XMLElement s(child(c,SYSTEM));
    XMLElement e(child(c,TASKS));
    XMLElement g(child(c,CONNECTIONS));
    std::string nam = c.attr(Attr_name);
    cluster.nodes.insert(std::make_pair(nam,Node(nam,c.attr(Attr_status))));
    Node& node = cluster.nodes[nam];
    Connections& conns = node.conns;
    Tasks& tasks = node.tasks;

    node.time = c.attr(Attr_time);
    tasks.clear();
    conns.clear();
    node.totalTaskCount = node.missTaskCount = node.taskCount = 0;
    node.totalConnCount = node.missConnCount = node.connCount = 0;
    node.rss = node.vsize = node.data = node.stack = 0;
    node.perc_cpu = node.perc_mem = 0.;
    if ( b ) {
      node.boot = b.attr(Attr_time);
    }
    if ( s ) {
      node.rss = ::atoi(s.attr(Attr_rss).c_str());
      node.data = ::atoi(s.attr(Attr_data).c_str());
      node.stack = ::atoi(s.attr(Attr_stack).c_str());
      node.vsize = ::atoi(s.attr(Attr_vsize).c_str());
      node.perc_cpu = ::atof(s.attr(Attr_perc_cpu).c_str());
      node.perc_mem = ::atof(s.attr(Attr_perc_mem).c_str());
    }
    if ( e ) {
      node.taskCount      = ::atoi(e.attr(Attr_ok).c_str());
      node.totalTaskCount = ::atoi(e.attr(Attr_count).c_str());
      node.missTaskCount  = ::atoi(e.attr(Attr_missing).c_str());
      for(XMLCollection t(child(e,TASKNODE), false); t; ++t)
	tasks.push_back(std::make_pair(t.attr(Attr_name),t.attr(Attr_status)==Status_OK));
    }
    if ( g ) {
      node.connCount      = ::atoi(g.attr(Attr_ok).c_str());
      node.totalConnCount = ::atoi(g.attr(Attr_count).c_str());
      node.missConnCount  = ::atoi(g.attr(Attr_missing).c_str());
      for(XMLCollection t(child(g,CONNECTIONNODE), false); t; ++t)
	conns.push_back(std::make_pair(t.attr(Attr_name),t.attr(Attr_status)==Status_OK));
    }
  }
}

// ----------------------------------------------------------------------------
void TaskSupervisorParser::getClusterNodes(Cluster& cluster) const {
  DOMNode* fde = getDoc(true)->getElementsByTagName(XMLStr("*"))->item(0);
  getNodes(fde,cluster);
}

// ----------------------------------------------------------------------------
void TaskSupervisorParser::getClusters(Clusters& clusters) const {
  DOMNode* fde = getDoc(true)->getElementsByTagName(XMLStr("*"))->item(0);
  for(XMLCollection c(child(fde,"Cluster"), false); c; ++c) {
    clusters.push_back(Cluster(c.attr(Attr_name),c.attr(Attr_status)));
    getNodes(c,clusters.back());
  }
}

// ----------------------------------------------------------------------------
void TaskSupervisorParser::getInventory(Inventory& inv) const {
  typedef Inventory::Task              _T;
  typedef Inventory::TaskList          _TL;
  typedef Inventory::TaskListMap       _TLM;
  typedef Inventory::ConnectionList    _CL;
  typedef Inventory::ConnectionListMap _CLM;
  typedef Inventory::NodeType          _NT;
  typedef Inventory::NodeTypeMap       _NTM;
  DOMNode* fde = getDoc(true)->getElementsByTagName(XMLStr("*"))->item(0);

  for(XMLCollection c(child(fde,"Task"), false); c; ++c) {
    inv.tasks[c.attr(Attr_name)] = _T(c.attr(Attr_name),c.attr(Attr_type));
    _T& t = inv.tasks[c.attr(Attr_name)];
    XMLElement items;
    t.path = items(child(c,"Path")).value();
    t.user = items(child(c,"UserName")).value();
    t.dimDns = items(child(c,"DimDnsNode")).value();
    t.dimSvc = items(child(c,"DimServiceName")).value();
    t.responsible = items(child(c,"Responsible")).value();
  }
  for(XMLCollection c(child(fde,"TaskList"), false); c; ++c) {
    _TL& tl = inv.tasklists[c.attr(Attr_name)];
    for(XMLCollection t(child(c,"Task"), false); t; ++t)
      tl.push_back(t.attr(Attr_name));
  }
  for(XMLCollection c(child(fde,"ConnectionList"), false); c; ++c) {
    _CL& cl = inv.connlists[c.attr(Attr_name)];
    for(XMLCollection t(child(c,"Connection"), false); t; ++t)
      cl.push_back(t.attr(Attr_name));
  }
  // Build whole Node type
  for(XMLCollection c(child(fde,"NodeType"),false); c; ++c) {
    _NT& nt = inv.nodetypes[c.attr(Attr_name)];
    for(XMLCollection t(child(c,"TaskList"),false,"TaskList"); t; ++t) {
      const _TL& tl = inv.tasklists[t.attr(Attr_name)];
      nt.tasks.insert(nt.tasks.end(),tl.begin(),tl.end());
    }
    for(XMLCollection t(child(c,"ConnectionList"), false,"ConnectionList"); t; ++t) {
      const _CL& cl = inv.connlists[t.attr(Attr_name)];
      nt.connections.insert(nt.connections.end(),cl.begin(),cl.end());
    }
    for(XMLCollection t(child(c,"Task"), false); t; ++t)
      nt.tasks.push_back(t.attr(Attr_name));
    for(XMLCollection t(child(c,"Connection"), false); t; ++t)
      nt.tasks.push_back(t.attr(Attr_name));
  }
}


class ClusterCheck {
};


#include "RTL/rtl.h"
#include <cstdarg>

static void help_cluster() {
  ::printf("You have to supply a file name\n");
  ::exit(0);
}

static size_t prt(void*,int,const char* fmt,va_list args) {
  size_t result;
  std::string format = fmt;
  format += "\n";
  result = ::vfprintf(stdout, format.c_str(), args);
  ::fflush(stdout);
  return result;
}


static std::string getInput(int argc, char** argv) {
  RTL::CLI cli(argc,argv,help_cluster);
  std::string fname;
  cli.getopt("input",1,fname);
  if ( fname.empty() ) help_cluster();
  ::lib_rtl_install_printer(prt,0);
  return fname;
}

extern "C" int tsksup_inventory(int argc, char** argv) {
  std::string fname=getInput(argc,argv);
  TaskSupervisorParser ts;
  ts.parseFile(fname);
  ROMon::Inventory inv;
  ts.getInventory(inv);
  std::cout << inv << std::endl;
  return 1;
}

extern "C" int tsksup_cluster(int argc, char** argv) {
  std::string fname=getInput(argc,argv);
  TaskSupervisorParser ts;
  ts.parseFile(fname);
  Cluster cluster;
  ts.getClusterNodes(cluster);
  std::cout << cluster << std::endl;
  return 1;
}
extern "C" int tsksup_network(int argc, char** argv) {
  std::string fname=getInput(argc,argv);
  TaskSupervisorParser ts;
  TaskSupervisorParser::Clusters clusters;
  ts.parseFile(fname);
  ts.getClusters(clusters);
  std::cout << clusters << std::endl;
  return 1;
}
