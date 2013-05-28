#ifndef ROMON_MONITORINGMONITOR_H
#define ROMON_MONITORINGMONITOR_H

// Framework include files
#include "ROMon/FarmMonitor.h"
// C++ include files
#include <map>
#include <set>

namespace ROMon {
  class Nodeset;

  class MonitoringMonitor : public InternalMonitor {
    struct BuffMon {
      int produced;
      int clients;
      BuffMon() : produced(0), clients(0)
      {}
    };
    struct NodeMon {
      typedef std::set<std::string> Tasks;
      typedef std::map<std::string,std::pair<int,int> > Clients;
      int     numBuffs;
      int     numClients;
      int     minIN, maxIN;
      int     evtIN,  evtOUT;
      int     rxEvt, txEvt;
      float   spacIN, spacOUT;
      float   slotIN, slotOUT;
      time_t  update;
      char    inUse;
      Clients monitors;
      Tasks   tasks;
      NodeMon() {   reset(); }
      NodeMon(const NodeMon& c) { *this = c; }
      NodeMon& operator=(const NodeMon& c);
      void reset();
    };

    typedef std::map<std::string,NodeMon*>  Monitors;
    bool        m_isReco;

    std::string m_partition;
    Monitors    m_nodes;
    Monitors    m_history;
    NodeMon     m_sum;
    NodeMon     m_sumHist;

    void extractData(const Nodeset& ns);
    void analyzeData();

  public:
    /// Access to problem flag
    bool hasProblems() const { return m_hasProblems; }
    /// Initializing constructor
    MonitoringMonitor(FarmMonitor* parent, const std::string& title);
    /// Standard destructor
    virtual ~MonitoringMonitor();
    /// Reset for new initialization
    virtual void reset();

    /// Count the number of tasks
    virtual void countTasks(const Nodeset&) {}
    /// Count the number of events in all buffers
    virtual void countEvents(const Nodeset&) {}
  };
  InternalMonitor* createMonitoringMonitor(FarmMonitor* parent, const std::string& title);
}
#endif // ROMON_MONITORINGMONITOR_H

#include "RTL/strdef.h"
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

InternalMonitor* ROMon::createMonitoringMonitor(FarmMonitor* parent, const string& title) {
  cout << "Connecting to farm " << title << endl;
  InternalMonitor* m = new MonitoringMonitor(parent,title);
  return m;
}

MonitoringMonitor::NodeMon& MonitoringMonitor::NodeMon::operator=(const NodeMon& c) {
  if ( this != &c ) {
    numBuffs   = c.numBuffs;
    numClients = c.numClients;
    minIN      = c.minIN;
    maxIN      = c.maxIN;
    evtIN      = c.evtIN;
    evtOUT     = c.evtOUT;
    spacIN     = c.spacIN;
    spacOUT    = c.spacOUT;
    slotIN     = c.slotIN;
    slotOUT    = c.slotOUT;
    update     = c.update;
    inUse      = c.inUse;
    monitors   = c.monitors;
    tasks      = c.tasks;
    rxEvt      = c.rxEvt;
    txEvt      = c.txEvt;
  }
  return *this;
}

void MonitoringMonitor::NodeMon::reset() {
  numBuffs = 0;
  numClients = 0;
  minIN      = maxIN  = 0;
  evtIN      = evtOUT  = 0;
  rxEvt      = txEvt = -1;
  spacIN     = spacOUT = 0;
  slotIN     = slotOUT = 0;
  update     = 0;
  inUse      = 0;
  monitors.clear();
  // Do NOT clear "tasks" here!
}

/// Initializing constructor
MonitoringMonitor::MonitoringMonitor(FarmMonitor* parent, const string& title) 
  : InternalMonitor(parent,title) 
{
  m_partition = parent->partition();
  m_steer &= ~COUNT_TASKS;
  m_steer &= ~COUNT_EVENTS;
  m_isReco = false;
  string svc, node=strlower(title);
  svc = svcPrefix()+node+"/ROpublish";
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
  m_title = svc;
  if ( node.substr(0,6) == "mona09" )   {
    m_isReco = true;
    m_snapshotDiff *= 3;
    cout << node << "> Update diff " << m_snapshotDiff << endl;
  }
}

/// Standard destructor
MonitoringMonitor::~MonitoringMonitor() {
  for(Monitors::iterator i=m_nodes.begin(); i!=m_nodes.end();++i)
    delete (*i).second;
  for(Monitors::iterator j=m_history.begin(); j!=m_history.end();++j)
    delete (*j).second;
}

/// Reset for new initialization
void MonitoringMonitor::reset() {
  for(Monitors::iterator i=m_nodes.begin(); i!=m_nodes.end();++i)
    (*i).second->tasks.clear();
  for(Monitors::iterator j=m_history.begin(); j!=m_history.end();++j)
    (*j).second->tasks.clear();
}

