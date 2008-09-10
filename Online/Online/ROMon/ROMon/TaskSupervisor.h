#ifndef TASKSUPERVISOR_H
#define TASKSUPERVISOR_H

#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <map>

/*
  <Cluster name="HLTD01" status="ALIVE">
  <Node name="hltd0104" status="ALIVE"><Found_Tasks count="16"/></Node>
  <Node name="hltd01" status="ALIVE">
  <Missing_Tasks  count="1">
  <Task name="LogGaudiSumSrvaaaa"/>
  </Missing_Tasks>
  <Found_Tasks count="22"/>
  </Node>
  <Node name="hltd0101" status="ALIVE"><Found_Tasks count="16"/></Node>
  <Node name="hltd0103" status="ALIVE"><Found_Tasks count="16"/></Node>
  <Node name="hltd0102" status="ALIVE"><Found_Tasks count="16"/></Node>
  </Cluster>
*/
namespace ROMon {
  struct Cluster {
    struct Node {
      typedef std::pair<std::string,bool> Item;
      typedef std::vector<Item> Tasks;
      typedef std::vector<Item> Connections;
      std::string name, status, time, boot;
    
      Tasks        tasks;
      Connections  conns;
      float perc_cpu, perc_mem;
      int          vsize, rss, data, stack;
      size_t       taskCount, missTaskCount, totalTaskCount;
      size_t       connCount, missConnCount, totalConnCount;

      void _cpy(const Node& n) {
	name = n.name;
	time = n.time;
	status = n.status;
	tasks = n.tasks;
	boot = n.boot;
	totalTaskCount = n.totalTaskCount;
	missTaskCount = n.missTaskCount;
	taskCount = n.taskCount;
	vsize = n.vsize;
	rss = n.rss;
	data = n.data;
	stack = n.stack;

	conns = n.conns;
	totalConnCount = n.totalConnCount;
	missConnCount = n.missConnCount;
	connCount = n.connCount;
      }
      Node() : perc_cpu(0.), perc_mem(0.), vsize(0), rss(0), data(0), stack(0),
	   taskCount(0), missTaskCount(0), totalTaskCount(0), 
	   connCount(0), missConnCount(0), totalConnCount(0)
      {}
      Node(const std::string& n,const std::string&s) : name(n), status(s) {}
      Node(const Node& n)  { _cpy(n); }
      Node& operator = (const Node& n)  { if (&n!=this) _cpy(n); return *this; }
      bool operator == (const Node& n) const {  return n.name == this->name;   }
      bool operator <  (const Node& n) const {  return n.name <  this->name;   }
    };
    typedef std::map<std::string,Node> Nodes;
    typedef Node::Tasks       Tasks;
    typedef Node::Connections Connections;

    std::string name, status, time;
    Nodes       nodes;
    Cluster() {}
    Cluster(const std::string& n,const std::string&s): name(n), status(s) {}
    Cluster(const Cluster& n) : name(n.name), status(n.status), nodes(n.nodes) {}

  };
  class Inventory {
  public:
    class Task {
    public:
      std::string name;
      std::string type;
      std::string path;
      std::string dimDns;
      std::string dimSvc;
      std::string user;
      std::string responsible;
      Task() {}
      Task(const Task& c) {	(*this) = c;      }
      Task(const std::string& n,const std::string& t) : name(n), type(t) {}
      Task& operator=(const Task& c)       {
	name=c.name;
	type=c.type;
	path=c.path;
	dimDns=c.dimDns;
	dimSvc=c.dimSvc;
	user=c.user;
	responsible=c.responsible;
	return *this;
      }
    };
    typedef std::string Connection;
    typedef std::map<std::string,Task>           TaskMap;
    typedef std::vector<std::string>             TaskList;
    typedef std::map<std::string,TaskList>       TaskListMap;
    typedef std::vector<std::string>             ConnectionList;
    typedef std::map<std::string,ConnectionList> ConnectionListMap;

    TaskMap           tasks;
    TaskListMap       tasklists;
    ConnectionListMap connlists;

    class NodeType {
    public:
      TaskList       tasks;
      ConnectionList connections;

      NodeType() {}
      NodeType(const NodeType& nt) { _cpy(nt); }
      NodeType& operator=(const NodeType& nt) { _cpy(nt); return *this; }
      void _cpy(const NodeType& nt) {
	tasks = nt.tasks;
	connections = nt.connections;
      }
    };
    typedef std::map<std::string,NodeType>  NodeTypeMap;
    NodeTypeMap nodetypes;
  };
}
std::ostream& operator<<(std::ostream& os, const ROMon::Cluster& c);
std::ostream& operator<<(std::ostream& os, const ROMon::Cluster::Node& n);
std::ostream& operator<<(std::ostream& os, const ROMon::Cluster::Node::Tasks& t);
std::ostream& operator<<(std::ostream& os, const std::list<ROMon::Cluster>& c);
std::ostream& operator<<(std::ostream& os, const ROMon::Inventory& inv);

#endif // TASKSUPERVISOR_H
