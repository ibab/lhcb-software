#ifndef ROMON_STORAGEMONITOR_H
#define ROMON_STORAGEMONITOR_H

// Framework include files
#include "ROMon/FarmMonitor.h"
// C++ include files
#include <map>
#include <set>

namespace ROMon {
  class Nodeset;

  class StorageMonitor : public InternalMonitor {
    struct BuffMon {
      int produced;
      int clients;
      BuffMon() : produced(0), clients(0)
      {}
    };
    struct NodeMon {
      typedef std::set<std::string> Tasks;
      typedef std::map<std::string,long> Clients;
      int     buffers;
      int     clients;
      int     minIN, maxIN;
      int     evtIN, evtOUT;
      int     rxEvt, txEvt;
      float   space;
      float   slots;
      time_t  update;
      char    inUse;
      Clients senders, receivers;
      Tasks   tasks;
      NodeMon() {   reset(); }
      NodeMon(const NodeMon& c) { *this = c; }
      NodeMon& operator=(const NodeMon& c);
      void reset();
    };

    typedef std::map<std::string,NodeMon*>  Monitors;
    std::string m_partition;
    Monitors   m_nodes;
    Monitors   m_history;
    NodeMon    m_sum;
    NodeMon    m_sumHist;

    /// Set a new alarm
    void setAlarm(Alarms& alms, const std::string& node, int type, time_t when, const std::string& opt="");

    void extractData(const Nodeset& ns);
    void analyzeData();

  public:
    /// Access to problem flag
    bool hasProblems() const { return m_hasProblems; }
    /// Initializing constructor
    StorageMonitor(FarmMonitor* parent, const std::string& title);
    /// Standard destructor
    virtual ~StorageMonitor();
    /// Reset for new initialization
    virtual void reset();

    /// Count the number of tasks
    virtual void countTasks(const Nodeset&) {}
    /// Count the number of events in all buffers
    virtual void countEvents(const Nodeset&) {}
  };
  InternalMonitor* createStorageMonitor(FarmMonitor* parent, const std::string& title);
}
#endif // ROMON_STORAGEMONITOR_H

#define MBM_IMPLEMENTATION
#include "ROMon/ROMon.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "ROMonDefs.h"
extern "C" {
#include "dic.h"
}
#include <limits>

using namespace ROMon;
using namespace std;

namespace {
  const char* _procNam(const char* nam) {
    const char* p;
    p = ::strchr(nam,'_');
    if ( 0 != p ) {
      p = ::strchr(++p,'_');
      if ( 0 != p )
        return ++p;
    }
    return "Unknown";
  }
}

InternalMonitor* ROMon::createStorageMonitor(FarmMonitor* parent, const string& title) {
  cout << "Connecting to farm " << title << endl;
  InternalMonitor* m = new StorageMonitor(parent,title);
  return m;
}

StorageMonitor::NodeMon& StorageMonitor::NodeMon::operator=(const NodeMon& c) {
  if ( this != &c ) {
    buffers   = c.buffers;
    clients = c.clients;
    minIN      = c.minIN;
    maxIN      = c.maxIN;
    evtIN      = c.evtIN;
    evtOUT     = c.evtOUT;
    space     = c.space;
    slots     = c.slots;
    update     = c.update;
    inUse      = c.inUse;
    senders    = c.senders;
    receivers  = c.receivers;
    tasks      = c.tasks;
    rxEvt      = c.rxEvt;
    txEvt      = c.txEvt;
  }
  return *this;
}

void StorageMonitor::NodeMon::reset() {
  buffers = 0;
  clients = 0;
  minIN      = maxIN  = 0;
  evtIN      = evtOUT  = 0;
  rxEvt      = txEvt = -1;
  space     = 0;
  slots     = 0;
  update     = 0;
  inUse      = 0;
  senders.clear();
  receivers.clear();
  // Do NOT clear "tasks" here!
}

/// Initializing constructor
StorageMonitor::StorageMonitor(FarmMonitor* parent, const string& title) 
  : InternalMonitor(parent,title) 
{
  m_steer &= ~COUNT_TASKS;
  m_steer &= ~COUNT_EVENTS;
  m_partition = parent->partition();
  string svc = svcPrefix()+strlower(title)+"/ROpublish";
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
  m_title = svc;
}

