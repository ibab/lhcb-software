#include "TaskSupervisorParser.h"
#include <iostream>
#include <iomanip>

using namespace xercesc;
using namespace std;
using namespace XML;

static const XMLTag MISS_TASKS     ("Missing_Tasks");
static const XMLTag FOUND_TASKS    ("Found_Tasks");
static const XMLTag TASKNODE       ( "Task");
static const XMLTag Attr_name      ( "name");
static const XMLTag Attr_status    ( "status");
static const XMLTag Attr_count     ( "count");
static const XMLTag Attr_time      ( "time");

std::ostream& operator<<(std::ostream& os, const Cluster& c);
std::ostream& operator<<(std::ostream& os, const Cluster::Node& n);
std::ostream& operator<<(std::ostream& os, const Cluster::Node::Tasks& t);
ostream& operator<<(ostream& os, const TaskSupervisorParser::Clusters& c);

ostream& operator<<(ostream& os, const Cluster::Node::Tasks& t) {
  for(Cluster::Node::Tasks::const_iterator i=t.begin(); i!=t.end();++i)
    os << setw(12) << left << (*i) << " ";
  return os;
}

ostream& operator<<(ostream& os, const Cluster::Node& n) {
  os << "    Node:" << setw(12) << left << n.name << "   [" << n.status << "]" << endl
     << "       Missing:" << n.missing << std::endl;
  return os;
}

ostream& operator<<(ostream& os, const Cluster& c) {
  os << "  Cluster:" << setw(12) << left << c.name << "  [" << c.status << "]" << endl;
  for(Cluster::Nodes::const_iterator i=c.nodes.begin(); i!=c.nodes.end();++i)
    os << (*i).second;
  return os;
}

ostream& operator<<(ostream& os, const TaskSupervisorParser::Clusters& c) {
  for(TaskSupervisorParser::Clusters::const_iterator i=c.begin(); i!=c.end();++i)
    os << "-------------------------------------------------------------" << endl << *i;
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
  XMLElement elt(fde);
  cluster.name   = elt.attr(Attr_name);
  cluster.time   = elt.attr(Attr_time);
  cluster.status = elt.attr(Attr_status);
  cluster.nodes.clear();
  for(XMLCollection c(child(fde,"Node"), false); c; ++c) {
    XMLElement e(child(c,FOUND_TASKS));
    std::string nam = c.attr(Attr_name);
    cluster.nodes.insert(std::make_pair(nam,Node(nam,c.attr(Attr_status))));
    Node& node = cluster.nodes[nam];
    Tasks& found   = node.found;
    Tasks& missing = node.missing;
    node.time      = c.attr(Attr_time);
    found.clear();
    missing.clear();
    node.totalCount = 0;
    if ( e ) {
      node.totalCount = ::atoi(e.attr(Attr_count).c_str());
      for(XMLCollection t(child(child(c,MISS_TASKS),TASKNODE), false); t; ++t)
	missing.push_back(t.attr(Attr_name));
      for(XMLCollection t(child(e,TASKNODE), false); t; ++t)
	found.push_back(t.attr(Attr_name));
    }
    node.missCount = missing.size();
    node.taskCount = node.totalCount - node.missCount;
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


extern "C" int tsksup_cluster(int argc, char** argv) {
  RTL::CLI cli(argc,argv,help_cluster);
  std::string fname;
  cli.getopt("input",1,fname);
  if ( fname.empty() ) help_cluster();
  ::lib_rtl_install_printer(prt,0);
  TaskSupervisorParser ts;
  ts.parseFile(fname);
  Cluster cluster;
  ts.getClusterNodes(cluster);
  std::cout << cluster << std::endl;
  return 1;
}
extern "C" int tsksup_network(int argc, char** argv) {
  RTL::CLI cli(argc,argv,help_cluster);
  std::string fname;
  cli.getopt("input",1,fname);
  if ( fname.empty() ) help_cluster();
  ::lib_rtl_install_printer(prt,0);
  TaskSupervisorParser ts;
  TaskSupervisorParser::Clusters clusters;
  ts.parseFile(fname);
  ts.getClusters(clusters);
  std::cout << clusters << std::endl;
  return 1;
}
