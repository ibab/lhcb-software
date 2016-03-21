#include "ROMon/TaskSupervisorParser.h"
#include <iostream>
#include <iomanip>
#include <ctime>

using namespace xercesc;
using namespace std;
using namespace XML;

#if 0
static const XML_Init __init__;
static const XMLTag BOOT              ( "Boot");
static const XMLTag SYSTEM            ( "System");
static const XMLTag TASKS             ( "Tasks");
static const XMLTag LOCALDISK         ( "Localdisk");
static const XMLTag TASKNODE          ( "Task");
static const XMLTag NODE              ( "Node");
static const XMLTag CONNECTIONS       ( "Connections");
static const XMLTag CONNECTIONNODE    ( "Connection");
static const XMLTag PROJECTS          ( "Projects");
static const XMLTag PROJECTNODE       ( "Project");

static const XMLTag Attr_name         ( "name");
static const XMLTag Attr_status       ( "status");
static const XMLTag Attr_count        ( "count");
static const XMLTag Attr_time         ( "time");
static const XMLTag Attr_type         ( "type");
static const XMLTag Attr_missing      ( "missing");
static const XMLTag Attr_ok           ( "ok");
static const XMLTag Attr_eventMgr     ( "event");
static const XMLTag Attr_dataMgr      ( "data");
static const XMLTag Attr_distMgr      ( "dist");
static const XMLTag Attr_fsmSrv       ( "fsmSrv");
static const XMLTag Attr_devHdlr      ( "devHandler");

static const XMLTag Attr_rss          ( "rss");
static const XMLTag Attr_data         ( "data");
static const XMLTag Attr_stack        ( "stack");
static const XMLTag Attr_vsize        ( "vsize");
static const XMLTag Attr_perc_mem     ( "perc_mem");
static const XMLTag Attr_perc_cpu     ( "perc_cpu");
static const XMLTag Attr_blk_size     ( "blk_size");
static const XMLTag Attr_blk_total    ( "total");
static const XMLTag Attr_blk_availible( "availible");
static const XMLTag Attr_dev_num      ( "numDevices");
static const XMLTag Attr_dev_good     ( "goodDevices");
#endif
namespace {
  struct Constants  {
    XML_Init __init__;
    XMLTag BOOT              ; // ( "Boot");
    XMLTag SYSTEM            ; // ( "System");
    XMLTag TASKS             ; // ( "Tasks");
    XMLTag LOCALDISK         ; // ( "Localdisk");
    XMLTag TASKNODE          ; // ( "Task");
    XMLTag NODE              ; // ( "Node");
    XMLTag CONNECTIONS       ; // ( "Connections");
    XMLTag CONNECTIONNODE    ; // ( "Connection");
    XMLTag PROJECTS          ; // ( "Projects");
    XMLTag PROJECTNODE       ; // ( "Project");

    XMLTag Attr_name         ; // ( "name");
    XMLTag Attr_status       ; // ( "status");
    XMLTag Attr_count        ; // ( "count");
    XMLTag Attr_time         ; // ( "time");
    XMLTag Attr_type         ; // ( "type");
    XMLTag Attr_missing      ; // ( "missing");
    XMLTag Attr_ok           ; // ( "ok");
    XMLTag Attr_eventMgr     ; // ( "event");
    XMLTag Attr_dataMgr      ; // ( "data");
    XMLTag Attr_distMgr      ; // ( "dist");
    XMLTag Attr_fsmSrv       ; // ( "fsmSrv");
    XMLTag Attr_devHdlr      ; // ( "devHandler");

    XMLTag Attr_rss          ; // ( "rss");
    XMLTag Attr_data         ; // ( "data");
    XMLTag Attr_stack        ; // ( "stack");
    XMLTag Attr_vsize        ; // ( "vsize");
    XMLTag Attr_perc_mem     ; // ( "perc_mem");
    XMLTag Attr_perc_cpu     ; // ( "perc_cpu");
    XMLTag Attr_blk_size     ; // ( "blk_size");
    XMLTag Attr_blk_total    ; // ( "total");
    XMLTag Attr_blk_availible; // ( "availible");
    XMLTag Attr_dev_num      ; // ( "dev_num");
    XMLTag Attr_dev_good     ; // ( "dev_good");
    Constants();
  };

