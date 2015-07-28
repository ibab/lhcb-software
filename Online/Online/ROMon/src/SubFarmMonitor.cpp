#ifndef ROMON_SUBFARMMONITOR_H
#define ROMON_SUBFARMMONITOR_H

// Framework include files
#include "ROMon/FarmMonitor.h"
// C++ include files
#include <map>

namespace ROMon {
  class Nodeset;
  class FSMTask;

  /** @class SubfarmMonitor SubfarmMonitor.cpp
   *
   */
  class SubfarmMonitor : public InternalMonitor {

    struct NodeMon {
      struct Task {
        int  input, output, pid;
        char state;
        Task() : input(0), output(0), pid(-1), state(TASK_FSM_STATE_DEAD) {}
        Task(const Task& t) : input(t.input), output(t.output), pid(t.pid), state(t.state) {}
        Task& operator=(const Task& t) {
          if ( this == &t ) return *this;
          input = t.input; output=t.output; pid=t.pid; state = t.state;
          return *this; 
        }
      };
      typedef std::map<std::string,Task> Clients;
      time_t       update;
      char         inUse;
      int          numBuffs;
      int          numClients;
      int          minEVENT,   maxEVENT;
      int          evtMEP,     evtEVENT,  evtSEND;
      float        spacMEP,    spacEVENT, spacSEND;
      float        slotMEP,    slotEVENT, slotSEND;
      Clients      moores;
      Task         mepRx, sender;
      StateSummary stateSum;
      NodeMon() {   reset(); }
      NodeMon(const NodeMon& c) { *this = c; }
      NodeMon& operator=(const NodeMon& c);
      void reset();
    };

    typedef std::map<std::string,NodeMon*>        Monitors;
    typedef std::pair<std::string,const FSMTask*> TaskDesc;
    typedef std::map<std::string,TaskDesc>        TaskMap;

    Monitors   m_nodes;
    Monitors   m_history;
    NodeMon    m_sum;
    NodeMon    m_sumHist;
    TaskMap    m_allTasks;

  public:
    /// Initializing constructor
    SubfarmMonitor(FarmMonitor* parent, const std::string& title);

    /// Standard destructor
    virtual ~SubfarmMonitor();

    /// Extract data for monitoring
    virtual void extractData(const Nodeset& ns);

    /// Analyse monitored data
    virtual void analyzeData();

    virtual bool useSnapshot() const {
      time_t now = time(0);
      return (now - m_lastUpdate) >= 10;//m_snapshotDiff;
    }
  };
  InternalMonitor* createSubfarmMonitor(FarmMonitor* parent, const std::string& title);
}
#endif // ROMON_SUBFARMMONITOR_H

#include "RTL/strdef.h"
#define MBM_IMPLEMENTATION
#include "ROMon/ROMon.h"
#include "ROMonDefs.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
extern "C" {
#include "dic.h"
}
#include <limits>

using namespace ROMon;
using namespace std;

namespace {
  const char* _procNam(const char* nam) {
    const char* p;
    if (0 != ::strstr(nam,"MEPRx") ) return nam;
    p = ::strchr(nam,'_');
    if ( 0 != p ) return ++p;
    return "Unknown";
  }
}

InternalMonitor* ROMon::createSubfarmMonitor(FarmMonitor* parent, const string& title) {
  InternalMonitor* m = new SubfarmMonitor(parent,title);
  return m;
}

SubfarmMonitor::NodeMon& SubfarmMonitor::NodeMon::operator=(const NodeMon& c) {
  if ( this != &c ) {
    numBuffs     = c.numBuffs;
    numClients   = c.numClients;
    minEVENT     = c.minEVENT;
    maxEVENT     = c.maxEVENT;
    evtMEP       = c.evtMEP;
    evtEVENT     = c.evtEVENT;
    evtSEND      = c.evtSEND;
    spacMEP      = c.spacMEP;
    spacEVENT    = c.spacEVENT;
    spacSEND     = c.spacSEND;
    slotMEP      = c.slotMEP;
    slotEVENT    = c.slotEVENT;
    slotSEND     = c.slotSEND;
    update       = c.update;
    inUse        = c.inUse;
    moores       = c.moores;
    mepRx        = c.mepRx;
    sender       = c.sender;
    stateSum     = c.stateSum;
  }
  return *this;
}