void MonitoringMonitor::extractData(const Nodeset& ns) {
  m_lastUpdate = ns.firstUpdate().first;
  NodeMon sum;
  sum.minIN    = numeric_limits<int>::max();
  sum.maxIN    = numeric_limits<int>::min();
  sum.spacIN   = numeric_limits<float>::max();
  sum.spacOUT  = numeric_limits<float>::max();
  sum.slotIN   = numeric_limits<float>::max();
  sum.slotOUT  = numeric_limits<float>::max();

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
    m_numTask = 0;
    m_numEvent = 0;
    for(Node::Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      char b = (*ib).name[0];
      const MBMBuffer::Control& ctrl = (*ib).ctrl;
      const MBMBuffer::Clients& clients = (*ib).clients;

      // Filter buffer names. We are not interested in all buffers - only the partition relevant ones
      if ( b == EVT_BUFFER && ::strncasecmp((*ib).name+7,m_partition.c_str(),m_partition.length()) != 0 ) 
        continue;
      else if ( b == INPUT_BUFFER && ::strncasecmp((*ib).name+6,m_partition.c_str(),m_partition.length()) != 0 ) 
        continue;
      else if ( b == SND_BUFFER && ::strncasecmp((*ib).name+5,m_partition.c_str(),m_partition.length()) != 0 ) 
        continue;
      else if ( b == OUTPUT_BUFFER && ::strncasecmp((*ib).name+7,m_partition.c_str(),m_partition.length()) != 0 ) 
        continue;

      ++m.numBuffs;
      m_numEvent += ctrl.tot_produced;
      switch(b) {
      case EVT_BUFFER:
      case INPUT_BUFFER:
        m.inUse  = 1;
        m.evtIN  = ctrl.tot_produced;
        m.spacIN = float(ctrl.i_space)/float(ctrl.bm_size);
        m.slotIN = float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax);
        break;
      case SND_BUFFER:
      case OUTPUT_BUFFER:
        m.inUse   = 1;
        m.evtOUT  = ctrl.tot_produced;
        m.spacOUT = float(ctrl.i_space)/float(ctrl.bm_size);
        m.slotOUT = float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax);
        break;
      default:
        continue;
      }
      m.minIN = numeric_limits<int>::max();
      m.maxIN = numeric_limits<int>::min();
      for (MBMBuffer::Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
        int nevt = (*ic).events;
        const char* nam = (*ic).name;
        ++m.numClients;
        const char* p = _procNam((*ic).name);
        switch(*p) {
        case REC_SENDER_TASK:
          if ( strncmp(p,"SND",3)==0 ) {
            m.txEvt = nevt;
          }
          break;
        case REC_RECEIVER_TASK:
          if ( strncmp(p,"RCV",3)==0 ) {
            m.maxIN = max(m.maxIN,nevt);
            m.minIN = min(m.minIN,nevt);
            m.rxEvt = nevt;
          }
          break;
        default:
          //  Normal  monitoring tasks:
          if ( m.tasks.find(nam)==m.tasks.end() )   {
            m.tasks.insert(nam);
          }
          if( b == EVT_BUFFER || b == INPUT_BUFFER )
            m.monitors[nam].first = nevt;
          else if (b == SND_BUFFER || b == OUTPUT_BUFFER )
            m.monitors[nam].second = nevt;
          break;
        }
      }
    }
    sum.numBuffs   += m.numBuffs;
    sum.numClients += m.numClients;
    sum.evtIN      += m.evtIN;
    sum.evtOUT     += m.evtOUT;
    sum.minIN       = min(m.minIN,sum.minIN);
    sum.maxIN       = max(m.maxIN,sum.maxIN);
    sum.spacIN      = min(m.spacIN,sum.spacIN);
    sum.spacOUT     = min(m.spacOUT,sum.spacOUT);
    sum.slotIN      = min(m.slotIN,sum.slotIN);
    sum.slotOUT     = min(m.slotOUT,sum.slotOUT);
  }
  m_sumHist    = m_sum;
  m_sum        = sum;
}

