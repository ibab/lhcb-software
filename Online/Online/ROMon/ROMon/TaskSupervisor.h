#ifndef TASKSUPERVISOR_H
#define TASKSUPERVISOR_H

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
struct Cluster {
  struct Node {
    typedef std::vector<std::string> Tasks;
    std::string name, status, time;
    Tasks  missing, found;
    size_t totalCount, missCount, taskCount;
    void _cpy(const Node& n) {
      name = n.name;
      time = n.time;
      status = n.status;
      missing = n.missing;
      found = n.found;
      totalCount = n.totalCount;
      missCount = n.missCount;
      taskCount = n.taskCount;
    }
    Node() : totalCount(0), missCount(0), taskCount(0) {}
    Node(const std::string& n,const std::string&s) : name(n), status(s) {}
    Node(const Node& n)  { _cpy(n); }
    Node& operator = (const Node& n)  { if (&n!=this) _cpy(n); return *this; }
    bool operator == (const Node& n) const {  return n.name == this->name;   }
    bool operator <  (const Node& n) const {  return n.name <  this->name;   }
  };
  typedef std::map<std::string,Node> Nodes;
  typedef Node::Tasks Tasks;

  std::string name, status, time;
  Nodes       nodes;
  Cluster() {}
  Cluster(const std::string& n,const std::string&s): name(n), status(s) {}
  Cluster(const Cluster& n) : name(n.name), status(n.status), nodes(n.nodes) {}
};

#endif // TASKSUPERVISOR_H
