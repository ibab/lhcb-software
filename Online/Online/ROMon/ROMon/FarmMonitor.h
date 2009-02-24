// $Id: FarmMonitor.h,v 1.2 2009-02-24 10:38:19 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Base class for monitors for readout monitoring 
//               in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/FarmMonitor.h,v 1.2 2009-02-24 10:38:19 frankb Exp $
#ifndef ROMON_FARMMONITOR_H
#define ROMON_FARMMONITOR_H 1

// Framework includes
#include "ROMon/PartitionListener.h"
#include "ROMon/Constants.h"
#include "CPP/Interactor.h"

// C++ include files
#include <map>
#include <ctime>
#include <vector>

class Interactor;

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  class FarmMonitor;
  class ProcFarm;
  class CPUfarm;
  class Nodeset;
  class AlarmTag;
  class Alarm;

  enum AlarmLevel {
    ERR_LVL_ALARM = 1<<31,
    ERR_LVL_ERROR = 1<<30,
    ERR_LVL_WARNING = 1<<29,
    ERR_LVL_MONITOR = 1<<28
  };

  enum AlarmCodes {
    ERR_NO_ERROR               = ERR_LVL_MONITOR,
    ERR_NO_UPDATES             = 1   +ERR_LVL_ALARM,
    ERR_NOT_USED               = 2   +ERR_LVL_MONITOR,
    ERR_SLOTS_LIMIT            = 3   +ERR_LVL_WARNING,
    ERR_SPACE_LIMIT            = 4   +ERR_LVL_WARNING,
    ERR_MEPRX_MISSING          = 5   +ERR_LVL_ALARM,
    ERR_MEPRX_STUCK            = 6   +ERR_LVL_ERROR, 
    ERR_RECEIVER_MISSING       = 7   +ERR_LVL_ALARM,
    ERR_RECEIVER_STUCK         = 8   +ERR_LVL_WARNING,
    ERR_SENDER_MISSING         = 9   +ERR_LVL_ALARM,
    ERR_SENDER_STUCK           = 10  +ERR_LVL_ERROR,
    ERR_MOORE_MISSING          = 11  +ERR_LVL_ALARM,
    ERR_MOORE_STUCK            = 12  +ERR_LVL_WARNING,
    ERR_MONITOR_MISSING        = 13  +ERR_LVL_ERROR,
    ERR_MONITOR_STUCK          = 14  +ERR_LVL_WARNING,
    ERR_NODAQ_ACTIVITY         = 15  +ERR_LVL_WARNING,
    ERR_NOHLT_ACTIVITY         = 16  +ERR_LVL_WARNING,
    ERR_NOSTORAGE_ACTIVITY     = 17  +ERR_LVL_WARNING,
    ERR_NOMONITORING_ACTIVITY  = 18  +ERR_LVL_MONITOR,
    ERR_NODE_STUCK             = 19  +ERR_LVL_ALARM,
    ERR_LAST_ERROR             = 20  +ERR_LVL_MONITOR,
    ERR_REMOVED                = 21  +ERR_LVL_MONITOR,
    ERR_REMOVEDALL             = 22  +ERR_LVL_MONITOR
  };

  class AlarmTag {
  public:
    int code;
    std::string subfarm, node;

    AlarmTag(int c, const std::string& s, const std::string& n) : code(c), subfarm(s), node(n) {    }
    AlarmTag(const AlarmTag& a) : code(a.code), subfarm(a.subfarm), node(a.node) {   }
    virtual ~AlarmTag() {}
    AlarmTag& operator=(const AlarmTag& a) {
      if ( &a != this ) {
	code = a.code;
	node = a.node;
	subfarm = a.subfarm;
      }
      return *this;
    }
  };

  class Alarm : public AlarmTag  {
  public:
    time_t when;
    std::string description;
    mutable std::string tag, optional;
    Alarm(const std::string& txt);
    Alarm(int c, time_t w, const std::string& s, const std::string& n, const std::string& dsc="",const std::string& opt="") 
      : AlarmTag(c,s,n), when(w), description(dsc), optional(opt) {    }
    Alarm(const Alarm& a) 
      : AlarmTag(a), when(a.when), description(a.description), tag(a.tag), optional(a.optional) {
    }
    virtual ~Alarm() {}
    std::string toString(const std::string& t) const;
    void fromString(const std::string& s);
    int level() const;
    int color() const;
    std::string time() const;
    const char* message() const;
    
    Alarm& operator=(const Alarm& a) {
      if ( &a != this ) {
	this->AlarmTag::operator=(a);
	when = a.when;
	description = a.description;
	optional = a.optional;
      }
      return *this;
    }
  };

  typedef std::vector<Alarm> Alarms;
  typedef std::pair<std::string, Alarms> AlarmInfo;
  typedef std::map<char,int>             StateSummary;


  /**@class InternalMonitor ROMon.h GaudiOnline/FarmMonitor.h
   *
   *   Monitor base class handling standard actions.
   *
   *   @author M.Frank
   */
  class InternalMonitor : public Interactor   {
  protected:
    FarmMonitor*      m_parent;
    std::string       m_name;
    std::string       m_title;
    int               m_svc;
    time_t            m_lastUpdate;
    StateSummary      m_stateSummary;
    int               m_snapshotDiff;
    int               m_numUpdate;
    long long int     m_numEvent;
    long long int     m_lastNumEvt;

    int               m_numTask;
    bool              m_hasProblems;
    unsigned int      m_steer;

  public:
    enum { COUNT_TASKS=1, COUNT_EVENTS=2 };

    /// Access to problem flag
    bool hasProblems() const            { return m_hasProblems;  }

    /// Access to parent to send interrupts
    FarmMonitor* parent() const         { return m_parent;       }

    /// Access subfarm name
    const std::string& name() const     { return m_name;         }

    /// Access title string
    const std::string& title() const    { return m_title;        }

    /// Access to last update time
    time_t lastUpdate() const           { return m_lastUpdate;   }

    /// Access to the state summary     
    StateSummary& stateSummary()        { return m_stateSummary; }

    /// Access to event counter
    int taskCount() const               {  return m_numTask; }

    /// Access to event counter
    long long int evtCount() const      {  return m_numEvent; }


    /// Set a new alarm
    void setAlarm(Alarms& alms, const std::string& node, int type, time_t when, const std::string& dsc="", const std::string& opt="");

  public:
    /// Initializing constructor
    InternalMonitor(FarmMonitor* parent, const std::string& title);

    /// Standard destructor
    virtual ~InternalMonitor();

    /// Log message with tag
    std::ostream& log(const std::string& tag,const std::string& node="");

    /// Check if data snapshor should be analyzed
    bool useSnapshot()  const;

    /// Disconnect from services: Only destructor may be called afterwards
    void disconnect();

    /// Set timeout error
    virtual void setTimeoutError();

    /// Extract data for monitoring
    virtual void extractData(const Nodeset& ns) = 0;

    /// Analyse monitored data
    virtual void analyzeData() = 0;

    /// Check monitor for errors
    virtual void check(time_t now);

    /// Update monitor content
    virtual void update(const void* data);

    /// Update monitor content
    virtual void update(const void* data, size_t len);

    /// Update display content
    virtual void updateContent(const Nodeset& ns);

    /// Count the number of tasks
    virtual void countTasks(const Nodeset& ns);

    /// Count the number of events in all buffers
    virtual void countEvents(const Nodeset& ns);

    /// Add state summary
    virtual void addStateSummary(StateSummary& sum);

    /// DIM command service callback
    static void dataHandler(void* tag, void* address, int* size);

    /// Interactor overload: Monitor callback handler
    virtual void handle(const Event& ev);
  };

  /**@class FarmMonitor ROMon.h GaudiOnline/FarmMonitor.h
   *
   *   Monitoring monitor for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class FarmMonitor : public InternalMonitor  {
  protected:
    enum { HLT_MODE, RECO_MODE, CTRL_MODE };
    typedef std::map<std::string, InternalMonitor*> SubMonitors;
    typedef std::vector<std::string>  Farms;
    typedef std::vector<Alarm*>       TypeAlarms;
    typedef std::map<int,TypeAlarms>  AlarmsByType;

    std::string                       m_match;
    std::string                       m_current;
    int                               m_mode;
    int                               m_serviceID;

    std::auto_ptr<PartitionListener>  m_listener;
    SubMonitors                       m_farmMonitors;
    Farms                             m_farms;
    AlarmsByType                      m_alarms;
    AlarmsByType                      m_newAlarms;
    AlarmsByType                      m_clrAlarms;
    int                               m_runState;
    long                              m_farmEvents;

    /// Update alarms from a subfarm
    void updateAlarms(const std::string& subfarm, Alarms& alarms);

public:
    /// Standard constructor
    FarmMonitor(int argc, char** argv);
    /// Standard destructor
    virtual ~FarmMonitor();
    /// Access partition name
    const std::string& partition() const { return m_name; }
    /// Reset for new initialization
    virtual void reset() {}

    /// Extract data for monitoring
    virtual void extractData(const Nodeset& ) {}
    /// Analyse monitored data
    virtual void analyzeData() {}

    
    /// Allow clients to check if the system is running
    bool isRunning() const;

    /// Publish alarm
    virtual void publish(const std::string& tag, const Alarm& alm);

    /// Show subfarm monitor
    int showSubfarm();
    /// Get farm monitor from cursor position
    InternalMonitor* currentMonitor();
    /// Accessor to sub-monitors of main panel
    SubMonitors& subMonitors() {  return m_farmMonitors; }
    /// Interactor overload: Monitor callback handler
    virtual void handle(const Event& ev);
    /// Connect to data sources
    void connect(const std::vector<std::string>& farms);
    /// DIM command service callback
    virtual void update(const void* data);
    /// DIM command service callback
    static void dnsDataHandler(void* tag, void* address, int* size);
    /// DIM command service callback
    static void feedData(void* tag, void** address, int* size, int* first);
  };
  // Max. 15 seconds without update allowed
  static int UPDATE_TIME_MAX = 15;
}      // End namespace ROMon

#include <iostream>
std::ostream& operator<<(std::ostream& os, const ROMon::Alarm& a);

#endif /* ROMON_FARMMONITOR_H */
