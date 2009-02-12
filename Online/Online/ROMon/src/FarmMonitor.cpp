// $Id: FarmMonitor.cpp,v 1.1 2009-02-12 13:09:52 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/FarmMonitor.cpp,v 1.1 2009-02-12 13:09:52 frankb Exp $


#define MBM_IMPLEMENTATION
#include "ROMon/ROMon.h"
#include "ROMon/ROMon.h"
#include "ROMon/FarmMonitor.h"
#include "ROMon/CPUMon.h"
#include "CPP/TimeSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"
#include "RTL/Lock.h"
#include "RTL/strdef.h"
#include "WT/wtdef.h"
extern "C" {
#include "dic.h"
}

// C++ include files
#include <set>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <fstream>
#include <iostream>
#include <algorithm>

#ifdef _WIN32
#define strcasecmp  _stricmp
#define strncasecmp _strnicmp
#define vsnprintf   _vsnprintf
#endif
using namespace ROMon;
using namespace std;

// Max. 15 seconds without update allowed
#define UPDATE_TIME_MAX 40

typedef vector<string> StringV;

static lib_rtl_lock_t    s_lock;
static const int   INT_max = numeric_limits<int>::max();
static const float FLT_max = numeric_limits<float>::max();


static void help() {
  cout << "  romon_farm -option [-option]" << endl
       << "       -all                         Show all subfarms." << endl
       << "       -p[artition]=<name>          Partition name providing monitoring information." << endl
       << endl;
}
namespace {
  const char* _procNam(const char* nam) {
    char* p;
    if (0 != ::strstr(nam,"MEPRx") ) return nam;
    p = ::strchr(nam,'_');
    if ( 0 != p ) return ++p;
    return "Unknown";
  }
}

namespace ROMon {
  class Alarm;
  typedef std::pair<std::string, Alarms> AlarmInfo;

  struct NullAlarm  {
    bool operator()(const Alarm* a) const { return a == 0; } 
  };
  struct DeleteAlarm   {
    void operator()(std::pair<const int,std::vector<Alarm*> >& a)  const
    {  for_each(a.second.begin(),a.second.end(),DeleteAlarm());    }
    bool operator()(Alarm* a) const       { if ( a ) delete a; return true; } 
  };
  struct FindAlarm {
    const Alarm&  m_a;
    FindAlarm(const Alarm& a) : m_a(a) {}
    void operator()(const std::pair<const int,std::vector<Alarm*> >& a)  const
    {  for_each(a.second.begin(),a.second.end(),FindAlarm(m_a));    }
    bool operator()(const Alarm& a) const {  return this->operator()(&a);   }
    bool operator()(const Alarm* a) const  
    {  return 0 != a && a->code == m_a.code && a->node == m_a.node;      }
  };
  struct PrintAlarm {
    const std::string& tag;
    PrintAlarm(const std::string& t) : tag(t) {}
    void operator()(std::pair<const int,std::vector<Alarm*> >& a)  const
    {  for_each(a.second.begin(),a.second.end(),PrintAlarm(tag));    }
    bool operator()(const Alarm* a) const {
      char txt[32];
      ::strftime(txt,sizeof(txt),"%H:%M:%S",::localtime(&a->when));
      cout << left << setw(8) << tag << setw(12) << left << a->node << ":" << left << setw(12) << txt << a->description << endl;
      return true;
    }
  };
  InternalMonitor* createFarmSubMonitor(FarmMonitor* parent, const string& title);
  InternalMonitor* createRecFarmSubMonitor(FarmMonitor* parent, const string& title);
  InternalMonitor* createCtrlFarmSubMonitor(FarmMonitor* parent, const string& title);
  InternalMonitor* createMonitoringSubMonitor(FarmMonitor* parent, const std::string& title);
  InternalMonitor* createStorageSubMonitor(FarmMonitor* parent, const std::string& title);
}
#include <set>
namespace ROMon {

  enum AlarmCodes {
    ERR_NO_ERROR=0,
    ERR_NO_UPDATES=1,
    ERR_NOT_USED,
    ERR_SLOTS_LIMIT,
    ERR_SPACE_LIMIT,
    ERR_MEPRX_MISSING,
    ERR_MEPRX_STUCK, 
    ERR_SENDER_MISSING,
    ERR_SENDER_STUCK,
    ERR_MOORE_MISSING,
    ERR_MOORE_STUCK,
    ERR_NODAQ_ACTIVITY,
    ERR_NOHLT_ACTIVITY,
    ERR_NOSTORAGE_ACTIVITY,
    ERR_NODE_STUCK,
    ERR_LAST_ERROR,
    ERR_REMOVED
  };