void SubfarmMonitor::NodeMon::reset() {
  numBuffs = 0;
  numClients = 0;
  minEVENT = maxEVENT  = 0;
  evtMEP   = evtEVENT  = evtSEND  = 0;
  sender.input = sender.output = -1;
  sender.state = 'N';
  mepRx    = sender;
  spacMEP  = spacEVENT = spacSEND = 0;
  slotMEP  = slotEVENT = slotSEND = 0;
  update   = 0;
  inUse    = 0;
  moores.clear();
  stateSum.clear();
}

/// Initializing constructor
SubfarmMonitor::SubfarmMonitor(FarmMonitor* parent, const string& title) 
  : InternalMonitor(parent,title)
{
  string svc = svcPrefix()+strlower(title)+"/ROpublish";
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
  m_title = svc;
  cout << "Connecting to subfarm " << title << " with service " << m_title << endl;
}

/// Standard destructor
SubfarmMonitor::~SubfarmMonitor() {
  for(Monitors::iterator i=m_nodes.begin(); i!=m_nodes.end();++i)
    delete (*i).second;
  for(Monitors::iterator j=m_history.begin(); j!=m_history.end();++j)
    delete (*j).second;
}

void SubfarmMonitor::extractData(const Nodeset& ns) {
  NodeMon sum;
  string subfarm = ns.name;
  TaskMap::iterator tm;
  Nodeset::Nodes::const_iterator n;
  bool    running  = parent()->isRunning();
  map<int,const FSMTask*> task_map;

  m_lastUpdate = ns.firstUpdate().first;
  sum.minEVENT  = numeric_limits<int>::max();
  sum.maxEVENT  = numeric_limits<int>::min();
  sum.spacMEP   = numeric_limits<float>::max();
  sum.spacEVENT = numeric_limits<float>::max();
  sum.spacSEND  = numeric_limits<float>::max();
  sum.slotMEP   = numeric_limits<float>::max();
  sum.slotEVENT = numeric_limits<float>::max();
  sum.slotSEND  = numeric_limits<float>::max();
  m_stateSummary.clear();

  for(tm=m_allTasks.begin(); tm!=m_allTasks.end(); ++tm)
    (*tm).second.second = 0;

  for (n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    const string node = (*n).name;
    const Node::Tasks&  tasks = *(*n).tasks();
    if ( m_history.find(node) == m_history.end() )
      m_history[node] = new NodeMon();
    if ( m_nodes.find(node) == m_nodes.end() )
      m_nodes[node] = new NodeMon();
    else
      *(m_history[node]) = *(m_nodes[node]);
    *(m_history[node]) = *(m_nodes[node]);
    NodeMon* m = m_nodes[node];
    m->reset();
    // This should ensure that we take note of all existing tasks participating in datataking:
    for(Node::Tasks::const_iterator it=tasks.begin(); it!=tasks.end(); it=tasks.next(it)) {
      TaskDesc& dsc = m_allTasks[(*it).name];
      dsc.first                    = node;
      dsc.second                   = it;
      task_map[(*it).processID]    = it;
      m->stateSum[(*it).state]    += 1;
      m_stateSummary[(*it).state] += 1;
    }
    m->sender.state = m->mepRx.state = char(running ? TASK_FSM_STATE_DEAD : TASK_FSM_STATE_NOT_READY);
  }

  for(TaskMap::const_iterator im=m_allTasks.begin(); im!=m_allTasks.end();++im) {
    const char* p = _procNam((*im).first.c_str());
    if ( *p == MOORE_TASK )  {
      const string& node = (*im).second.first;
      m_nodes[node]->moores[p].state = char(running ? TASK_FSM_STATE_DEAD : TASK_FSM_STATE_NOT_READY);
    }
  }

  for (n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    const string node = (*n).name;
    NodeMon& m = *m_nodes[node];
    const Node::Tasks&  tasks = *(*n).tasks();
    for(Node::Tasks::const_iterator it=tasks.begin(); it!=tasks.end(); it=tasks.next(it)) {
      const char* p = (*it).name;
      const char* q = strchr(p,'_')+1;
      const char  c = ::toupper(p[0])=='H' ? *q : 'Z';
      //cout << (*it).name << " " << strchr(p,'_')+1 << " " << c << endl;
      if ( c == MOORE_TASK )  {
        NodeMon::Task& t = m.moores[q];
        t.input = t.output = 0;
        t.state = TASK_FSM_STATE_NOT_READY;
      }
      else if ( c == SENDER_TASK ) {
        m.sender.input = m.sender.output = 0;
        m.sender.state = TASK_FSM_STATE_NOT_READY;
      }
      else if ( c == BUILDER_TASK ) {
        m.mepRx.input = m.mepRx.output = 0;
        m.mepRx.state = TASK_FSM_STATE_NOT_READY;
      }
    }
  }

  for (n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    const Node::Buffers& buffs = *(*n).buffers();
    MBMBuffer::Clients::const_iterator ic;
    Node::Buffers::const_iterator ib;
    const string node = (*n).name;
    NodeMon& m = *m_nodes[node];

    m.update = (*n).time;
    for(ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      char b = (*ib).name[0];
      const MBMBuffer::Control& ctrl = (*ib).ctrl;
      const MBMBuffer::Clients& clients = (*ib).clients;

      ++m.numBuffs;
      switch(b) {
      case MEP_BUFFER:
        m.inUse   = 1;
        m.evtMEP  = ctrl.tot_produced;
        m.spacMEP = float(ctrl.i_space)/float(ctrl.bm_size);
        m.slotMEP = float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax);
        break;
      case EVT_BUFFER:
        m.inUse     = 1;
        m.evtEVENT  = ctrl.tot_produced;
        m.spacEVENT = float(ctrl.i_space)/float(ctrl.bm_size);
        m.slotEVENT = float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax);
        break;
      case RES_BUFFER:
      case SND_BUFFER:
        m.inUse    = 1;
        m.evtSEND  = ctrl.tot_produced;
        m.spacSEND = float(ctrl.i_space)/float(ctrl.bm_size);
        m.slotSEND = float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax);
        break;
      default:
        continue;
      }
      m.minEVENT = numeric_limits<int>::max();
      m.maxEVENT = numeric_limits<int>::min();
      for (ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
        ++m.numClients;
        int nevt = (*ic).events;
        int pid  = (*ic).processID;
        const char* nc = (*ic).name;
        const char* p  = _procNam(nc);
        map<int,const FSMTask*>::const_iterator t1 = task_map.find(pid);
        char state = char(t1==task_map.end() ? int(TASK_FSM_STATE_DEAD) : (*t1).second->state);
        switch(*p) {
        case BUILDER_TASK:
          m.mepRx.input = m.mepRx.output = nevt;
          m.mepRx.state = state;
          m.mepRx.pid = pid;
          break;
        case SENDER_TASK:
          if( b == RES_BUFFER || b == SND_BUFFER )  {
            m.sender.input = m.sender.output = nevt;
            m.sender.state = state;
            m.sender.pid = pid;
          }
          break;
        case MOORE_TASK: {
          //  Normal  and        TAE event processing
          NodeMon::Task* t = &m.moores[p];
          t->pid = pid;
          t->state = state;
          if( b == EVT_BUFFER || b == MEP_BUFFER )  {
            t->input = nevt;
            m.maxEVENT = max(m.maxEVENT,nevt);
            m.minEVENT = min(m.minEVENT,nevt);
          }
          else {
            t->output = nevt;
          }
        }
          break;
        default:
          break;
        }
      }
    }
    sum.numBuffs   += m.numBuffs;
    sum.numClients += m.numClients;
    sum.evtMEP     += m.evtMEP;
    sum.evtEVENT   += m.evtEVENT;
    sum.evtSEND    += m.evtSEND;
    sum.minEVENT    = min(m.minEVENT,sum.minEVENT);
    sum.maxEVENT    = max(m.maxEVENT,sum.maxEVENT);
    sum.spacMEP     = min(m.spacMEP,sum.spacMEP);
    sum.spacEVENT   = min(m.spacEVENT,sum.spacEVENT);
    sum.spacSEND    = min(m.spacSEND,sum.spacSEND);
    sum.slotMEP     = min(m.slotMEP,sum.slotMEP);
    sum.slotEVENT   = min(m.slotEVENT,sum.slotEVENT);
    sum.slotSEND    = min(m.slotSEND,sum.slotSEND);
  }
  // Tasks which are no longer present need to be marked as DEAD (D)
  for(tm=m_allTasks.begin(); tm!=m_allTasks.end(); ++tm) {
    if ( (*tm).second.second == 0 )    {
      m_stateSummary[TASK_FSM_STATE_DEAD] += 1;
    }
  }
  m_sumHist    = m_sum;
  m_sum        = sum;
}

