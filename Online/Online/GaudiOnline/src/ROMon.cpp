#define MBM_IMPLEMENTATION
#include <cstdio>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sys/timeb.h>
#include "RTL/rtl.h"
#include "RTL/Lock.h"
#include "WT/wtdef.h"
#include "MBM/bmstruct.h"
//#include "MBM/bm_internals.h"
#include <map>
#include "dis.hxx"
#include "dic.hxx"
#include "ROMon.h"

#include "GaudiOnline/DimTaskFSM.h"

namespace ROMon {

  class ROMonInfo;
  class ROMonServer;
  class RODimFSMListener;
  class ROMonDimServer;
  class ROMonPublish;
  class ROMonAsciiPublish;
  class ROMonDimPublish;
  
  /**@class RODimListener ROMon.h GaudiOnline/ROMon.h
   *
   * Base class to listen to server creation/deletion
   *
   * @author M.Frank
   */
  class RODimListener : public DimInfoHandler {
  public:
    /// Add handler for a given message source
    virtual void addHandler(const std::string& node,const std::string& svc) = 0;
    /// Remove handler for a given message source
    virtual void removeHandler(const std::string& node, const std::string& svc) = 0;
  };


  /**@class ROMonServer ROMon.h GaudiOnline/ROMon.h
   *
   * Readout monitor server
   *
   * @author M.Frank
   */
 class ROMonServer  {
  protected:
    /// Time sleep in milliseconds between updates
    int m_delay;

  public:
    /// Standard constructor
    ROMonServer(int sleep_delay) : m_delay(sleep_delay) {}
    /// Default destructor
    virtual ~ROMonServer() {}
    /// Dump buffer information to memory
    void dumpBufferInfo(const char* bm_name, BMID dsc, MBMBuffer* mbm);
    /// Handle update
    virtual int handle(void* buff, size_t buff_len);
    /// Start monitoring activity
    int monitor(void* buff, size_t buff_len);
  };

  /**@class RODimFSMListener ROMon.h GaudiOnline/ROMon.h
   *
   *   DIM FSM status listener to collect FSM information from tasks
   *
   *   @author M.Frank
   */
  class RODimFSMListener : RODimListener {
  public:
    typedef std::map<std::string,std::pair<DimInfo*,FSMTask> > Clients;

  protected:
    /// Map of known clients
    Clients   m_clients;
    /// Pointer to DNS info handler to accept client notifications
    ROMonInfo*  m_dns;

  public:
    /// Standard constructor
    RODimFSMListener();
    /// Standard destructor
    virtual ~RODimFSMListener();
    /// Access to client information
    const Clients& clients() const {  return m_clients; }
    /// Add handler for a given message source
    virtual void addHandler(const std::string& node,const std::string& svc);
    /// Remove handler for a given message source
    virtual void removeHandler(const std::string& node, const std::string& svc);
    /// DimInfoHandler overload
    virtual void infoHandler();
  };

  /**@class RODimNodeListener ROMon.h GaudiOnline/ROMon.h
   *
   *   DIM FSM status listener to collect the full information from nodes
   *
   *   @author M.Frank
   */
  class RODimNodeListener : RODimListener {
  public:
    struct Descriptor {
      int len;
      int actual;
      char* data;
      Descriptor() : len(0), actual(0), data(0) {}
    };
    typedef std::map<std::string,std::pair<DimInfo*,Descriptor> > Clients;

  protected:
    /// Map of known clients
    Clients   m_clients;
    /// Pointer to DNS info handler to accept client notifications
    ROMonInfo*  m_dns;

  public:
    /// Standard constructor
    RODimNodeListener();
    /// Standard destructor
    virtual ~RODimNodeListener();
    /// Access to client information
    const Clients& clients() const {  return m_clients; }
    /// Add handler for a given message source
    virtual void addHandler(const std::string& node,const std::string& svc);
    /// Remove handler for a given message source
    virtual void removeHandler(const std::string& node, const std::string& svc);
    /// DimInfoHandler overload
    virtual void infoHandler();
  };

  /**@class ROMonInfo ROMon.h GaudiOnline/ROMon.h
   *
   *   DIM info handler to detect new/dead services
   *
   *   @author M.Frank
   */
  class ROMonInfo : public DimInfo  {
  protected:

    /// Reference to parent/owner object
    RODimListener*    m_server;

    /// Extract node/service name from DNS info
    void getServiceNode(char* s, std::string& svc, std::string& node)  const;
    /// Add handler for a given message source
    void addHandler(const std::string& node, const std::string& svc);
    /// Remove handler for a given message source
    void removeHandler(const std::string& node, const std::string& svc);
    /// DimInfo overload to process messages
    void infoHandler();

  public:

    /// Standard constructor
    ROMonInfo(RODimListener* s);
    /// Standard destructor
    virtual ~ROMonInfo()  {}
  };

  class ROMonGblBuffer  {
  protected:
    int               m_delay;
    int               m_print;
    int               m_verbose;
    lib_rtl_gbl_t     m_gbl;
    lib_rtl_lock_t    m_lock;
    std::string       m_section_name;
    size_t            m_section_size;