  class FarmSubMonitor : public InternalMonitor {
    int               m_numUpdate;
    bool              m_hasProblems;
    bool              m_inUse;

    struct BuffMon {
      int produced;
      int clients;
      BuffMon() : produced(0), clients(0)
      {}
    };
    struct NodeMon {
      typedef std::map<std::string,std::pair<int,int> > Clients;
      int    error;
      int    numBuffs;
      int    numClients;
      int    minEVENT, maxEVENT;
      int    evtMEP,   evtEVENT,  evtSEND;
      int    mepRxEvt, senderEvt;
      float  spacMEP,  spacEVENT, spacSEND;
      float  slotMEP,  slotEVENT, slotSEND;
      time_t update;
      char   inUse;
      Clients moores;
      NodeMon() {   reset(); }
      NodeMon(const NodeMon& c) { *this = c; }
      NodeMon& operator=(const NodeMon& c);
      void reset();
    };

    typedef std::map<std::string,NodeMon*>  Monitors;
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
    FarmSubMonitor(FarmMonitor* parent, const string& title);
    /// Standard destructor
    virtual ~FarmSubMonitor();
    /// Check display for errors
    void check(time_t now);
    /// Set timeout error
    void setTimeoutError();

    /// Update monitor content
    virtual void update(const void* data);
    /// Update display content
    virtual void updateContent(const Nodeset& ns);
  };


  InternalMonitor* _conn(FarmMonitor* p, const string& title) {
    cout << "Connecting to subfarm " << title << endl;
    InternalMonitor* m = new FarmSubMonitor(p,title);
    return m;
  }
  InternalMonitor* createFarmSubMonitor(FarmMonitor* m, const string& title) { return _conn(m,title);}
  InternalMonitor* createRecFarmSubMonitor(FarmMonitor* m, const string& title) { return _conn(m,title);}
  InternalMonitor* createCtrlFarmSubMonitor(FarmMonitor* m, const string& title) { return _conn(m,title);}
  InternalMonitor* createMonitoringSubMonitor(FarmMonitor* m, const string& title) { return _conn(m,title);}
  InternalMonitor* createStorageSubMonitor(FarmMonitor* m, const string& title) { return _conn(m,title);}

  struct CheckMonitor {
    time_t now;
    CheckMonitor(time_t n) : now(n) {}
    void operator()(const std::pair<std::string, InternalMonitor*>& mon) { mon.second->check(now); }
  };

#ifndef OutputLogger_H
#define OutputLogger_H

#include <string>
#include <iostream>

  /** class OutputLogger

  Small helper class to catch output of a task

  @author M.Frank
  @version 1.0

  */
  class OutputLogger : public std::streambuf     { 
  public: 
    /// Standard Constructor
    OutputLogger (); 
    /// Standard destructor
    ~OutputLogger (); 
    /// Callback to fill stream buffer
    int overflow (int c); 
    /// Callback indicating EOF
    int underflow (); 

  private:  
    /// Save old stream buffer
    std::streambuf* _old; 
    /// String currently beeing assembled
    std::string _buf; 
  }; 
#endif

}



namespace {
  /// Extract node/service name from DNS info
  void getServiceNode(char* s, string& svc, string& node) {
    char* at = strchr(s,'@');
    *at = 0;
    svc = s;
    node = at+1;
  }
}


/// Standard Constructor
OutputLogger::OutputLogger()
{
  _buf = ""; 
  ::printf("Redirecting cout to OutputLogger....\n");
  _old = cout.rdbuf(this);
} 
 
/// Standard Destructor
OutputLogger::~OutputLogger ()    { 
  cout.rdbuf(_old); 
} 

/// Callback when filling the stream buffer 
int OutputLogger::overflow (int c)    {
  if (c == '\n')     {
    _buf += c;
    switch(::toupper(_buf[0])) {
    case 'D':
    case 'V':
    case 'I':
    case 'N':
    case 'W':
    case 'E':
    case 'F':
    default:
      ::printf(_buf.c_str());
    break;
    }
    _buf="";
  } 
  else     { 
    _buf += c; 
  } 
  return (c); 
} 

/// Callback on EOF
int OutputLogger::underflow ()  { 
  return (EOF); 
} 