void MonitoringMonitor::analyzeData() {
  char txt[128];
  time_t now = time(0);
  auto_ptr<AlarmInfo> alarms(new AlarmInfo(m_name,Alarms()));
  m_hasProblems = false;
  for(Monitors::iterator i=m_nodes.begin(); i!=m_nodes.end();++i) {
    const string& node = (*i).first;
    bool check_senders = node.substr(0,6)=="mona09" || node=="mona0801";
    bool check_outbuf  = check_senders && node != "mona0801" && node != "mona0901";
    bool check_receivers = true;
    bool recv_dead = false, send_dead = true;
    NodeMon& m = *((*i).second);
    NodeMon& h = *(m_history[(*i).first]);
    time_t   when = m.update;
    
    txt[0] = 0;
    if ( now-m.update > m_snapshotDiff )
      setAlarm(alarms->second,node,ERR_NO_UPDATES,when);
    else if ( !m.inUse )
      setAlarm(alarms->second,node,ERR_NOT_USED,when);
    else   {
      NodeMon::Clients::const_iterator ih;
      if ( check_receivers ) {
        if ( m.rxEvt < 0 )   {                                                // RECEIVER DEAD/not present
          setAlarm(alarms->second,node,ERR_RECEIVER_MISSING,when);
          recv_dead = true;
          cout << "DECLARE: " << (*i).first << "  " << when << " Rx:" << m.rxEvt << endl;
        }
        else if ( m.rxEvt == 0 && m_sum.evtIN > m_sumHist.evtIN )   {         // RECEIVER stuck while running
          setAlarm(alarms->second,node,ERR_RECEIVER_STUCK,when);
          cout << "DECLARE: " << (*i).first << "  " << when << " Rx:" << m.rxEvt << endl;
        }
        else if ( m.evtIN  <= h.evtIN  && m_sum.evtIN > m_sumHist.evtIN )     // RECEIVER stuck while running
          setAlarm(alarms->second,node,ERR_RECEIVER_STUCK,when);
      }
      if ( check_senders ) {
        if ( m.txEvt < 0 )   {                                                // SENDER DEAD/not present
          setAlarm(alarms->second,node,ERR_SENDER_MISSING,when);
          send_dead = true;
          cout << "DECLARE: " << (*i).first << "  " << when << " Tx:" << m.txEvt << endl;
        }
        else if ( m.txEvt == 0 && m_sum.evtOUT > m_sumHist.evtOUT )  {        // SENDER stuck while running
          setAlarm(alarms->second,node,ERR_SENDER_STUCK,when);
          cout << "DECLARE: " << (*i).first << "  " << when << " Tx:" << m.txEvt << endl;
        }
        else if ( m.evtOUT  <= h.evtOUT  && m_sum.evtOUT > m_sumHist.evtOUT ){// SENDER stuck while running
          setAlarm(alarms->second,node,ERR_SENDER_STUCK,when);
        }
      }
      for(NodeMon::Tasks::const_iterator im=m.tasks.begin(); im!=m.tasks.end(); ++im)  {
        if ( m.monitors.find(*im) == m.monitors.end() )
          setAlarm(alarms->second,node,ERR_MONITOR_MISSING,when,*im);
      }
      if ( m_sum.evtIN > m_sumHist.evtIN ) {  // Need some minimal activity....
        for(NodeMon::Clients::const_iterator ic=m.monitors.begin(); ic!=m.monitors.end(); ++ic) {
          if ( (*ic).second.first <= 1 ) continue;
          if ( (ih=h.monitors.find((*ic).first)) == h.monitors.end() ) continue;
          if ( (*ic).second.first <= (*ih).second.first ) {
            setAlarm(alarms->second,node,ERR_MONITOR_STUCK,when,(*ic).first);
          }
        }
      }
      
      txt[0] = 0;
      if ( m.slotIN < SLOTS_MIN || (check_outbuf && m.slotOUT < SLOTS_MIN) ) {
        if ( !m_isReco && !(recv_dead || send_dead) ) {
          if ( m.slotIN    < SLOTS_MIN ) ::strcat(txt," INPUT");
          if ( check_outbuf && m.slotOUT   < SLOTS_MIN ) ::strcat(txt," OUTPUT");
          if ( m.evtIN == h.evtIN || (check_outbuf && m.evtOUT == h.evtOUT) )
            setAlarm(alarms->second,node,ERR_NODE_STUCK,when, txt);
          else
            setAlarm(alarms->second,node,ERR_SLOTS_LIMIT,when, txt);
        }
      }
      else if ( m.spacIN < SPACE_MIN || (check_outbuf && m.spacOUT < SPACE_MIN) ) {
        if ( !m_isReco && !(recv_dead || send_dead) ) {
          if ( m.spacIN    < SPACE_MIN ) ::strcat(txt," INPUT");
          if ( check_outbuf && m.spacOUT   < SPACE_MIN ) ::strcat(txt," OUTPUT");
          if ( m.evtIN == h.evtIN || (check_outbuf && m.evtOUT == h.evtOUT) )
            setAlarm(alarms->second,node,ERR_NODE_STUCK,when, txt);
          else
            setAlarm(alarms->second,node,ERR_SPACE_LIMIT,when, txt);
        }
      }
      else if ( m.evtIN > 0 && m.evtIN <= h.evtIN && m_sum.evtIN  == m_sumHist.evtIN ) // No activity IN buffer
        setAlarm(alarms->second,node,ERR_NOMONITORING_ACTIVITY,when);
    }
  }
  IocSensor::instance().send(m_parent,CMD_CHECK,alarms.release());
}

