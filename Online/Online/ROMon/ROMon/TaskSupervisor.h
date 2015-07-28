#ifndef TASKSUPERVISOR_H
#define TASKSUPERVISOR_H

#include "CPP/Interactor.h"
#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <map>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  // Forward declarations
  class SubfarmSummary;

  /* @class Cluster TaskSupervisor.h ROMon/TaskSupervisor.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  struct Cluster {


    /* @class Cluster::Project TaskSupervisor.h ROMon/TaskSupervisor.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct PVSSProject {
      std::string name;
      bool   eventMgr;
      bool   dataMgr;
      bool   distMgr;
      bool   fsmSrv;
      bool   devHdlr;
      PVSSProject() : eventMgr(false), dataMgr(false), distMgr(false), fsmSrv(false), devHdlr(false) {}
      PVSSProject(const PVSSProject& c) : name(c.name), eventMgr(c.eventMgr),
                                          dataMgr(c.dataMgr), distMgr(c.distMgr), fsmSrv(c.fsmSrv), devHdlr(c.devHdlr) {}
      PVSSProject& operator=(const PVSSProject& c)   { 
        if ( this != &c )  {
          name=c.name; 
          eventMgr=c.eventMgr; 
          dataMgr=c.dataMgr;
          distMgr=c.distMgr;
          fsmSrv=c.fsmSrv;
          devHdlr=c.devHdlr;
        }
        return *this;
      }	
      bool operator==(const PVSSProject& c) const
      { return name==c.name && eventMgr==c.eventMgr && dataMgr==c.dataMgr && distMgr==c.distMgr && fsmSrv==c.fsmSrv && devHdlr==c.devHdlr; }
      bool operator<(const PVSSProject& c) const
      { return name<c.name && eventMgr<c.eventMgr && dataMgr<c.dataMgr && distMgr<c.distMgr && fsmSrv<c.fsmSrv && devHdlr<c.devHdlr;; }
      bool ok() const {  return eventMgr && dataMgr && distMgr; }
      static const char* state(bool value) { return value ? "RUNNING" : "DEAD"; }
      static std::string projectName(const std::string& n);
    };
    typedef std::vector<PVSSProject> Projects;

    /* @class Cluster::Node TaskSupervisor.h ROMon/TaskSupervisor.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Node {
      typedef std::pair<std::string,bool>    Item;
      typedef std::vector<Item>              Tasks;
      typedef std::vector<Item>              Connections;
      typedef std::vector<PVSSProject>       Projects;
      std::string name, status, time, boot;

      Tasks        tasks;
      Connections  conns;
      Projects     projects;
      float perc_cpu, perc_mem;
      int          vsize, rss, data, stack;
      size_t       taskCount, missTaskCount, totalTaskCount;
      size_t       connCount, missConnCount, totalConnCount;
      long         blk_size, blk_total, blk_availible;

      void _cpy(const Node& n) {
        name = n.name;
        time = n.time;
        status = n.status;
        tasks = n.tasks;
        projects = n.projects;
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
        blk_size = n.blk_size;
        blk_total = n.blk_total;
        blk_availible = n.blk_availible;
      }
      Node() : perc_cpu(0.), perc_mem(0.), vsize(0), rss(0), data(0), stack(0),
               taskCount(0), missTaskCount(0), totalTaskCount(0),
               connCount(0), missConnCount(0), totalConnCount(0),
               blk_size(0), blk_total(0), blk_availible(0)
      {}
      Node(const std::string& n,const std::string&s) : name(n), status(s) {}
      Node(const Node& n)  { _cpy(n); }
      Node& operator = (const Node& n)  { if (&n!=this) _cpy(n); return *this; }
      bool operator == (const Node& n) const {  return n.name == this->name;   }
      bool operator <  (const Node& n) const {  return n.name <  this->name;   }
    };
    typedef std::map<std::string,Node> Nodes;
    typedef Node::Tasks                Tasks;
    typedef Node::Connections          Connections;

    std::string name, status, time;
    Nodes       nodes;
    Cluster() {}
    Cluster(const std::string& n,const std::string&s): name(n), status(s) {}
    Cluster(const Cluster& n) : name(n.name), status(n.status), nodes(n.nodes) {}
    Cluster& operator=(const Cluster& c)  {
      if ( this != &c )  {
        name   = c.name;
        status = c.status;
        nodes  = c.nodes;
      }
      return *this;
    }
  };

  /* @class Inventory TaskSupervisor.h ROMon/TaskSupervisor.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class Inventory {
  public:
    /* @class Inventory::Task TaskSupervisor.h ROMon/TaskSupervisor.h
     *
     * @author  M.Frank
     * @version 1.0
     */
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
      Task(const Task& c) {        (*this) = c;      }
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
    typedef std::string Project;
    typedef std::map<std::string,Task>           TaskMap;
    typedef std::vector<std::string>             TaskList;
    typedef std::map<std::string,TaskList>       TaskListMap;
    typedef std::vector<std::string>             ProjectList;
    typedef std::vector<std::string>             ConnectionList;
    typedef std::map<std::string,ConnectionList> ConnectionListMap;

    /* @class Inventory::NodeType TaskSupervisor.h ROMon/TaskSupervisor.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    class NodeType {
    public:
      std::string    name;
      TaskList       tasks;
      ProjectList    projects;
      ConnectionList connections;
      void _cpy(const NodeType& c) {
        tasks = c.tasks;
        projects = c.projects;
        connections = c.connections;
      }

      NodeType()                             {                        }
      NodeType(const NodeType& c)            { _cpy(c);               }
      NodeType& operator=(const NodeType& c) { _cpy(c); return *this; }
    };

    /* @class Inventory::NodeCollection TaskSupervisor.h ROMon/TaskSupervisor.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    class NodeCollection {
    public:
      typedef std::map<std::string,std::string> NodeList;
      std::string name;
      NodeList nodes;
      NodeCollection() {}
      NodeCollection(const NodeCollection& c):nodes(c.nodes) {                              }
      NodeCollection& operator=(const NodeCollection& c)     { nodes=c.nodes; return *this; }
      bool operator==(const NodeCollection& c) const         { return nodes==c.nodes;       }
      bool operator<(const NodeCollection& c) const          { return nodes<c.nodes;        }
    };

    typedef std::map<std::string,NodeType>       NodeTypeMap;
    typedef std::map<std::string,NodeCollection> NodeCollectionMap;

    TaskMap           tasks;
    TaskListMap       tasklists;
    ConnectionListMap connlists;
    NodeTypeMap       nodetypes;
    NodeCollectionMap nodecollections;

    std::ostream& print(std::ostream& os)   const;

  };

  class InventoryClient {
  protected:
    /// CLient name
    std::string m_name;
    /// Reference to inventory datapoint
    Inventory*  m_inventory;
  public:
    /// Initializing constructor
    InventoryClient(const std::string& nam, Inventory* inv) : m_name(nam), m_inventory(inv) {}
    /// Default destructor
    virtual ~InventoryClient() {}
    /// Access to inventory information
    Inventory* inventory() const {   return m_inventory; }
    /// Client name
    const std::string& name() const {  return m_name; }
  };

  class NodeTaskMon : public InventoryClient, public Interactor  {
  public:
    enum State { DEAD, ALIVE };
  protected:
    typedef std::map<std::string,bool> Items;
    typedef std::map<std::string,int>  ItemsI;
    typedef Inventory::TaskList        TaskList;
    typedef Inventory::ProjectList     ProjectList;
    typedef Inventory::ConnectionList  ConnectionList;
    /// Interactor parent object
    Interactor*         m_parent;
    /// CLient type
    std::string         m_type;
    /// Error information
    std::string         m_error;
    /// Node type information
    Inventory::NodeType m_nodeType;
    /// List of tasks present
    Items               m_tasks;
    /// List of projects present
    Items               m_projects;
    /// List of connections present
    ItemsI              m_connections;
    /// DIM service ID with subscribed information
    int                 m_infoId;
    /// DIM service ID with subscribed information
    int                 m_pingId;
    /// Flag to indicate information update
    bool                m_changed;
    /// Time stamp of last update
    time_t              m_taskUpdate, m_connUpdate;
    /// String containing XML information of tasks
    std::string         m_taskStatus;
    /// String containing XML information of connections
    std::string         m_connStatus;
    /// Node status
    State               m_state;
    /// Number of bad connections
    int                 m_numBadTasks;
    /// Number of bad tasks
    int                 m_numBadConnections;
    /// Total number of MB of the local disk capacity
    float               m_diskSize;
    /// Total number of MB left of the local disk capacity
    float               m_diskAvailible;


  public:
    /// Initializing constructor
    NodeTaskMon(Interactor* parent, const std::string& nam, const std::string& typ, Inventory* inv);
    /// Default destructor
    virtual ~NodeTaskMon();
    /// DimInfo overload to process messages
    static void infoHandler(void* tag, void* address, int* size);
    /// DimInfo overload to process messages
    static void pingHandler(void* tag, void* address, int* size);
    /// Access node type
    const std::string& type() const            {  return m_type;              }
    /// Access error information
    const std::string& error() const           {  return m_error;             }
    /// Set error information
    void setError(const std::string& e)        {  m_error = e;                }
    /// Parent interactor object
    Interactor* parent() const                 {  return m_parent;            }
    /// Last task update
    const time_t& taskUpdate() const           {  return m_taskUpdate;        }
    /// Encode connection status information in XML
    const std::string& connectionStatus() const{  return m_connStatus;        }
    /// Encode task information in XML
    const std::string& taskStatus() const      {  return m_taskStatus;        }
    /// Access the number of bad tasks
    int numBadTasks() const                    {  return m_numBadTasks;       }
    /// Access the number of bad connections
    int numBadConnections() const              {  return m_numBadConnections; }
    /// Total number of MB of the local disk capacity
    float diskSize()  const                    {  return m_diskSize;          }
    /// Total number of MB left of the local disk capacity
    float diskAvailible()  const               {  return m_diskAvailible;     }

    /// Access to node state
    State state() const                        {  return m_state;             }
    /// Start monitoring activity of this object
    virtual int start();
    /// Start monitoring activity of this object
    virtual int stop();
    /// Publish monitoring information
    int publish();
    /// Encode connection status information in XML
    const std::string& updateConnections();
    /// Interactor overload: Display callback handler
    virtual void handle(const Event& ev);
    /// Update task information
    void updateTaskInfo(const char* ptr, size_t len);
    /// Update connection information
    void updatePingInfo(const char* ptr, size_t len);
  };

  class SubfarmTaskMon : public InventoryClient, public Interactor  {
  protected:
    typedef std::map<std::string,NodeTaskMon*> Monitors;
    typedef Inventory::NodeCollection::NodeList NodeList;
    /// List of individual node monitors known to this sub-farm
    Monitors        m_nodes;
    /// Node list of this subfarm
    NodeList        m_nodeList;
    /// DIM Service identifier
    int             m_serviceID;
    /// DIM service identifier for summary data
    int             m_summaryID;
    /// Buffer for publishing data
    std::string     m_data;
    /// Buffer with subfarm summary information
    SubfarmSummary* m_summary;

  public:
    /// Initializing constructor
    SubfarmTaskMon(const std::string& nam, Inventory* inv);
    /// Default destructor
    virtual ~SubfarmTaskMon() {}
    /// Start the monitoring object
    int start();
    /// Stop the monitoring object
    int stop();
    /// Publish monitoring information
    int publish();
    /// Interactor overload: Display callback handler
    virtual void handle(const Event& ev);
    /// DIM callback on dis_update_service
    static void feedData(void* tag, void** buf, int* size, int* first);
    /// DIM callback on dis_update_service
    static void feedSummary(void* tag, void** buf, int* size, int* first);
  };

}
namespace ROMon  {
  std::ostream& operator<<(std::ostream& os, const Cluster& c);
  std::ostream& operator<<(std::ostream& os, const Cluster::Node& n);
  std::ostream& operator<<(std::ostream& os, const Cluster::Node::Tasks& t);
  std::ostream& operator<<(std::ostream& os, const std::list<Cluster>& c);
  std::ostream& operator<<(std::ostream& os, const Inventory::NodeCollection& nc);
  std::ostream& operator<<(std::ostream& os, const Inventory::NodeCollectionMap& ncm);
  std::ostream& operator<<(std::ostream& os, const Inventory::NodeType& nt);
  std::ostream& operator<<(std::ostream& os, const Inventory::NodeTypeMap& ntm);
  std::ostream& operator<<(std::ostream& os, const Inventory& inv);
}
#endif // TASKSUPERVISOR_H