FarmSubMonitor::NodeMon& FarmSubMonitor::NodeMon::operator=(const NodeMon& c) {
  if ( this != &c ) {
    error      = c.error;
    numBuffs   = c.numBuffs;
    numClients = c.numClients;
    minEVENT   = c.minEVENT;
    maxEVENT   = c.maxEVENT;
    evtMEP     = c.evtMEP;
    evtEVENT   = c.evtEVENT;
    evtSEND    = c.evtSEND;
    spacMEP    = c.spacMEP;
    spacEVENT  = c.spacEVENT;
    spacSEND   = c.spacSEND;
    slotMEP    = c.slotMEP;
    slotEVENT  = c.slotEVENT;
    slotSEND   = c.slotSEND;
    update     = c.update;
    inUse      = c.inUse;
    moores     = c.moores;
    mepRxEvt   = c.mepRxEvt;
    senderEvt  = c.senderEvt;
  }
  return *this;
}

void FarmSubMonitor::NodeMon::reset() {
  error = 0;
  numBuffs = 0;
  numClients = 0;
  minEVENT = maxEVENT  = 0;
  evtMEP   = evtEVENT  = evtSEND  = 0;
  mepRxEvt = senderEvt = -1;
  spacMEP  = spacEVENT = spacSEND = 0;
  slotMEP  = slotEVENT = slotSEND = 0;
  update   = 0;
  inUse    = 0;
  moores.clear();
}

/// Initializing constructor
FarmSubMonitor::FarmSubMonitor(FarmMonitor* parent, const string& title) 
  : InternalMonitor(parent,title) 
{
  m_numUpdate = 0;
  m_lastUpdate = time(0);
  string svc = "/";
  for(size_t i=0; i<title.length();++i) svc += ::tolower(title[i]);
  svc += "/ROpublish";
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
  m_hasProblems = false;
}

/// Standard destructor
FarmSubMonitor::~FarmSubMonitor() {
  for(Monitors::iterator i=m_nodes.begin(); i!=m_nodes.end();++i)
    delete (*i).second;
  for(Monitors::iterator j=m_history.begin(); j!=m_history.end();++j)
    delete (*j).second;
}

/// Update monitor content
void FarmSubMonitor::update(const void* address) {
  if ( address ) {
    const Nodeset* ns = (const Nodeset*)address;
    if ( ns->type == Nodeset::TYPE ) {
      updateContent(*ns);
    }
  }
}

/// Check display for errors
void FarmSubMonitor::check(time_t now) {
  if ( hasProblems() ) {
    if ( now - lastUpdate() > 2*UPDATE_TIME_MAX ) {
      setTimeoutError();
    }
  }
}

/// Set timeout error
void FarmSubMonitor::setTimeoutError() {
  log("ERROR") << "No update information available" << endl;
}

/// Update display content
void FarmSubMonitor::updateContent(const Nodeset& ns) {
  time_t now = time(0);

  std::string snam=std::string(ns.name).substr(0,3);
  if (snam == "sto" || snam == "cal" || snam == "mon") {
    m_lastUpdate = now;
    m_hasProblems = false;
    //log("WARNING") << "Skip " << ns.name << endl;
    return;
  }

  if ( (now - m_lastUpdate) < UPDATE_TIME_MAX ) return;


  //log("WARNING") << "Checking " << ns.name << "  " << m_name << endl;
  extractData(ns);
  analyzeData();
}