/// Standard destructor
StorageMonitor::~StorageMonitor() {
  for(Monitors::iterator i=m_nodes.begin(); i!=m_nodes.end();++i)
    delete (*i).second;
  for(Monitors::iterator j=m_history.begin(); j!=m_history.end();++j)
    delete (*j).second;
}

/// Reset for new initialization
void StorageMonitor::reset() {
  for(Monitors::iterator i=m_nodes.begin(); i!=m_nodes.end();++i)
    (*i).second->tasks.clear();
  for(Monitors::iterator j=m_history.begin(); j!=m_history.end();++j)
    (*j).second->tasks.clear();
}

void StorageMonitor::extractData(const Nodeset& ns) {
  m_lastUpdate = ns.firstUpdate().first;
  NodeMon sum;
  sum.minIN = numeric_limits<int>::max();
  sum.maxIN = numeric_limits<int>::min();
  sum.space = numeric_limits<float>::max();
  sum.slots = numeric_limits<float>::max();

  for (Nodeset::Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    const Node::Buffers& buffs = *(*n).buffers();
    const string node = (*n).name;
    if ( m_history.find(node) == m_history.end() )
      m_history[node] = new NodeMon();
    if ( m_nodes.find(node) == m_nodes.end() )
      m_nodes[node] = new NodeMon();
    else
      *(m_history[node]) = *(m_nodes[node]);
    *(m_history[node]) = *(m_nodes[node]);
    NodeMon& m = *(m_nodes[node]);

    m.reset();
    m.update = (*n).time;
    for(Node::Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      char b = (*ib).name[0];
      const MBMBuffer::Control& ctrl = (*ib).ctrl;
      const MBMBuffer::Clients& clients = (*ib).clients;

      // Filter buffer names. We are not interested in all buffers - only the partition relevant ones
      if ( b != EVT_BUFFER || ::strncasecmp((*ib).name+7,m_partition.c_str(),m_partition.length()) != 0 ) 
        continue;

      ++m.buffers;
      //switch(b) {
      //case EVT_BUFFER:
      m.inUse  = 1;
      m.evtIN  = ctrl.tot_produced;
      m.space = float(ctrl.i_space)/float(ctrl.bm_size);
      m.slots = float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax);
      //  break;
      //default:
      //  continue;
      //}
      m.minIN = numeric_limits<int>::max();
      m.maxIN = numeric_limits<int>::min();
      for (MBMBuffer::Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
        int nevt = (*ic).events;
        const char* nam = (*ic).name;
        const char* p = _procNam(nam);

        ++m.clients;
        m.tasks.insert(nam);
        //cout << "Check task:" << nam << " " << p << "  " << nevt << endl;
        switch(*p) {
        case 'W':
          // Writers to storage:         WRTxxxxx
          if ( p[1]=='R' && p[2]=='T' )  {
            m.senders[nam] = nevt;
            m.evtOUT      += nevt;
          }
          break;
        case 'S':
          // Senders to streaming layer  SNDxxxx
          if ( p[1]=='N' && p[2]=='D' )  {
            m.senders[nam] = nevt;
            m.evtOUT      += nevt;
          }
          // HLT receivers               SF0x_HLT
          else if ( p[1] == 'F' ) {
            m.receivers[nam] = nevt;
            m.evtIN += nevt;
            m.maxIN  = max(m.maxIN,nevt);
            m.minIN  = min(m.minIN,nevt);
          }
          break;
        case 'R':
          // Receivers on streaming layer RCVxxxx
          m.receivers[nam] = nevt;
          if ( p[3] != '_' )  { // "RCV_Rec"
            m.evtIN += nevt;
            m.maxIN  = max(m.maxIN,nevt);
            m.minIN  = min(m.minIN,nevt);
          }
          break;
        default:
          break;
        }
      }
    }
    sum.buffers    += m.buffers;
    sum.clients    += m.clients;
    sum.evtIN      += m.evtIN;
    sum.evtOUT     += m.evtOUT;
    sum.minIN       = min(m.minIN,sum.minIN);
    sum.maxIN       = max(m.maxIN,sum.maxIN);
    sum.space       = min(m.space,sum.space);
    sum.slots       = min(m.slots,sum.slots);
  }
  m_sumHist    = m_sum;
  m_sum        = sum;
}