  public:
    /// Standard constructor
    ROMonGblBuffer(int argc , char** argv, void (*help_fun)());
    /// Default destructor
    virtual ~ROMonGblBuffer();
    /// Help printout in case of -h /? or wrong arguments
    static void help();
  };  

  /**@class ROMonTaskCollector ROMon.h GaudiOnline/ROMon.h
   *
   * Readout monitor DIM server for a single node
   *
   * @author M.Frank
   */
  class ROMonTaskCollector : public ROMonGblBuffer {
  protected:
    ROMonServer       m_mbmInfo;
    RODimFSMListener  m_fsmInfo;

  public:
    /// Standard constructor
    ROMonTaskCollector(int argc, char** argv);
    /// Default destructor
    virtual ~ROMonTaskCollector() {}
    /// Start monitoring activity
    virtual int monitor();
    /// Help printout in case of -h /? or wrong arguments
    static void help();
  };

  /**@class ROMonNodeCollector ROMon.h GaudiOnline/ROMon.h
   *
   * Readout monitor DIM server for a single node
   *
   * @author M.Frank
   */
  class ROMonNodeCollector : public ROMonGblBuffer {
  protected:
    RODimNodeListener m_info;

  public:
    /// Standard constructor
    ROMonNodeCollector(int argc, char** argv);
    /// Default destructor
    virtual ~ROMonNodeCollector() {}
    /// Start monitoring activity
    virtual int monitor();
    /// Help printout in case of -h /? or wrong arguments
    static void help();
  };


  /**@class ROMonPublish ROMon.h GaudiOnline/ROMon.h
   *
   * Readout monitor information publisher
   *
   * @author M.Frank
   */
  class ROMonPublish  {
  protected:
    int           m_delay;
    int           m_type;
    int           m_verbose;
    std::string   m_section_name;
    size_t        m_section_size;

  public:
    enum DataType { NODE_TYPE=1, NODESET_TYPE=2 };

    /// Standard constructor
    ROMonPublish(int argc , char** argv);
    /// Default destructor
    virtual ~ROMonPublish() {}
    /// Help printout in case of -h /? or wrong arguments
    static void help();
    /// Start monitoring activity
    virtual int monitor();
    /// Publish the collected information
    virtual void publish(void* data) = 0;
  };

  /**@class ROMonAsciiPublish ROMon.h GaudiOnline/ROMon.h
   *
   * Readout monitor information publisher to stdout
   *
   * @author M.Frank
   */
  class ROMonAsciiPublish : public ROMonPublish  {
  public:
    /// Standard constructor
    ROMonAsciiPublish(int argc , char** argv) : ROMonPublish(argc,argv) {}
    /// Default destructor
    virtual ~ROMonAsciiPublish() {}
    /// Publish the collected information
    virtual void publish(void* node);
  };

  /**@class ROMonDimPublish ROMon.h GaudiOnline/ROMon.h
   *
   * Readout monitor server
   *
   * @author M.Frank
   */
  class ROMonDimPublish : public ROMonPublish  {
  protected:
    unsigned int m_service;
    std::string  m_svcName;
    std::pair<int,char*> m_data;
    static void feedData(void* tag, void** address, int* size, int* first);
  public:
    /// Standard constructor
    ROMonDimPublish(int argc , char** argv);
    /// Default destructor
    virtual ~ROMonDimPublish();
    /// Publish the collected information
    virtual void publish(void* node);
  };
}

using namespace ROMon;

namespace {

  template<typename T> std::ostream& operator<<(std::ostream& os, const FixItems<T>& items) {
    os << "Start address:" << (void*)&items
       << " Num Elements:" << std::setw(6)  << std::right << items.size()
       << " - "            << std::setw(6)  << std::left << (items.end()-items.begin())
       << " Length:"       << std::setw(4)  << std::right << items.length()
       << " DataLen:"      << std::setw(4)  << std::right << items.data_length()
       << " Item size: "   << std::setw(4)  << std::right << sizeof(T)
       << " Begin: "       << (void*)items.begin()
       << " End: "         << (void*)items.end();
    if ( items.begin() != items.end() ) os << std::endl;
    for (T* p=items.begin(); p!=items.end(); ++p) os << *p;
    return os;
  }

  template<typename T> std::ostream& operator<<(std::ostream& os, const VarItems<T>& items) {
    os << "Start address:" << (void*)&items
       << " Num Elements:" << std::setw(6)  << std::right << items.size()
       << " - "            << std::setw(6)  << std::left << (items.end()-items.begin())
       << " Length:"       << std::setw(4)  << std::right << items.length()
       << " DataLen:"      << std::setw(4)  << std::right << items.data_length()
       << " Item size: "   << std::setw(4)  << std::right << sizeof(T)
       << " Begin: "       << (void*)items.begin()
       << " End: "         << (void*)items.end();
    if ( items.begin() != items.end() ) os << std::endl;
    for (T* p=items.begin(); p!=items.end(); p=items.next(p)) os << *p;
    return os;
  }