void FarmSubMonitor::extractData(const Nodeset& ns) {
  m_lastUpdate = ns.firstUpdate().first;
  NodeMon sum;
  sum.minEVENT    = numeric_limits<int>::max();
  sum.maxEVENT    = numeric_limits<int>::min();
  sum.spacMEP     = numeric_limits<float>::max();
  sum.spacEVENT   = numeric_limits<float>::max();
  sum.spacSEND    = numeric_limits<float>::max();
  sum.slotMEP     = numeric_limits<float>::max();
  sum.slotEVENT   = numeric_limits<float>::max();
  sum.slotSEND    = numeric_limits<float>::max();

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

      ++m.numBuffs;
      switch(b) {
      case MEP_BUFFER:
	m.inUse = 1;
	m.evtMEP  = ctrl.tot_produced;
	m.spacMEP = float(ctrl.i_space)/float(ctrl.bm_size);
	m.slotMEP = float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax);
	break;
      case EVT_BUFFER:
	m.inUse = 1;
	m.evtEVENT  = ctrl.tot_produced;
	m.spacEVENT = float(ctrl.i_space)/float(ctrl.bm_size);
	m.slotEVENT = float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax);
	break;
      case RES_BUFFER:
      case SND_BUFFER:
	m.inUse = 1;
	m.evtSEND  = ctrl.tot_produced;
	m.spacSEND = float(ctrl.i_space)/float(ctrl.bm_size);
	m.slotSEND = float(ctrl.p_emax-ctrl.i_events)/float(ctrl.p_emax);
	break;
      default:
	continue;
      }
      m.minEVENT = numeric_limits<int>::max();
      m.maxEVENT = numeric_limits<int>::min();
      for (MBMBuffer::Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
        ++m.numClients;
        const char* p = _procNam((*ic).name);
        switch(*p) {
        case BUILDER_TASK:
          if( b == MEP_BUFFER ) m.mepRxEvt = (*ic).events;
          break;
        case SENDER_TASK:
          if( b == RES_BUFFER || b == SND_BUFFER ) m.senderEvt = (*ic).events;
          break;
        case MOORE_TASK:
          //  Normal  and        TAE event processing
          if( b == EVT_BUFFER || b == MEP_BUFFER )  {
	    m.moores[(*ic).name].first = (*ic).events;
            m.maxEVENT = max(m.maxEVENT,(*ic).events);
            m.minEVENT = min(m.minEVENT,(*ic).events);
          }
	  else {
	    m.moores[(*ic).name].second = (*ic).events;
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

  m_sumHist    = m_sum;
  m_sum        = sum;
}

void FarmSubMonitor::analyzeData() {
  char txt[128];
  time_t now = time(0);
  std::auto_ptr<AlarmInfo> alarms(new AlarmInfo(m_name,Alarms()));

  m_hasProblems = false;
  for(Monitors::iterator i=m_nodes.begin(); i!=m_nodes.end();++i) {
    const string& node = (*i).first;
    NodeMon& m = *((*i).second);
    NodeMon& h = *(m_history[(*i).first]);
    time_t   when = m.update;
    
    txt[0] = 0;
    m.error = ERR_NO_ERROR; 
    if ( now-m.update > UPDATE_TIME_MAX )
      setAlarm(alarms->second,node, ERR_NO_UPDATES, when);
    else if ( !m.inUse )
      setAlarm(alarms->second,node, ERR_NOT_USED, when);
    else   {
      NodeMon::Clients::const_iterator ih;
      if ( m.mepRxEvt < 0 )                                                     // MEPRX DEAD/not present
	setAlarm(alarms->second,node, ERR_MEPRX_MISSING, when);
      else if ( m.mepRxEvt == 0 && m_sum.evtMEP >  m_sumHist.evtMEP )           // MEPRX stuck while running
	setAlarm(alarms->second,node, ERR_MEPRX_STUCK, when);
      else if ( m.evtMEP   <= h.evtMEP && m_sum.evtMEP >  m_sumHist.evtMEP )    // MEPRX stuck while running
	setAlarm(alarms->second,node, ERR_MEPRX_STUCK, when);
      
      if ( m.senderEvt < 0 )                                                    // SENDER DEAD/not present
	setAlarm(alarms->second,node, ERR_SENDER_MISSING, when);
      else if ( m.senderEvt == 0 && m_sum.evtSEND > m_sumHist.evtSEND )         // SENDER stuck while running
	setAlarm(alarms->second,node, ERR_SENDER_STUCK, when);
      else if ( m.evtSEND  <= h.evtSEND  && m_sum.evtSEND > m_sumHist.evtSEND ) // SENDER stuck while running
	setAlarm(alarms->second,node, ERR_SENDER_STUCK, when);

      if ( m.moores.size() != 8 ) {
	for(int im=1; im<=8; ++im )  {
	  ::sprintf(txt,"%s_GauchoJob_%d",node.c_str(),im);
	  for (int in=0; in<4; ++in ) txt[in] = ::toupper(txt[in]);
	  if ( m.moores.find(txt) == m.moores.end() ) {
	    setAlarm(alarms->second,node, ERR_MOORE_MISSING, when, txt);
	  }
	}
      }
      if ( m.mepRxEvt > 0 ) {    // MEPRX is requirement for Moores!
	if ( m_sum.evtEVENT > m_sumHist.evtEVENT ) {  // Need some minimal activity....
	  for(NodeMon::Clients::const_iterator ic=m.moores.begin(); ic!=m.moores.end(); ++ic) {
	    if ( (*ic).second.first <= 1 ) continue;
	    ih = h.moores.find((*ic).first);
	    if ( ih == h.moores.end() ) continue;
	    if ( (*ic).second.first <= (*ih).second.first ) {
	      ::sprintf(txt,"%s %d %d",(*ic).first.c_str(), (*ic).second.first, (*ih).second.first);
	      setAlarm(alarms->second,node, ERR_MOORE_STUCK, when, txt);
	    }
	  }
	}
      }

      if ( m.error == ERR_NO_ERROR ) {
	txt[0] = 0;
	if ( m.slotMEP < SLOTS_MIN || m.slotEVENT < SLOTS_MIN || m.slotSEND < SLOTS_MIN ) {
	  if ( m.slotMEP   < SLOTS_MIN ) ::strcat(txt,"MEP ");
	  if ( m.slotEVENT < SLOTS_MIN ) ::strcat(txt,"EVENT ");
	  if ( m.slotSEND  < SLOTS_MIN ) ::strcat(txt,"RES/SEND ");
	  if ( m.evtMEP == h.evtMEP || m.evtEVENT == h.evtEVENT || m.evtSEND == h.evtSEND )
	    setAlarm(alarms->second,node, ERR_NODE_STUCK, when, txt);
	  else
	    setAlarm(alarms->second,node, ERR_SLOTS_LIMIT, when, txt);
	}
	else if ( m.spacMEP < SPACE_MIN || m.spacEVENT < SPACE_MIN || m.spacSEND < SPACE_MIN  ) {
	  if ( m.spacMEP   < SPACE_MIN ) ::strcat(txt,"MEP ");
	  if ( m.spacEVENT < SPACE_MIN ) ::strcat(txt,"EVENT ");
	  if ( m.spacSEND  < SPACE_MIN ) ::strcat(txt,"RES/SEND ");
	  m.error = ERR_SPACE_LIMIT;
	  if ( m.evtMEP == h.evtMEP || m.evtEVENT == h.evtEVENT || m.evtSEND == h.evtSEND )
	    setAlarm(alarms->second,node, ERR_NODE_STUCK, when, txt);
	  else
	    setAlarm(alarms->second,node, ERR_SPACE_LIMIT, when, txt);
	}
	else if ( m.evtMEP   <= h.evtMEP && m_sum.evtMEP == m_sumHist.evtMEP )    // No activity on any MEP buffer
	  setAlarm(alarms->second,node, ERR_NODAQ_ACTIVITY, when);
	else if ( m.evtSEND  <= h.evtSEND  && m_sum.evtSEND == m_sumHist.evtSEND )// No activity on any send buffer
	  setAlarm(alarms->second,node, ERR_NOSTORAGE_ACTIVITY, when);
	else if ( m.evtEVENT > 0 && m.evtEVENT <= h.evtEVENT && m_sum.evtEVENT  == m_sumHist.evtEVENT ) // No activity EVENT buffer
	  setAlarm(alarms->second,node, ERR_NOHLT_ACTIVITY, when);
      }
    }

    if ( m.error != ERR_NO_ERROR ) {
      m_hasProblems = true;
    }
  }
  IocSensor::instance().send(m_parent,CMD_CHECK,alarms.release());
}

/// Set a new alarm
void FarmSubMonitor::setAlarm(Alarms& alms, const std::string& node, int typ, time_t when, const std::string& opt) {
  std::string subfarm = node.substr(0,node.length()-2);
  switch(typ) {
  case ERR_NO_ERROR:
    alms.push_back(Alarm(typ, when, subfarm, node, "No obvious Errors detected...."));
    break;
  case ERR_NO_UPDATES:
    alms.push_back(Alarm(typ, when, subfarm, node, "No update information available"));
    break;
  case ERR_NOT_USED:
    alms.push_back(Alarm(typ, when, subfarm, node, "Node not used....."));
    break;
  case ERR_SLOTS_LIMIT: 
    alms.push_back(Alarm(typ, when, subfarm, node, "SLOTS at limit:"+opt));
    break;
  case ERR_SPACE_LIMIT:
    alms.push_back(Alarm(typ, when, subfarm, node, "SPACE at limit:"+opt));
    break;
  case ERR_NODE_STUCK:
    alms.push_back(Alarm(typ, when, subfarm, node, "Buffer at limit:"+opt));
    break;
  case ERR_MEPRX_MISSING:
    alms.push_back(Alarm(typ, when, subfarm, node, "MEPRX dead."));
    break;
  case ERR_MEPRX_STUCK:
    alms.push_back(Alarm(typ, when, subfarm, node, "MEPRx stuck."));
    break;
  case ERR_SENDER_MISSING:
    alms.push_back(Alarm(typ, when, subfarm, node, "Sender dead."));
    break;
  case ERR_SENDER_STUCK:
    alms.push_back(Alarm(typ, when, subfarm, node, "Sender stuck."));
    break;
  case ERR_MOORE_MISSING:
    alms.push_back(Alarm(typ, when, subfarm, node, "MOORE dead. "+opt));
    break;
  case ERR_MOORE_STUCK:
    alms.push_back(Alarm(typ, when, subfarm, node, "MOORE stuck. "+opt));
    break;
  case ERR_NODAQ_ACTIVITY:
    alms.push_back(Alarm(typ, when, subfarm, node, "No DAQ activity visible."));
    break;
  case ERR_NOHLT_ACTIVITY:
    alms.push_back(Alarm(typ, when, subfarm, node, "No HLT activity visible."));
    break;
  case ERR_NOSTORAGE_ACTIVITY:
    alms.push_back(Alarm(typ, when, subfarm, node, "No STORAGE activity visible."));
    break;
  }
}


InternalMonitor::InternalMonitor(FarmMonitor* parent, const string& title) 
  : m_parent(parent), m_name(title), m_svc(0)
{
  m_lastUpdate = time(0);
}

InternalMonitor::~InternalMonitor() {
  disconnect();
}

/// Log message with tag
ostream& InternalMonitor::log(const std::string& tag,const std::string& node) {
  if ( node.empty() ) {
    cout << left << setw(8) << tag << setw(12) << left << m_name << ":";
  }
  else {
    cout << left << setw(8) << tag << setw(12) << left << node << ":";
  }
  return cout;
}

void InternalMonitor::disconnect() {
  if ( m_svc != 0 ) {
    ::dic_release_service(m_svc);
    m_svc = 0;
  }
}

/// Update display content
void InternalMonitor::update(const void* data, size_t /* len */) {
  return update(data);
}

/// DIM command service callback
void InternalMonitor::dataHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    InternalMonitor* disp = *(InternalMonitor**)tag;
    unsigned char* ptr = new unsigned char[*size+sizeof(int)];
    *(int*)ptr = *size;
    ::memcpy(ptr+sizeof(int),address,*size);
    IocSensor::instance().send(disp,CMD_UPDATE,ptr);
  }
}

