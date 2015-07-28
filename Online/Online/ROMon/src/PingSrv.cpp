#ifndef ROMON_PINGER_H
#define ROMON_PINGER_H

// Framework include files
#include "CPP/Interactor.h"
#include "ROMon/CPUMon.h"
#include "RTL/rtl.h"

// C++ include files
#include <vector>
#include <string>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /** @class NodePinger
   *
   */
  class NodePinger : public Interactor  {
  protected:
    typedef Connectionset::Connections Connections;

    /// Buffer pointer
    char*            m_ptr;
    /// List of individual node monitors known to this sub-farm
    Connectionset*   m_connections;
    /// Own name
    std::string      m_name;
    /// DIM Service identifier
    int              m_serviceID;
    /// Worker thread identifier
    lib_rtl_thread_t m_thread;
    /// Flag to steer thread execution
    bool             m_run;

  public:
    /// Initializing constructor
    NodePinger(const std::string& nam, const std::vector<std::string>& conns);
    /// Default destructor
    virtual ~NodePinger();
    /// Access to own name
    const std::string& name() const { return m_name; }
    /// Access to connections container
    Connections& connections() { return m_connections->connections; }
    /// Start the monitoring object
    int start();
    /// Stop the monitoring object
    int stop();
    /// Interactor overload: Display callback handler
    virtual void handle(const Event& ev);
    /// DIM callback on dis_update_service
    static void feedData(void* tag, void** buf, int* size, int* first);
    /// Thread handler
    static int runThread(void* arg);
  };
}
#endif // ROMON_PINGER_H

#include "ROMonDefs.h"
#include "CPP/TimeSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"

#include <stdexcept>
#include <cstring>
extern "C" {
#include "dis.h"
#include "dic.h"
}

using namespace std;
using namespace ROMon;

#define CONNECTION_UPDATE_TIMEDIFF 30
#define TASK_UPDATE_TIMEDIFF       60
#define PUBLISH_TIMEDIFF           10
#define TIME_FORMAT                "%Y-%m-%d %H:%M:%S"
#define CMD_DATA                   12345
#define PING_SUCCESS_STATUS         5


/// Initializing constructor
NodePinger::NodePinger(const string& nam, const std::vector<std::string>& conns) 
  : m_name(nam), m_serviceID(0), m_thread(0)
{
  Connection c;
  m_ptr = new char[sizeof(Connectionset)+sizeof(Connection)*(conns.size()+1)];
  m_connections = new(m_ptr) Connectionset();
  Connectionset::Connections& cs = m_connections->reset()->connections;
  Connectionset::Connections::iterator ci = cs.reset();
  m_connections->type = Connectionset::TYPE;
  ro_get_node_name(m_connections->name,sizeof(m_connections->name));
  ro_gettime(&m_connections->time,(unsigned int*)&m_connections->millitm);
  for(std::vector<std::string>::const_iterator i=conns.begin(); i!=conns.end();++i) {
    ::strncpy((*ci).node,(*i).c_str(),sizeof((*ci).node));
    (*ci).node[sizeof(c.node)-1]=0;
    (*ci).last = 0;
    (*ci).status = -1;
    ci = cs.add(ci);
  }
}

/// Default destructor
NodePinger::~NodePinger() { 
  if ( m_connections ) m_connections->~Connectionset();
  if ( m_ptr ) delete [] m_ptr; 
}

/// Start the monitoring object
int NodePinger::start() {
  string svc = "/"+strlower(name())+"/PingSrv";
  if ( 0 != m_serviceID ) ::dis_remove_service(m_serviceID);
  m_serviceID = ::dis_add_service((char*)svc.c_str(),(char*)"C",0,0,feedData,(long)this);
  ::lib_rtl_output(LIB_RTL_INFO,"Ping Service %s on %s started.",svc.c_str(),name().c_str());
  if ( 0 != m_thread )  {
    ::lib_rtl_delete_thread(m_thread);
    m_thread = 0;
  }
  m_run = true;
  if ( !lib_rtl_is_success(lib_rtl_start_thread(runThread,this,&m_thread)) ) {
    ::lib_rtl_output(LIB_RTL_FATAL,"Failed to create worker thread: %s",
                     ::lib_rtl_error_message(::lib_rtl_get_error()));
    return 0;
  }
  return 1;
}

/// Stop the monitoring object
int NodePinger::stop() {
  if ( 0 != m_thread ) {
    m_run = false;
    ::lib_rtl_join_thread(m_thread);
  }
  m_thread = 0;
  if ( 0 != m_serviceID ) {
    ::dis_remove_service(m_serviceID);
  }
  m_serviceID = 0;
  return 1;
}

/// DIM callback on dis_update_service
void NodePinger::feedData(void* tag, void** buf, int* size, int* /* first */) {
  NodePinger* h = *(NodePinger**)tag;
  *size = h->m_connections->length();
  *buf = h->m_ptr;
}