  std::ostream& operator<<(std::ostream& os, const CONTROL& c) {
    os << "Events: Prod:" << std::setw(8) << c.tot_produced
       << " Pend:" << std::setw(3) << c.i_events
       << " Max:"  << std::setw(4) << c.p_emax
       << " Space[kB]: Tot:" << std::setw(5) << (c.bm_size*c.bytes_p_Bit)/1024
       << " Free:" << (c.i_space*c.bytes_p_Bit)/1024
       << " Users:Tot:" << std::setw(3) << c.i_users
       << " Max:" << c.p_umax
       << std::endl;
    return os;
  }

  std::ostream& operator<<(std::ostream& os, const MBMClient& c) {
    os << " MBMClient@"  << (void*)&c
       << " Name:"      << std::setw(24) << std::left << c.name 
       << " PID:"       << std::setw(6)  << c.processID
       << " Partition:" << std::setw(6)  << std::hex << c.partitionID << std::dec
       << " State:"     << std::setw(2)  << std::right << (int)c.state
       << " Type:"      << c.type
       << " Events:"    << std::setw(6) << c.events;
    if ( c.type == 'C' ) os << " Reqs:" << c.reqs[0] << c.reqs[1] << c.reqs[2] << c.reqs[3];   
    os << std::endl;
    return os;
  }

  std::ostream& operator<<(std::ostream& os, const MBMBuffer& b) {
    os << "MBM@"     << (void*)&b
       << " Name:"   << std::setw(16) << std::left << b.name
       << " Len:"    << b.length() << std::endl;
    os << "Control:" << b.ctrl     << std::endl;
    os << "Clients:" << b.clients  << std::endl;
    return os;
  }

  std::ostream& operator<<(std::ostream& os, const FSMTask& t) {
    os << "Task@"    << (void*)&t
       << " UTGID:"  << std::setw(32) << std::left << t.name
       << " PID:"    << std::setw(6)  << t.processID
       << " State:"  << t.state << " -> " << t.targetState << " [" << t.metaState << "]"
       << " Stamps:" << t.lastCmd << " " << t.doneCmd
       << std::endl;
    return os;
  }

  std::ostream& operator<<(std::ostream& os, const Node& n) {
    char buff[64];
    ::strftime(buff,sizeof(buff),"%Y-%m-%d %H:%M:%S",::localtime(&n.time));
    Node::Tasks*   t = n.tasks();
    Node::Buffers* b = n.buffers();
    os << "Node@"       << (void*)&n
       << " Name:"      << n.name
       << " Time:"      << buff << "." << n.millitm
       << std::endl;
    os << "Buffers:"    << *b << std::endl;
    os << "Tasks:  "    << *t << std::endl;
    os << "Node@"       << (void*)&n
       << " BuffLen:"   << b->length()
       << " "           << b->data_length()
       << " bmSize:"    << n.bmSize
       << " TaskLen:"   << t->length()
       << " "           << t->data_length()
       << " FSMTasks:"  << t->size()*sizeof(FSMTask)
       << " Clients:"   << t->size()*sizeof(MBMClient)
       << " taskSize:"  << n.taskSize
       << " Total:"     << n.totalSize
       << std::endl;
    return os;
  }

  std::ostream& operator<<(std::ostream& os, const Nodeset& n) {
    os << "Nodeset@"       << (void*)&n
       << " Name:"      << n.name
       << std::endl;
    os << "Nodes:"    << n.nodes << std::endl;
    return os;
  }
}

MBMBuffer* MBMBuffer::reset() {
  ::memset(this,0,sizeof(MBMBuffer));  
  return this;
}

MBMClient* MBMClient::reset() {
  ::memset(this,0,sizeof(MBMClient));  
  return this;
}

FSMTask* FSMTask::reset() {
  ::memset(this,0,sizeof(FSMTask));  
  return this;
}

Node* Node::reset() {
  ::memset(this,0,sizeof(Node)+sizeof(Node::Buffers)+sizeof(Node::Tasks));
  return this;
}

Node::Buffers* Node::buffers()  const {
  return (Buffers*)(name + sizeof(name));
}

Node::Tasks* Node::tasks()  const {
  Buffers* b = buffers();
  return (Tasks*)(((char*)b) + b->length());
}

void Node::fixup() {
  bmSize = buffers()->length();
  taskSize = tasks()->length();
  totalSize = bmSize + taskSize + sizeof(Node);
}

Nodeset* Nodeset::reset() {
  ::memset(this,0,sizeof(Nodeset));
  return this;
}


static FSMTask& fill(FSMTask* t, const MBMClient& c) {
  ::strcpy(t->name,c.name);
  t->processID   = c.processID;
  t->state       = 'I';
  t->targetState = 'R';
  t->metaState   = 'E';
  t->lastCmd  = 0;
  t->doneCmd = 1;
  return *t;
}