/// Interactor overload: Monitor callback handler
void InternalMonitor::handle(const Event& ev)    {
  switch(ev.eventtype) {
  case IocEvent: {
    switch(ev.type) {
    case CMD_UPDATE: {
      RTL::Lock lock(s_lock);
      unsigned char* ptr = (unsigned char*)ev.data;
      update(ptr + sizeof(int), *(int*)ptr);
      delete [] ptr;
      break;
    }
    default:
      break;
    }
    break;
  }
  default:
    break;
  }
}

/// Standard constructor
FarmMonitor::FarmMonitor(int argc, char** argv) : InternalMonitor(0,""), m_mode(HLT_MODE)
{
  bool all = false;
  char txt[128];
  string anchor;
  RTL::CLI cli(argc,argv,help);
  cli.getopt("partition",   2, m_name = "ALL");
  cli.getopt("match",       2, m_match = "*");
  all = 0 != cli.getopt("all",2);
  m_mode = cli.getopt("reconstruction",2) == 0 ? HLT_MODE : RECO_MODE;
  if ( cli.getopt("taskmonitor",2) != 0 ) m_mode = CTRL_MODE;

  ::lib_rtl_create_lock(0,&s_lock);
  if ( m_mode == RECO_MODE && all && m_match=="*" )
    ::sprintf(txt,"Reconstruction farm display of all known subfarms ");
  else if ( m_mode == RECO_MODE && all )
    ::sprintf(txt,"Reconstruction farm display of all known subfarms with name '%s'",m_match.c_str());
  else if ( m_mode == RECO_MODE )
    ::sprintf(txt,"Reconstruction farm display of partition %s ",m_name.c_str());
  else if ( m_mode == CTRL_MODE && all && m_match=="*" )
    ::sprintf(txt,"Task Control farm display of all known subfarms ");
  else if ( m_mode == CTRL_MODE && all )
    ::sprintf(txt,"Task Control farm display of all known subfarms with name '%s'",m_match.c_str());
  else if ( m_mode == CTRL_MODE )
    ::sprintf(txt,"Task Control farm display of partition %s ",m_name.c_str());
  else if ( m_match == "*" && all )
    ::sprintf(txt,"HLT Farm display of all known subfarms ");
  else if ( all )
    ::sprintf(txt,"HLT Farm display of all known subfarms with the name '%s'",m_match.c_str());
  else
    ::sprintf(txt,"HLT Farm display of partition %s ",m_name.c_str());
  m_title = txt;
  log("INFO") << txt << endl;
  if ( all )
    m_svc = ::dic_info_service((char*)"DIS_DNS/SERVER_LIST",MONITORED,0,0,0,dnsDataHandler,(long)this,0,0);
  else
    m_listener = auto_ptr<PartitionListener>(new PartitionListener(this,m_name));

  for(int i=ERR_NO_ERROR; i<ERR_LAST_ERROR; ++i) {
    m_alarms[i].clear();
    m_newAlarms[i].clear();
    m_clrAlarms[i].clear();
  }
}