void SubfarmMonitor::analyzeData() {
  char txt[128];
  time_t now = time(0);
  bool    running  = parent()->isRunning();
  auto_ptr<AlarmInfo> alarms(new AlarmInfo(m_name,Alarms()));
  cout << name() << "Running:" << running << " ";

  m_hasProblems = false;
  for(Monitors::const_iterator i=m_nodes.begin(); i!=m_nodes.end();++i) {
    const string& node = (*i).first;
    const NodeMon& m = *(*i).second;
    const NodeMon& h = *m_history[(*i).first];
    time_t      when = m.update;
    txt[0] = 0;
    cout << node << " ";
    if ( now-m.update > m_snapshotDiff )
      setAlarm(alarms->second,node,ERR_NO_UPDATES,when,node);
    else if ( !m.inUse )
      setAlarm(alarms->second,node,ERR_NOT_USED,when,node);
    else   {
      NodeMon::Clients::const_iterator ih;
      if ( m.mepRx.output < 0 || m.mepRx.state == TASK_FSM_STATE_DEAD )                 // MEPRX DEAD/not present
        setAlarm(alarms->second,node,ERR_MEPRX_MISSING,when);
      else if ( running && m.mepRx.output == 0 && m_sum.evtMEP >  m_sumHist.evtMEP )    // MEPRX stuck while running
        setAlarm(alarms->second,node,ERR_MEPRX_STUCK,when);
      else if ( running && m.evtMEP   <= h.evtMEP && m_sum.evtMEP >  m_sumHist.evtMEP ) // MEPRX stuck while running
        setAlarm(alarms->second,node,ERR_MEPRX_STUCK,when);
      
      if ( m.sender.input < 0 || m.sender.state == TASK_FSM_STATE_DEAD )                   // SENDER DEAD/not present
        setAlarm(alarms->second,node,ERR_SENDER_MISSING,when);
      else if ( running && m.sender.input == 0 && m_sum.evtSEND > m_sumHist.evtSEND )      // SENDER stuck while running
        setAlarm(alarms->second,node,ERR_SENDER_STUCK,when);
      else if ( running && m.evtSEND  <= h.evtSEND  && m_sum.evtSEND > m_sumHist.evtSEND ) // SENDER stuck while running
        setAlarm(alarms->second,node,ERR_SENDER_STUCK,when);

      for(NodeMon::Clients::const_iterator ic=m.moores.begin(); ic!=m.moores.end(); ++ic) {
        const string& n = (*ic).first;
        const NodeMon::Task* t = &((*ic).second);
        if ( t->state == TASK_FSM_STATE_DEAD )  {
          // cout << (void*)m << " " << "---2----" << (void*)t << " Moore:" << node << " Task:" << n << " DEAD " 
          //      << t->pid << endl;
          setAlarm(alarms->second,node,ERR_MOORE_MISSING,when,n);
        }
        else if ( running && m.mepRx.output > 0 ) {     // MEPRX is requirement for Moores!
          if ( m_sum.evtEVENT > m_sumHist.evtEVENT ) {  // Need some minimal activity....
            if ( t->input <= 1 ) continue;
            ih = h.moores.find(n);
            if ( ih == h.moores.end() ) continue;
            if ( t->input <= (*ih).second.input ) {
              ::sprintf(txt,"%d %d",t->input,(*ih).second.input);
              setAlarm(alarms->second,node,ERR_MOORE_STUCK,when,n /*,txt */);
            }
          }
        }
      }

      // Remaining alarms may only occur while running
      if ( !running ) continue;

      txt[0] = 0;
      if ( m.slotMEP < SLOTS_MIN || m.slotEVENT < SLOTS_MIN || m.slotSEND < SLOTS_MIN ) {
        if ( m.slotMEP   < SLOTS_MIN ) ::strcat(txt,"MEP ");
        if ( m.slotEVENT < SLOTS_MIN ) ::strcat(txt,"EVENT ");
        if ( m.slotSEND  < SLOTS_MIN ) ::strcat(txt,"RES/SEND ");
        if ( m.evtMEP == h.evtMEP || m.evtEVENT == h.evtEVENT || m.evtSEND == h.evtSEND )
          setAlarm(alarms->second,node,ERR_NODE_STUCK,when,"");
        else
          setAlarm(alarms->second,node,ERR_SLOTS_LIMIT,when,"");
      }
      else if ( m.spacMEP < SPACE_MIN || m.spacEVENT < SPACE_MIN || m.spacSEND < SPACE_MIN  ) {
        if ( m.spacMEP   < SPACE_MIN ) ::strcat(txt,"MEP ");
        if ( m.spacEVENT < SPACE_MIN ) ::strcat(txt,"EVENT ");
        if ( m.spacSEND  < SPACE_MIN ) ::strcat(txt,"RES/SEND ");
        if ( m.evtMEP == h.evtMEP || m.evtEVENT == h.evtEVENT || m.evtSEND == h.evtSEND )
          setAlarm(alarms->second,node,ERR_NODE_STUCK,when,"");
        else
          setAlarm(alarms->second,node,ERR_SPACE_LIMIT,when,"");
        cout << "1111 ";
      }
      else if ( m.evtMEP   <= h.evtMEP && m_sum.evtMEP == m_sumHist.evtMEP )    // No activity on any MEP buffer
        setAlarm(alarms->second,node,ERR_NODAQ_ACTIVITY,when,node);
      else if ( m.evtSEND  <= h.evtSEND  && m_sum.evtSEND == m_sumHist.evtSEND )// No activity on any send buffer
        setAlarm(alarms->second,node,ERR_NOSTORAGE_ACTIVITY,when,node);
      else if ( m.evtEVENT > 0 && m.evtEVENT <= h.evtEVENT && m_sum.evtEVENT  == m_sumHist.evtEVENT ) // No activity EVENT buffer
        setAlarm(alarms->second,node,ERR_NOHLT_ACTIVITY,when,node);
      else if ( m.evtEVENT == 0 && m.evtEVENT == h.evtEVENT && m_sumHist.maxEVENT > 1000 ) // No activity EVENT buffer
        setAlarm(alarms->second,node,ERR_NOHLT_ACTIVITY,when,node);
    }
  }
  cout << endl;
  IocSensor::instance().send(m_parent,CMD_CHECK,alarms.release());
}