static MBMBuffer& fill(MBMBuffer* mbm, int partitionID, const char* name) {
  static int task_no = 0;
  const char types[] = {'C','P','U'};
  ::memset(mbm,0,sizeof(MBMBuffer));
  MBMBuffer::Clients::iterator it = mbm->clients.reset();
  ::strcpy(mbm->name,name);
  for(int i=0; i<10; ++i, it=mbm->clients.add(it)) {
    ::sprintf((*it).name,"Task_%s_%02d",name,++task_no);
    (*it).partitionID = partitionID;
    (*it).processID   = 1024+10*i+task_no;
    (*it).events      = 1000*i;
    (*it).type        = types[i%3];
    (*it).state       = 'a'+i;
  }
  return *mbm;
}

static Node& fill(Node* n, const char* name) {
  char txt[64];
  static int buff_no = 0;
  n->reset();
  ::strcpy(n->name,name);
  Node::Buffers* b = n->buffers();
  Node::Buffers::iterator ib = b->reset();
  for(int i=0; i<10; ++i, ib = b->add(ib) ) {
    ::sprintf(txt,"Events_%02d",++buff_no);
    fill(ib, 0x111*i, txt);
  }
  Node::Tasks* t = n->tasks();
  Node::Tasks::iterator it = t->reset();
  for(ib=b->begin(); ib != b->end(); ib = b->next(ib) ) {
    MBMBuffer::Clients::iterator ic=(*ib).clients.begin();
    for(; ic != (*ib).clients.end(); ++ic, it=t->add(it))
      fill(it,*ic);
  }
  n->fixup();
  return *n;
}

//==============================================================================

/// Start monitoring activity
int ROMonServer::monitor(void* buff, size_t len) {
  Node* n = (Node*)buff;
  while(1)    {
    handle(n, len);
    std::cout << *n << std::endl;
    ::lib_rtl_sleep(2000);
  }
  return 1;
}

/// Handle update
int ROMonServer::handle(void* buff, size_t /* len */)  {
  Node* n = (Node*)buff;
  lib_rtl_gbl_t bm_all = 0;
  timeb tm;
  ::ftime(&tm);
  n->reset();
  n->time = tm.time;
  n->millitm = tm.millitm;
  ::strncpy(n->name,RTL::nodeName().c_str(),sizeof(n->name));
  n->name[sizeof(n->name)-1] = 0;
  Node::Buffers* b = n->buffers();
  Node::Buffers::iterator mbm = b->reset();

  int status = ::mbm_map_global_buffer_info(&bm_all,false);
  if( lib_rtl_is_success(status) )   {   
    try {
      BUFFERS* buffers = (BUFFERS*)bm_all->address;
      for (int i = 0; i < buffers->p_bmax; ++i)  {
	if ( buffers->buffers[i].used == 1 )  {
	  const char* bm_name = buffers->buffers[i].name;
	  BMID dsc = mbm_map_mon_memory(bm_name);
	  if ( dsc ) {
	    try {
	      dumpBufferInfo(bm_name,dsc,mbm);
	    }
	    catch(...)    {
	    }
	    ::mbm_unmap_memory(dsc);
	    mbm = b->add(mbm);
	  }
	}
      }
    }
    catch(...) {
    }
    ::mbm_unmap_global_buffer_info(bm_all,false);
    return WT_SUCCESS;
  }
  //::lib_rtl_printf("Cannot map global MBM buffer information....\n");
  return 0;
}

/// Dump buffer information to memory
void ROMonServer::dumpBufferInfo(const char* bm_name, BMID dsc, MBMBuffer* mbm)   {
  CONTROL  *ctr = mbm_get_control_table(dsc);
  USER     *us  = mbm_get_user_table(dsc), *utst=(USER*)~0x0;
  MBMBuffer::Clients::iterator t = mbm->clients.reset();
  ::memset(mbm,0,sizeof(MBMBuffer));
  ::memcpy(&mbm->ctrl,ctr,sizeof(CONTROL));
  ::strncpy(mbm->name,bm_name,sizeof(mbm->name));
  mbm->name[sizeof(mbm->name)-1] = 0;
  for (int j=0; j < ctr->p_umax; j++,us++)    {
    if ( us == utst || us == 0              ) break;
    if ( us->block_id != int(MBM::BID_USER) ) continue;
    if ( us->busy     == 0                  ) continue;
    t->reqs[0] = t->reqs[1] = t->reqs[2] = t->reqs[3] = ' ';
    for (int k=0; k<us->n_req; ++k )  {
      if      ( us->req[k].user_type == BM_REQ_ONE  ) t->reqs[1] = '1';
      else if ( us->req[k].user_type == BM_REQ_USER ) t->reqs[2] = 'U';
      else if ( us->req[k].user_type == BM_REQ_ALL  ) t->reqs[3] = 'A';
      else if ( us->req[k].freq < 100.0             ) t->reqs[0] = 'S';
    }
    strncpy(t->name,us->name,sizeof(t->name));
    t->name[sizeof(t->name)-1] = 0;
    t->partitionID = us->partid;
    t->processID   = us->pid;
    t->type        = 'U';
    t->state       = 0;
    t->events      = 0;
    if ( us->ev_produced>0 || us->get_sp_calls>0 )   {
      t->type      = 'P';
      t->state     = us->p_state+1;
      t->events    = us->ev_produced;
    }
    else if ( us->ev_actual>0 || us->get_ev_calls>0 || us->n_req>0 ) {
      t->type      = 'C';
      t->state     = us->c_state+1;
      t->events    = us->ev_seen;
    }
    t = mbm->clients.add(t);
  }
}