/// Standard destructor
FarmMonitor::~FarmMonitor()  {  
  disconnect();
  m_listener = auto_ptr<PartitionListener>(0);
  subMonitors().clear();
  ::printf("Farm display deleted and resources freed......\n");
}

/// DIM command service callback
void FarmMonitor::dnsDataHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    FarmMonitor* disp = *(FarmMonitor**)tag;
    disp->update(address);
  }
}

/// Update alarms from a subfarm
void FarmMonitor::updateAlarms(const string& subfarm, Alarms& alarms) {
  int cnt_new = 0, cnt_clr = 0;
  for(Alarms::iterator i=alarms.begin(); i!=alarms.end(); ++i) {
    TypeAlarms& a = m_alarms[(*i).code];
    if ( find_if(a.begin(),a.end(),FindAlarm(*i)) == a.end() )  {
      a.push_back(new Alarm(*i));
      m_newAlarms[(*i).code].push_back(a.back());
      ++cnt_new;
    }
  }

  // Have to do the reverse as well:
  // -- If the alarm is present with this code,node,subfarm, it has to be removed
  for(AlarmsByType::iterator p=m_alarms.begin(); p!=m_alarms.end(); ++p)  {
    TypeAlarms& a = (*p).second;
    for(size_t k=0; k<a.size(); ++k) {
      Alarm& alm = *a[k];
      // Only clear alarms which origine from the same subfarm....
      if ( alm.subfarm == subfarm ) {
	if ( find_if(alarms.begin(),alarms.end(),FindAlarm(alm)) == alarms.end() ) {
	  m_clrAlarms[alm.code].push_back(&alm);
	  a.erase(a.begin()+k);
	  ++cnt_clr;
	  --k;
	}
      }
    }
    // a.erase(find_if(a.begin(),a.end(),NullAlarm()),a.end());
  }
  cout << subfarm << " All:" << alarms.size() << " New:" << cnt_new << " Clear:" << cnt_clr << endl;
}