  Constants::Constants() :
    BOOT              ( "Boot"),
    SYSTEM            ( "System"),
    TASKS             ( "Tasks"),
    LOCALDISK         ( "Localdisk"),
    TASKNODE          ( "Task"),
    NODE              ( "Node"),
    CONNECTIONS       ( "Connections"),
    CONNECTIONNODE    ( "Connection"),
    PROJECTS          ( "Projects"),
    PROJECTNODE       ( "Project"),

    Attr_name         ( "name"),
    Attr_status       ( "status"),
    Attr_count        ( "count"),
    Attr_time         ( "time"),
    Attr_type         ( "type"),
    Attr_missing      ( "missing"),
    Attr_ok           ( "ok"),
    Attr_eventMgr     ( "event"),
    Attr_dataMgr      ( "data"),
    Attr_distMgr      ( "dist"),
    Attr_fsmSrv       ( "fsmSrv"),
    Attr_devHdlr      ( "devHandler"),

    Attr_rss          ( "rss"),
    Attr_data         ( "data"),
    Attr_stack        ( "stack"),
    Attr_vsize        ( "vsize"),
    Attr_perc_mem     ( "perc_mem"),
    Attr_perc_cpu     ( "perc_cpu"),
    Attr_blk_size     ( "blk_size"),
    Attr_blk_total    ( "total"),
    Attr_blk_availible( "availible"),
    Attr_dev_num      ( "dev_num"),
    Attr_dev_good     ( "dev_good")
  {}
}

static const string Status_OK("OK");
static Constants* gc = 0;
using namespace ROMon;