/// Standard constructor
ROMonInfo::ROMonInfo(RODimListener* s) : DimInfo("DIS_DNS/SERVER_LIST","DEAD"), m_server(s) {
}

/// Extract node/service name from DNS info
void ROMonInfo::getServiceNode(char* s, std::string& svc, std::string& node) const {
  char* at = strchr(s,'@');
  *at = 0;
  svc = s;
  node = at+1;
}

/// Add handler for a given message source
void ROMonInfo::addHandler(const std::string& node, const std::string& svc) {
  if ( m_server ) m_server->addHandler(node,svc);
}

/// Remove handler for a given message source
void ROMonInfo::removeHandler(const std::string& node, const std::string& svc) {
  if ( m_server ) m_server->removeHandler(node,svc);
}

/// DimInfo overload to process messages
void ROMonInfo::infoHandler()  {
  char c, *msg = getString();
  std::string svc, node;
  switch(c=msg[0]) {
  case '+':
    getServiceNode(++msg,svc,node);
    addHandler(node,svc);
    break;
  case '-':
    getServiceNode(++msg,svc,node);
    removeHandler(node,svc);
    break;
  case '!':
    getServiceNode(++msg,svc,node);
    std::cout << "Service " << msg << " in ERROR." << std::endl;
    break;
  default:
    if ( *(int*)msg != *(int*)"DEAD" )  {
      char *at, *p = msg, *last = msg;
      while ( last != 0 && (at=strchr(p,'@')) != 0 )  {
	last = strchr(at,'|');
	if ( last ) *last = 0;
	getServiceNode(p,svc,node);
	addHandler(node,svc);
	p = last+1;
      }
    }
    else  {
      std::cout << "DIM nameserver died...." << std::endl;
    }
    break;
  }
}

/// Standard constructor
RODimFSMListener::RODimFSMListener() {
  m_dns = new ROMonInfo(this);
}

/// Standard destructor
RODimFSMListener::~RODimFSMListener() {
  dim_lock();
  for (Clients::iterator i=m_clients.begin(); i != m_clients.end(); ++i)  {
    delete (*i).second.first;
  }
  delete m_dns;
  m_dns = 0;
  dim_unlock();
}

/// Add handler for a given message source
void RODimFSMListener::addHandler(const std::string& node,const std::string& svc)    {
  static std::string myNode = RTL::nodeNameShort();
  static std::string myUtgid = RTL::processName();
  if ( ::strncasecmp(node.c_str(),myNode.c_str(),myNode.length()) == 0 ) {
    if ( ::strncasecmp(svc.c_str(),myUtgid.c_str(),myUtgid.length()) != 0 ) {
      if ( ::strncasecmp(svc.c_str(),"DIS_DNS",7) != 0 ) {
	std::string nam = svc+"/fsm_status";
	dim_lock();
	Clients::iterator i=m_clients.find(nam);
	if ( i == m_clients.end() )  {
	  char def[32];
	  FSMTask task;
	  memset(def,0xFF,sizeof(def));
	  DimInfo* info = new DimInfo(nam.c_str(),(void*)def,sizeof(def),this);
	  std::cout << "Create DimInfo:" << nam << std::endl;
	  strncpy(task.name,svc.substr(0,svc.find("/")).c_str(),sizeof(task.name));
	  task.name[sizeof(task.name)-1] = 0;
	  task.processID = -1;
	  m_clients[nam]=std::make_pair(info,task);
	}
	dim_unlock();
      }
    }
  }
}

/// Remove handler for a given message source
void RODimFSMListener::removeHandler(const std::string& /* node */, const std::string& svc)   {
  std::string nam = svc+"/fsm_status";
  dim_lock();
  Clients::iterator i=m_clients.find(nam);
  std::cout << "Try to remove " << nam << std::endl;
  if ( i != m_clients.end() ) {
    std::cout << "Delete DimInfo:" << (*i).first << " " << (void*)(*i).second.first << std::endl;
    delete (*i).second.first;
    m_clients.erase(i);
  }
  dim_unlock();
}

/// DimInfo overload to process messages
void RODimFSMListener::infoHandler() {
  typedef LHCb::DimTaskFSM Task;
  const char* nam = getInfo()->getName();
  if ( nam ) {
    Task::FSMMonitoring* mon = (Task::FSMMonitoring*)getInfo()->getData();
    if ( mon && mon->pid != -1 ) {
      Clients::iterator i=m_clients.find(nam);
      if ( i != m_clients.end() ) {
	FSMTask& t = (*i).second.second;
	t.processID   = mon->pid;
	t.state       = mon->state;
	t.targetState = mon->targetState;
	t.metaState   = mon->metaState;
	t.lastCmd     = mon->lastCmd;
	t.doneCmd     = mon->lastCmd;
	// std::cout << "Info from " << nam << " " << Task::stateName(mon->state) << std::endl;
      }
    }
  }
}