/// DIM command service callback
void FarmMonitor::update(const void* address) {
  char c, *msg = (char*)address;
  string svc, node;
  size_t idx, idq;
  switch(c=msg[0]) {
  case '+':
    getServiceNode(++msg,svc,node);
    idx = svc.find("/ROpublish");
    idq = svc.find("/hlt");
    if ( idq == string::npos ) idq = svc.find("/mona");
    if ( idq == string::npos ) idq = svc.find("/store");
    if ( idx != string::npos && idq == 0 ) {
      string f = svc.substr(1,idx-1);
      if ( ::strcase_match_wild(f.c_str(),m_match.c_str()) ) {
	IocSensor::instance().send(this,CMD_ADD,new string(f));
      }
    }
    break;
  case '-':
    break;
  case '!':
    //getServiceNode(++msg,svc,node);
    //log() << "Service " << msg << " in ERROR." << endl;
    break;
  default:
    if ( *(int*)msg != *(int*)"DEAD" )  {
      char *at, *p = msg, *last = msg;
      auto_ptr<Farms> farms(new Farms);
      while ( last != 0 && (at=strchr(p,'@')) != 0 )  {
	last = strchr(at,'|');
	if ( last ) *last = 0;
	getServiceNode(p,svc,node);
	idx = svc.find("/ROpublish");
	idq = svc.find("/hlt");
	if ( idq == string::npos ) idq = svc.find("/mona");
	if ( idq == string::npos ) idq = svc.find("/store");
	if ( idx != string::npos && idq == 0 ) {
	  string f = svc.substr(1,idx-1);
	  if ( ::strcase_match_wild(f.c_str(),m_match.c_str()) ) {
	    farms->push_back(f);
	  }
	}
	p = last+1;
      }
      if ( !farms->empty() )
	IocSensor::instance().send(this,CMD_CONNECT,farms.release());
    }
    break;
  }
}