void StorageMonitor::analyzeData() {
  time_t now = time(0); 
  bool    running  = parent()->isRunning();
  NodeMon::Clients::const_iterator ircv, isnd, ih;
  auto_ptr<AlarmInfo> alarms(new AlarmInfo(m_name,Alarms()));

  m_hasProblems = false;
  for(Monitors::iterator i=m_nodes.begin(); i!=m_nodes.end();++i) {
    const string& node = (*i).first;
    NodeMon& m = *((*i).second);
    NodeMon& h = *(m_history[(*i).first]);
    time_t   when = m.update;
    
    if ( now-m.update > m_snapshotDiff )
      setAlarm(alarms->second,node,ERR_NO_UPDATES,when);
    else if ( !m.inUse )
      setAlarm(alarms->second,node,ERR_NOT_USED,when);
    else   {
      for(NodeMon::Tasks::const_iterator im=m.tasks.begin(); im!=m.tasks.end(); ++im)  {
        ircv = m.receivers.find(*im);
        isnd = m.senders.find(*im);
        const string& nam = *im;
        if ( ircv != m.receivers.end() ) {
          long nevt = (*ircv).second;
          ih = h.receivers.find(*im);
          // In a steady system there is no activity!
          if ( !running && ih != h.senders.end() && nevt == (*ih).second && m_sum.evtIN == m_sumHist.evtIN )
          {}
          // RECEIVER stuck while running
          else if ( nevt == 0 && m_sum.evtIN > m_sumHist.evtIN )
            setAlarm(alarms->second,node,ERR_RECEIVER_STUCK,when,nam);
          // RECEIVER stuck while running
          else if ( ih != h.receivers.end() && nevt  <= (*ih).second  && m_sum.evtIN > m_sumHist.evtIN )
            setAlarm(alarms->second,node,ERR_RECEIVER_STUCK,when,nam);
          continue;
        }
        else if ( isnd == m.senders.end() )  {
          setAlarm(alarms->second,node,ERR_RECEIVER_MISSING,when,nam);
          continue;
        }
        if ( isnd != m.senders.end() ) {
          long nevt = (*isnd).second;
          ih = h.senders.find(*im);
          
          // In a steady system there is no activity!
          if ( !running && ih != h.senders.end() && nevt == (*ih).second && m_sum.evtOUT == m_sumHist.evtOUT )
          {}
          // SENDER stuck while running
          else if ( nevt == 0 && m_sum.evtOUT > m_sumHist.evtOUT )
            setAlarm(alarms->second,node,ERR_SENDER_STUCK,when,nam);
          // SENDER stuck while running
          else if ( ih != h.senders.end() && nevt <= (*ih).second && m_sum.evtOUT > m_sumHist.evtOUT )
            setAlarm(alarms->second,node,ERR_SENDER_STUCK,when,nam);
          continue;
        }
        else if ( ircv == m.receivers.end() ) {
          setAlarm(alarms->second,node,ERR_SENDER_MISSING,when,nam);
          continue;
        }
      }

      // Remaining alarms may only occur while running
      if ( !running ) continue;
      
      if ( m.slots < SLOTS_MIN ) {
        if ( m.evtIN == h.evtIN )
          setAlarm(alarms->second,node,ERR_NODE_STUCK,when," Events");
        else
          setAlarm(alarms->second,node,ERR_SLOTS_LIMIT,when," Events");
      }
      else if ( m.space < SPACE_MIN ) {
        if ( m.evtIN == h.evtIN )
          setAlarm(alarms->second,node,ERR_NODE_STUCK,when," Events");
        else
          setAlarm(alarms->second,node,ERR_SPACE_LIMIT,when," Events");
      }
      else if ( m.evtIN > 0 && m.evtIN <= h.evtIN && m_sum.evtIN  == m_sumHist.evtIN ) { // No activity IN buffer
        setAlarm(alarms->second,node,ERR_NOSTORAGE_ACTIVITY,when);
      }
    }
  }
  IocSensor::instance().send(m_parent,CMD_CHECK,alarms.release());
}

/// Set a new alarm
void StorageMonitor::setAlarm(Alarms& alms, const string& node, int typ, time_t when, const string& opt) {
  alms.push_back(Alarm(typ,when,"storectl01",node,opt));
  m_hasProblems = true;
}