/// Standard constructor
RODimNodeListener::RODimNodeListener() {
  m_dns = new ROMonInfo(this);
}

/// Standard destructor
RODimNodeListener::~RODimNodeListener() {
  delete m_dns;
  m_dns = 0;
  dim_lock();
  for (Clients::iterator i=m_clients.begin(); i != m_clients.end(); ++i)  {
    delete (*i).second.first;
    ::free((*i).second.second.data);
  }
  dim_unlock();
}

/// Add handler for a given message source
void RODimNodeListener::addHandler(const std::string& node,const std::string& /* svc */) {
  std::string nam = "RoMon_"+node.substr(0,node.find("."));
  dim_lock();
  Clients::iterator i=m_clients.find(nam);
  if ( i == m_clients.end() )  {
    Descriptor dsc;
    DimInfo* info = new DimInfo(nam.c_str(),(void*)&dsc,sizeof(dsc),this);
    std::cout << "[RODimNodeListener] Create DimInfo:" << nam << std::endl;
    m_clients[nam]=std::make_pair(info,dsc);
  }
  dim_unlock();
}

/// Remove handler for a given message source
void RODimNodeListener::removeHandler(const std::string& node, const std::string& /* svc */)   {
  std::string nam = "RoMon_"+node.substr(0,node.find("."));
  dim_lock();
  Clients::iterator i=m_clients.find(nam);
  if ( i != m_clients.end() ) {
    char* data = (*i).second.second.data;
    std::cout << "[RODimNodeListener] Delete DimInfo:" << (*i).first << " " << (void*)(*i).second.first << std::endl;
    delete (*i).second.first;
    ::free(data);
    m_clients.erase(i);
  }
  dim_unlock();
}

/// DimInfo overload to process messages
void RODimNodeListener::infoHandler() {
  const char* nam = getInfo()->getName();
  if ( nam ) {
    Clients::iterator i=m_clients.find(nam);
    if ( i != m_clients.end() ) {
      Descriptor& dsc = (*i).second.second;
      int len = getInfo()->getSize();
      if ( dsc.len < len ) {
	dsc.len = len;
	::free(dsc.data);
	dsc.data = (char*)::malloc(dsc.len);
      }
      dsc.actual = len;
      ::memcpy(dsc.data,getInfo()->getData(),dsc.actual);
    }
  }
}

/// Standard constructor
ROMonGblBuffer::ROMonGblBuffer(int argc, char** argv, void (*help_fun)()) 
  : m_delay(2000), m_print(0), m_gbl(0), m_lock(0),
    m_section_name("ROMonitor"), m_section_size(64)
{
  RTL::CLI cli(argc, argv, help_fun);
  cli.getopt("gbl",1,m_section_name);
  cli.getopt("size",1,m_section_size);
  cli.getopt("delay",1,m_delay);
  m_print = cli.getopt("print",1) != 0;

  m_section_size *= 1024;
  int sc = ::lib_rtl_create_section(m_section_name.c_str(),m_section_size,&m_gbl);
  if ( !lib_rtl_is_success(sc) ) {
    ::lib_rtl_printf("Failed to create ROMON global section:%s\n",m_section_name.c_str());
    return;
  }
  sc = ::lib_rtl_create_lock(m_section_name.c_str(),&m_lock);
  if ( !lib_rtl_is_success(sc) ) {
    ::lib_rtl_printf("Failed to create ROMON global lock:%s\n",m_section_name.c_str());
    return;
  }
}

/// Default destructor
ROMonGblBuffer::~ROMonGblBuffer() {
  if ( m_gbl ) {
    ::lib_rtl_delete_section(m_gbl);
    m_gbl = 0;
  }
  if ( m_lock ) {
    ::lib_rtl_cancel_lock(m_lock);
    ::lib_rtl_delete_lock(m_lock);
    m_lock = 0;
  }
}

/// Help printout in case of -h /? or wrong arguments
void ROMonGblBuffer::help() {
  ::lib_rtl_printf("    -g[bl]=<name>     Name of shared global section\n");
  ::lib_rtl_printf("    -s[ize]=<number>  Size of the global section in kBytes\n");
  ::lib_rtl_printf("    -d[elay]=<number> Delay in milliseconds between two updates of the information\n");
  ::lib_rtl_printf("    -p[rint]=<number> Make printout to stdout\n");  
}

/// Standard constructor
ROMonTaskCollector::ROMonTaskCollector(int argc, char** argv) 
: ROMonGblBuffer(argc,argv,ROMonTaskCollector::help), m_mbmInfo(0), m_fsmInfo()
{
}

/// Help printout in case of -h /? or wrong arguments
void ROMonTaskCollector::help() {
  ::lib_rtl_printf("romon_collect -opt [-opt]\n");
  ROMonGblBuffer::help();
}