/// Interactor overload: Monitor callback handler
void FarmMonitor::handle(const Event& ev) {
  time_t now = time(0);
  Farms::iterator i;
  SubMonitors::iterator k;

  RTL::Lock lock(s_lock);
  switch(ev.eventtype) {
  case TimeEvent:
    return;
  case IocEvent:
    switch(ev.type) {
    case CMD_RUNSTATE:
      break;
    case CMD_SHOW:
      break;
    case CMD_UPDATE:
      /// Check for new alarms
      for_each(m_clrAlarms.begin(),m_clrAlarms.end(),PrintAlarm("CLEAR  "));
      for_each(m_newAlarms.begin(),m_newAlarms.end(),PrintAlarm("DECLARE"));
      for_each(m_clrAlarms.begin(),m_clrAlarms.end(),DeleteAlarm());
      m_clrAlarms.clear();
      m_newAlarms.clear();
      break;
    case CMD_ADD:
      i = find(m_farms.begin(),m_farms.end(),*(string*)ev.data);
      if ( i == m_farms.end() ) {
	m_farms.push_back(*(string*)ev.data);
	connect(m_farms);
      }
      delete (string*)ev.data;
      return;
    case CMD_CONNECT: {
      m_farms = *(StringV*)ev.data;
      connect(m_farms);
      delete (StringV*)ev.data;
      return;
    }
    case CMD_CHECK: {
      std::auto_ptr<AlarmInfo> alms((AlarmInfo*)ev.data);
      updateAlarms(alms->first, alms->second);
      for_each(m_farmMonitors.begin(),m_farmMonitors.end(),CheckMonitor(now));
      IocSensor::instance().send(this,int(CMD_UPDATE),this);
      break;
    }
    case CMD_DELETE:
      delete this;
      ::lib_rtl_sleep(200);
      ::exit(0);
      return;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

void FarmMonitor::connect(const vector<string>& farms) {
  SubMonitors::iterator k;
  SubMonitors copy;
  char txt[128];

  ::sprintf(txt,"Total number of subfarms:%d",int(farms.size()));
  log("INFO") << txt << endl;

  for (Farms::const_iterator i=farms.begin(); i != farms.end(); ++i) {
    k = m_farmMonitors.find(*i);
    if ( k == m_farmMonitors.end() ) {
      if ( m_mode == RECO_MODE )
	copy.insert(make_pair(*i,createRecFarmSubMonitor(this,*i)));
      else if ( m_mode == CTRL_MODE )
	copy.insert(make_pair(*i,createCtrlFarmSubMonitor(this,*i)));
      else if ( ::strncasecmp((*i).c_str(),"mona0",5)==0 )
	copy.insert(make_pair(*i,createMonitoringSubMonitor(this,*i)));
      else if ( ::strncasecmp((*i).c_str(),"storectl",8)==0 )
	copy.insert(make_pair(*i,createStorageSubMonitor(this,*i)));
      else
	copy.insert(make_pair(*i,createFarmSubMonitor(this,*i)));
    }
    else {
      copy.insert(*k);
      m_farmMonitors.erase(k);
    }
  }
  for (k=m_farmMonitors.begin(); k != m_farmMonitors.end(); ++k)
    delete (*k).second;
  m_farmMonitors = copy;
}

static size_t do_output(void*,int level,const char* fmt, va_list args) {
  char buffer[1024];
  size_t len = ::vsnprintf(buffer, sizeof(buffer), fmt, args);
  //::upic_write_message(buffer,"");
  switch(level) {
  case LIB_RTL_VERBOSE:
    cout << left << setw(8) << "VERBOSE" << setw(12) << left << "RTL" << ":" << buffer << endl;
    break;
  case LIB_RTL_DEBUG:
    cout << left << setw(8) << "DEBUG" << setw(12) << left << "RTL" << ":" << buffer << endl;
    break;
  case LIB_RTL_INFO:
    cout << left << setw(8) << "INFO" << setw(12) << left << "RTL" << ":" << buffer << endl;
    break;
  case LIB_RTL_WARNING:
    cout << left << setw(8) << "WARNING" << setw(12) << left << "RTL" << ":" << buffer << endl;
    break;
  case LIB_RTL_ERROR:
    cout << left << setw(8) << "ERROR" << setw(12) << left << "RTL" << ":" << buffer << endl;
    break;
  case LIB_RTL_FATAL:
    cout << left << setw(8) << "FATAL" << setw(12) << left << "RTL" << ":" << buffer << endl;
    break;
  case LIB_RTL_ALWAYS:
    cout << left << setw(8) << "ALWAYS" << setw(12) << left << "RTL" << ":" << buffer << endl;
    break;
  }
  return len;
}

extern "C" int romon_farmmon(int argc,char** argv) {
  OutputLogger logger;
  FarmMonitor* disp = new FarmMonitor(argc,argv);
  ::lib_rtl_install_printer(do_output,0);
  IocSensor::instance().run();
  delete disp;
  return 1;
}