/// Thread handler
int NodePinger::runThread(void* arg) {
  string cmd;
  char text[2048];
  NodePinger* p = (NodePinger*)arg;
  ::lib_rtl_output(LIB_RTL_INFO,"Starting PING thread on node:%s.",p->name().c_str());
  while(p->m_run) {
    for(Connections::iterator i=p->connections().begin(); i!=p->connections().end();++i) {
      string node = (*i).node;
      cmd = "/bin/ping -c 1 -w 1 ";
      if ( node.find("-d1") != string::npos ) cmd += "-s 8000 ";
      cmd += node;
      //::lib_rtl_output(LIB_RTL_DEBUG,"Executing command:%s",cmd.c_str());
      FILE* f = ::lib_rtl_pipe_open(cmd.c_str(),"r");
      if ( f ) {
        int nb = ::fread(text,1,sizeof(text)-1,f);
        if ( nb > 0 ) {
          text[nb] = 0;
          (*i).last = (int)::time(0);
          if ( ::strstr(text," 100% packet loss") == 0 )
            (*i).status = PING_SUCCESS_STATUS;
          else if ( (*i).status >= 0 )
            --(*i).status;
        }
        else {
          ::lib_rtl_output(LIB_RTL_ERROR,"Read-error: %d bytes read:%s.",nb,::lib_rtl_error_message(::lib_rtl_get_error()));
        }
        ::lib_rtl_pipe_close(f);
      }
      else {
        ::lib_rtl_output(LIB_RTL_ERROR,"Error:%s.",::lib_rtl_error_message(::lib_rtl_get_error()));
      }
    }
    ro_gettime(&p->m_connections->time,(unsigned int*)&p->m_connections->millitm);
    ::lib_rtl_sleep(CONNECTION_UPDATE_TIMEDIFF*1000);
  }
  ::lib_rtl_output(LIB_RTL_INFO,"Exit PING thread on node:%s.",p->name().c_str());
  return 1;
}

/// Handle interaction event
void NodePinger::handle(const Event& ev) {
  try {
    switch(ev.eventtype) {
    case TimeEvent:
      if (ev.timer_data == (void*)CMD_DATA ) {
        if ( m_serviceID ) {
          ::dis_update_service(m_serviceID);
        }
        TimeSensor::instance().add(this,PUBLISH_TIMEDIFF,(void*)CMD_DATA);
      }
      break;
    default:
      break;
    }
  }
  catch(const exception& e) {
    ::lib_rtl_output(LIB_RTL_ERROR,"Exception in callback processing:%s",e.what());
  }
  catch(...) {
    ::lib_rtl_output(LIB_RTL_ERROR,"UNKNOWN exception in callback processing.");
  }
}

static void help_Pinger() {
  ::printf("You have to supply a file name\n");
  ::exit(0);
}

#include "ROMon/TaskSupervisorParser.h"
extern "C" int run_node_ping(int argc, char** argv) {
  string root = ::getenv("ROMONDATA") ? ::getenv("ROMONDATA") : "file://";
  string dns  = ::getenv("DIM_DNS_NODE") ? ::getenv("DIM_DNS_NODE") : "";
  string node = RTL::nodeNameShort();
  string server = "/"+node+"/"+RTL::processName();
  string fname = "file://" + root + "/" + strupper(dns) + ".xml";
  string inventory = "file://" + root + "/" + "TaskInventory.xml";
  vector<string> connections;  
  if ( 0 != ::getenv("DIM_DNS_NODE") )  {
    ROMon::Inventory inv;
    RTL::CLI cli(argc,argv,help_Pinger);
    cli.getopt("inventory",3,inventory);
    cli.getopt("input",3,fname);
    if ( fname.empty() ) help_Pinger();
    ::lib_rtl_install_printer(ro_rtl_print,(void*)LIB_RTL_INFO);
    dns = strupper(dns);
    node = strlower(node);
    XML::TaskSupervisorParser ts;
    ts.parseFile(inventory);
    ts.getInventory(inv);
    ts.parseFile(fname);
    ts.getInventory(inv);
    Inventory::NodeCollectionMap::const_iterator ni=inv.nodecollections.find(dns);
    if ( ni != inv.nodecollections.end() ) {
      const Inventory::NodeCollection::NodeList& nl = (*ni).second.nodes;
      Inventory::NodeCollection::NodeList::const_iterator niter = nl.find(node);
      if ( niter != nl.end() ) {
        const string& typ = (*niter).second;
        const Inventory::NodeTypeMap::iterator j = inv.nodetypes.find(typ);
        if ( j != inv.nodetypes.end() ) {
          size_t idx;
          const Inventory::NodeType& nt = (*j).second;
          for(Inventory::ConnectionList::const_iterator c=nt.connections.begin(); c!=nt.connections.end();++c) {
            string n = *c;
            if ( (idx=n.find("<DIM_DNS_NODE>")) != string::npos )
              n.replace(idx,idx+14,::getenv("DIM_DNS_NODE"));
            else if ( (idx=n.find("<NODE>")) != string::npos )
              n.replace(idx,idx+6,::RTL::nodeNameShort());
            else if ( (idx=n.find("<DATAINTERFACE>")) != string::npos )
              n.replace(idx,idx+15,RTL::nodeNameShort()+"-d1");
            connections.push_back(n);
          }
          goto Start;
        }
      }
    }
    // Error: stop process!
    ::lib_rtl_output(LIB_RTL_ERROR,"Failed to start PING ROMon client on Node:%s.",node.c_str());
    return 0;
  }
  else {
    ::lib_rtl_output(LIB_RTL_ERROR,"DIM_DNS_NODE undefined: Failed to start PING ROMon client on Node:%s.",node.c_str());
    return 0;
  }
 Start:
  ::lib_rtl_output(LIB_RTL_DEBUG,"Input file is:%s Node:%s Task:%s.",fname.c_str(),node.c_str(),server.c_str());
  NodePinger mon(node,connections);
  mon.start();
  TimeSensor::instance().add(&mon,1,(void*)CMD_DATA);
  ::dis_start_serving((char*)server.c_str());
  TimeSensor::instance().run();
  return 1;
}