/// Start monitoring activity
int ROMonTaskCollector::monitor() {
  if ( m_gbl ) {
    typedef RODimFSMListener::Clients Clients;
    void* addr = m_gbl->address;
    bool exec = true;
    while(exec)    {
      Node* n = (Node*)addr;
      if ( n ) {  // Take GBL lock while updating
	RTL::Lock lock(m_lock);
	m_mbmInfo.handle(n, m_section_size);
	Node::Tasks* tasks = n->tasks();
	Node::Tasks::iterator it = tasks->reset();
	const Clients& cl = m_fsmInfo.clients();
	if ( ((char*)it) > ((char*)addr)+m_section_size ) {
	  std::cout << "Global section memory too small.....exiting" << std::endl;
	  break;
	}
	else {
	  dim_lock();
	  for(Clients::const_iterator ic = cl.begin(); ic != cl.end(); ++ic) {
	    const FSMTask& t = (*ic).second.second;
	    if ( ((char*)it) > ((char*)addr)+m_section_size ) {
	      std::cout << "Global section memory too small.....exiting" << std::endl;
	      dim_unlock();
	      break;
	    }
	    if ( t.processID != -1 ) {
	      ::memcpy(it,&t,sizeof(FSMTask));
	      it = tasks->add(it);
	    }
	  }
	  dim_unlock();
	  n->fixup();
	}
      }
      if ( m_print ) {
	std::cout << "========================" << ::lib_rtl_timestr() 
		  << "========================" << std::endl
		  << *n << std::endl;
      }
      ::lib_rtl_sleep(m_delay);
    }
    return 1;
  }
  std::cout << "ROMon gbl invalid....exiting." << std::endl;
  return 0;
}

/// Standard constructor
ROMonNodeCollector::ROMonNodeCollector(int argc, char** argv) 
: ROMonGblBuffer(argc,argv,ROMonNodeCollector::help)
{
}

/// Help printout in case of -h /? or wrong arguments
void ROMonNodeCollector::help() {
  ::lib_rtl_printf("romon_node_collect -opt [-opt]\n");
  ROMonGblBuffer::help();
}

/// Start monitoring activity
int ROMonNodeCollector::monitor() {
  if ( m_gbl ) {
    typedef RODimNodeListener::Clients Clients;
    char* addr = (char*)m_gbl->address;
    bool exec = true;
    while(exec)    {
      Nodeset* n = (Nodeset*)addr;
      if ( n ) {  // Take GBL lock while updating
	RTL::Lock lock(m_lock);
	Nodeset::Nodes& nodes = n->reset()->nodes;
	Nodeset::Nodes::iterator it = nodes.reset();
	const Clients& cl = m_info.clients();
	::strncpy(n->name,RTL::nodeName().c_str(),sizeof(n->name));
	n->name[sizeof(n->name)-1] = 0;
	if ( ((char*)it) > addr+m_section_size ) {
	  std::cout << "Global section memory too small.....exiting" << std::endl;
	  break;
	}
	else {
	  dim_lock();
	  for(Clients::const_iterator ic = cl.begin(); ic != cl.end(); ++ic) {
	    if ( ((char*)it) > addr+m_section_size ) {
	      std::cout << "Global section memory too small.....exiting" << std::endl;
	      dim_unlock();
	      break;
	    }
	    ::memcpy(it,(*ic).second.second.data,(*ic).second.second.actual);
	    it = nodes.add(it);
	  }
	  dim_unlock();
	}
      }
      if ( m_print ) {
	std::cout << "========================" << ::lib_rtl_timestr() 
		  << "========================" << std::endl
		  << *n << std::endl;
      }
      ::lib_rtl_sleep(m_delay);
    }
    return 1;
  }
  std::cout << "ROMon gbl invalid....exiting." << std::endl;
  return 0;
}

/// Standard constructor
ROMonPublish::ROMonPublish(int argc, char** argv) 
  : m_delay(2000), m_type(NODE_TYPE), m_section_name("ROMonitor"), m_section_size(64)
{
  RTL::CLI cli(argc, argv, ROMonPublish::help);
  cli.getopt("gbl",1,m_section_name);
  cli.getopt("size",1,m_section_size);
  cli.getopt("delay",1,m_delay);
  m_verbose = cli.getopt("verbose",1) != 0;
  m_section_size *= 1024;
}

/// Help callback
void ROMonPublish::help() {
  ::lib_rtl_printf("romon_gbl_dump -opt [-opt]\n");
  ::lib_rtl_printf("    -g[bl]=<name>     Name of shared global section\n");
  ::lib_rtl_printf("    -s[ize]=<number>  Size of the global section in kBytes\n");
  ::lib_rtl_printf("    -d[elay]=<number> Delay between two updates of the information\n");
  ::lib_rtl_printf("    -t[ype]=<number>  NODE:1 NODESET:2 Default:%d\n",NODE_TYPE);
  ::lib_rtl_printf("    -v[erbose]        Verbose mode.\n");
}

/// Start monitoring activity
int ROMonPublish::monitor() {
  lib_rtl_gbl_t gbl;
  bool exec = true;
  while(exec)    {
    int sc = ::lib_rtl_map_section(m_section_name.c_str(),m_section_size,&gbl);
    if ( !lib_rtl_is_success(sc) ) {
      ::lib_rtl_printf("Failed to map ROMON global section:%s\n",m_section_name.c_str());
    }
    else {
      if ( m_verbose ) std::cout << "Connected to global section:" << m_section_name << std::endl;
      publish(gbl->address);
      ::lib_rtl_unmap_section(gbl);
    }
    gbl = 0;
    ::lib_rtl_sleep(m_delay);
  }
  return 1;
}