// ----------------------------------------------------------------------------
TaskSupervisorParser::TaskSupervisorParser() : XMLDocument()   {
  if ( 0 == gc ) gc = new Constants;
}
// ----------------------------------------------------------------------------
TaskSupervisorParser::~TaskSupervisorParser()   {
}
// ----------------------------------------------------------------------------
void TaskSupervisorParser::getNodes(DOMNode* fde, Cluster& cluster) const {
  XMLElement elt(fde);
  cluster.name   = elt.attr(gc->Attr_name);
  cluster.time   = elt.attr(gc->Attr_time);
  cluster.status = elt.attr(gc->Attr_status);
  cluster.nodes.clear();
  for(XMLCollection c(child(fde,"Node"), false); c; ++c) {
    XMLElement b(child(c,gc->BOOT));
    XMLElement s(child(c,gc->SYSTEM));
    XMLElement l(child(c,gc->LOCALDISK));
    XMLElement e(child(c,gc->TASKS));
    XMLElement p(child(c,gc->PROJECTS));
    XMLElement g(child(c,gc->CONNECTIONS));
    string nam = c.attr(gc->Attr_name);
    cluster.nodes.insert(make_pair(nam,Node(nam,c.attr(gc->Attr_status))));
    Node& node = cluster.nodes[nam];

    cluster.time = c.attr(gc->Attr_time);
    node.time = c.attr(gc->Attr_time);
    node.tasks.clear();
    node.conns.clear();
    node.projects.clear();
    node.blk_size = 0;
    node.blk_total = 0;
    node.blk_availible = 0;
    node.dev_good = 0;
    node.dev_num = 0;
    node.totalTaskCount = node.missTaskCount = node.taskCount = 0;
    node.totalConnCount = node.missConnCount = node.connCount = 0;
    node.rss = node.vsize = node.data = node.stack = 0;
    node.perc_cpu = node.perc_mem = 0.;
    if ( b ) {
      char buff[64];
      ::memset(buff,0,sizeof(buff));
      time_t boot_time = ::atoi(b.attr(gc->Attr_time).c_str());
      ::strftime(buff,sizeof(buff),"%Y-%m-%d %H:%M:%S",::localtime(&boot_time));
      node.boot = buff;
    }
    if ( l ) {
      node.dev_num   = ::atol(l.attr(gc->Attr_dev_num).c_str());
      node.dev_good  = ::atol(l.attr(gc->Attr_dev_good).c_str());
      node.blk_size  = ::atol(l.attr(gc->Attr_blk_size).c_str());
      node.blk_total = ::atol(l.attr(gc->Attr_blk_total).c_str());
      node.blk_availible = ::atol(l.attr(gc->Attr_blk_availible).c_str());
    }
    if ( s ) {
      node.rss = ::atoi(s.attr(gc->Attr_rss).c_str());
      node.data = ::atoi(s.attr(gc->Attr_data).c_str());
      node.stack = ::atoi(s.attr(gc->Attr_stack).c_str());
      node.vsize = ::atoi(s.attr(gc->Attr_vsize).c_str());
      node.perc_cpu = (float)::atof(s.attr(gc->Attr_perc_cpu).c_str());
      node.perc_mem = (float)::atof(s.attr(gc->Attr_perc_mem).c_str());
    }
    if ( e ) {
      node.taskCount      = ::atoi(e.attr(gc->Attr_ok).c_str());
      node.totalTaskCount = ::atoi(e.attr(gc->Attr_count).c_str());
      node.missTaskCount  = ::atoi(e.attr(gc->Attr_missing).c_str());
      for(XMLCollection t(child(e,gc->TASKNODE), false); t; ++t)
        node.tasks.push_back(make_pair(t.attr(gc->Attr_name),t.attr(gc->Attr_status)==Status_OK));
    }
    if ( g ) {
      node.connCount      = ::atoi(g.attr(gc->Attr_ok).c_str());
      node.totalConnCount = ::atoi(g.attr(gc->Attr_count).c_str());
      node.missConnCount  = ::atoi(g.attr(gc->Attr_missing).c_str());
      for(XMLCollection t(child(g,gc->CONNECTIONNODE), false); t; ++t)
        node.conns.push_back(make_pair(t.attr(gc->Attr_name),t.attr(gc->Attr_status)==Status_OK));
    }
    if ( p ) {
      for(XMLCollection t(child(p,gc->PROJECTNODE), false); t; ++t) {
        node.projects.push_back(Cluster::PVSSProject());
        Cluster::PVSSProject& pr = node.projects.back();
        pr.name     = t.attr(gc->Attr_name);
        pr.eventMgr = t.attr(gc->Attr_eventMgr)=="RUNNING";
        pr.dataMgr  = t.attr(gc->Attr_dataMgr)=="RUNNING";
        pr.distMgr  = t.attr(gc->Attr_distMgr)=="RUNNING";
        pr.devHdlr  = t.attr(gc->Attr_devHdlr)=="RUNNING";
        pr.fsmSrv   = t.attr(gc->Attr_fsmSrv)=="RUNNING";
      }
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
    clusters.push_back(Cluster(c.attr(gc->Attr_name),c.attr(gc->Attr_status)));
    getNodes(c,clusters.back());
  }
}

// ----------------------------------------------------------------------------
void TaskSupervisorParser::getInventory(Inventory& inv) const {
  typedef Inventory::Task              _T;
  typedef Inventory::TaskList          _TL;
  typedef Inventory::ConnectionList    _CL;
  typedef Inventory::NodeType          _NT;
  typedef Inventory::NodeCollection    _NC;
  DOMNode* fde = getDoc(true)->getElementsByTagName(XMLStr("*"))->item(0);

  for(XMLCollection c(child(fde,"Task"), false, "Task"); c; ++c) {
    inv.tasks[c.attr(gc->Attr_name)] = _T(c.attr(gc->Attr_name),c.attr(gc->Attr_type));
    _T& t = inv.tasks[c.attr(gc->Attr_name)];
    XMLElement items;
    t.path = items(child(c,"Path")).value();
    t.user = items(child(c,"UserName")).value();
    t.dimDns = items(child(c,"DimDnsNode")).value();
    t.dimSvc = items(child(c,"DimServiceName")).value();
    t.responsible = items(child(c,"Responsible")).value();
  }
  for(XMLCollection c(child(fde,"TaskList"), false, "TaskList"); c; ++c) {
    _TL& tl = inv.tasklists[c.attr(gc->Attr_name)];
    for(XMLCollection t(child(c,"Task"), false); t; ++t)
      tl.push_back(t.attr(gc->Attr_name));
  }
  for(XMLCollection c(child(fde,"ConnectionList"), false, "ConnectionList"); c; ++c) {
    _CL& cl = inv.connlists[c.attr(gc->Attr_name)];
    for(XMLCollection t(child(c,"Connection"), false); t; ++t)
      cl.push_back(t.attr(gc->Attr_name));
  }
  // Build whole Node type
  for(XMLCollection c(child(fde,"NodeType"),false,"NodeType"); c; ++c) {
    _NT& nt = inv.nodetypes[c.attr(gc->Attr_name)];
    nt.name = c.attr(gc->Attr_name);
    for(XMLCollection t(child(c,"TaskList"),false,"TaskList"); t; ++t) {
      const _TL& tl = inv.tasklists[t.attr(gc->Attr_name)];
      nt.tasks.insert(nt.tasks.end(),tl.begin(),tl.end());
    }
    for(XMLCollection t(child(c,"ConnectionList"), false,"ConnectionList"); t; ++t) {
      const _CL& cl = inv.connlists[t.attr(gc->Attr_name)];
      nt.connections.insert(nt.connections.end(),cl.begin(),cl.end());
    }
    for(XMLCollection t(child(c,"Task"), false,"Task"); t; ++t)
      nt.tasks.push_back(t.attr(gc->Attr_name));
    for(XMLCollection t(child(c,"Connection"), false,"Connection"); t; ++t)
      nt.connections.push_back(t.attr(gc->Attr_name));
    for(XMLCollection t(child(c,"Project"), false,"Project"); t; ++t)
      nt.projects.push_back(t.attr(gc->Attr_name));
  }
  for(XMLCollection c(child(fde,"NodeList"), false,"NodeList"); c; ++c) {
    string nam = c.attr(gc->Attr_name);
    if ( nam.empty() ) {
      XMLElement item;
      for(XMLCollection n(child(c,"Name"), false); n; ++n)
        nam = n.value();
      cout << "---->:" << nam << endl;
    }
    _NC& nc = inv.nodecollections[nam];
    nc.name = nam;
    for(XMLCollection n(child(c,"Node"), false, "Node"); n; ++n)
      nc.nodes[n.attr(gc->Attr_name)] = n.attr(gc->Attr_type);
  }
}

#include "RTL/rtl.h"
#include <cstdarg>

static void help_cluster() {
  ::printf("You have to supply a file name\n");
  ::exit(0);
}

static size_t prt(void*,int,const char* fmt,va_list args) {
  size_t result;
  string format = fmt;
  format += "\n";
  result = ::vfprintf(stdout, format.c_str(), args);
  ::fflush(stdout);
  return result;
}


static string getInput(int argc, char** argv) {
  RTL::CLI cli(argc,argv,help_cluster);
  string fname;
  cli.getopt("input",1,fname);
  if ( fname.empty() ) help_cluster();
  ::lib_rtl_install_printer(prt,0);
  return fname;
}

extern "C" int tsksup_inventory(int argc, char** argv) {
  string fname=getInput(argc,argv);
  TaskSupervisorParser ts;
  ts.parseFile(fname);
  ROMon::Inventory inv;
  ts.getInventory(inv);
  cout << inv << endl;
  return 1;
}

extern "C" int tsksup_cluster(int argc, char** argv) {
  string fname=getInput(argc,argv);
  TaskSupervisorParser ts;
  ts.parseFile(fname);
  Cluster cluster;
  ts.getClusterNodes(cluster);
  cout << cluster << endl;
  return 1;
}
extern "C" int tsksup_network(int argc, char** argv) {
  string fname=getInput(argc,argv);
  TaskSupervisorParser ts;
  TaskSupervisorParser::Clusters clusters;
  ts.parseFile(fname);
  ts.getClusters(clusters);
  cout << clusters << endl;
  return 1;
}