/// Publish the collected information
void ROMonAsciiPublish::publish(void* data) {
  std::cout << "========================" << lib_rtl_timestr() << "========================" << std::endl;
  switch(m_type)  {
  case NODE_TYPE:
    std::cout << *(Node*)data << std::endl;
    break;
  case NODESET_TYPE:
    std::cout << *(Nodeset*)data << std::endl;
    break;
  default:
    std::cout << "Unknown data type -- cannot be published." << std::endl;
    break;
  }
}

/// Standard constructor
ROMonDimPublish::ROMonDimPublish(int argc , char** argv) : ROMonPublish(argc,argv) {
  RTL::CLI cli(argc, argv, ROMonPublish::help);
  cli.getopt("publish",1,m_svcName);
  switch(m_type)  {
  case NODE_TYPE:
    m_service = ::dis_add_service((char*)("RoMon_"+RTL::nodeNameShort()).c_str(),"C",0,0,feedData,(long)this);
    break;
  case NODESET_TYPE:
    m_service = ::dis_add_service((char*)("RoMon_"+RTL::nodeNameShort()).c_str(),"C",0,0,feedData,(long)this);
    break;
  default:
    if ( !m_svcName.empty() )  {
      m_service = ::dis_add_service((char*)m_svcName.c_str(),"C",0,0,feedData,(long)this);
    }
    else  {
      std::cout << "Unknown data type -- cannot be published." << std::endl;
      throw std::runtime_error("Unknown data type and unknwon service name -- cannot be published.");
    }
    break;
  }
}

/// Default destructor
ROMonDimPublish::~ROMonDimPublish() {
  dis_remove_service(m_service);
}

void ROMonDimPublish::feedData(void* tag, void** buf, int* size, int* first) {
  static const char* empty = "";
  ROMonDimPublish* h = *(ROMonDimPublish**)tag;
  if ( *first ) {
    *size = 0;
    *buf = (void*)empty;
    return;
  }
  *size = h->m_data.first;
  *buf = h->m_data.second;
}

/// Publish the collected information
void ROMonDimPublish::publish(void* data) {
  switch(m_type)  {
  case NODE_TYPE:
    m_data.first  = ((Node*)data)->totalSize;
    break;
  case NODESET_TYPE:
    m_data.first  = ((Nodeset*)data)->length();
    break;
  default:
    std::cout << "Unknown data type -- cannot be published." << std::endl;
    break;
  }
  m_data.second = (char*)data;
  int nclients  = ::dis_update_service(m_service);
  std::cout << "[ROMonDimPublish] Publish to " << nclients << " " << m_data.first
	    << " bytes @" << (void*)data << std::endl;
}

extern "C" int romon_test_structs(int, char**) {
  size_t len = 1024*64;
  char* buff = new char[len];
  typedef FixItems<int> IntItems;
  IntItems* i_items = (IntItems*)buff;
  i_items->reset();
  std::cout << *i_items << std::endl;
  for(int i=0, *i_it=i_items->begin(); i<10; ++i) {
    *i_it = i;
    i_it=i_items->add(i_it);
  }
  std::cout << *i_items << std::endl;
  std::cout << fill((MBMBuffer*)buff, 0x14d, "Events") << std::endl;
  std::cout << fill((Node*)buff, "storectl01") << std::endl;
  delete [] buff;
  return 1;
}

extern "C" int romon_test_serv(int, char**) {
  size_t len = 32*1024;
  char* buff = new char[len];
  ROMonServer mon(2000);
  int sc = mon.monitor(buff,len);
  ::delete [] buff;
  return sc;
}

extern "C" int romon_test_dim_info(int, char**) {
  ROMonInfo info(0);
  DimServer::start(RTL::processName().c_str());
  std::cout << "Going asleep" << std::endl;
  while(1) {
    lib_rtl_sleep(1000);
  }
  return 1;
}

extern "C" int romon_test_fsm_listener(int, char**) {
  RODimFSMListener listener;
  DimServer::start(RTL::processName().c_str());
  std::cout << "Going asleep" << std::endl;
  while(1) {
    lib_rtl_sleep(1000);
  }
  return 1;
}

extern "C" int romon_collect(int argc, char** argv) {
  ROMonTaskCollector romon(argc,argv);
  DimServer::start(RTL::processName().c_str());
  return romon.monitor();
}

extern "C" int romon_gbl_dump(int argc, char** argv) {
  ROMonAsciiPublish romon(argc,argv);
  return romon.monitor();
}

extern "C" int romon_dimserver(int argc, char** argv) {
  ROMonDimPublish romon(argc,argv);
  DimServer::start(RTL::processName().c_str());
  return romon.monitor();
}

extern "C" int romon_node_collect(int argc, char** argv) {
  ROMonNodeCollector romon(argc,argv);
  DimServer::start(RTL::processName().c_str());
  return romon.monitor();
}

