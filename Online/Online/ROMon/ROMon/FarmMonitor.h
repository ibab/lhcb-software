// $Id: FarmMonitor.h,v 1.9 2010-10-14 06:44:04 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/FarmMonitor.h,v 1.9 2010-10-14 06:44:04 frankb Exp $
#ifndef ROMON_FARMMONITOR_H
#define ROMON_FARMMONITOR_H 1

// Framework includes
#include "ROMon/PartitionListener.h"
#include "ROMon/Constants.h"
#include "ROMon/Alarm.h"
#include "CPP/Interactor.h"

// C++ include files
#include <map>
#include <ctime>
#include <vector>
#include <memory>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  class FarmMonitor;
  class ProcFarm;
  class CPUfarm;
  class Nodeset;

  /**@class InternalMonitor ROMon.h GaudiOnline/FarmMonitor.h
   *
   *   Monitor base class handling standard actions.
   *
   *   @author M.Frank
   */
  class InternalMonitor : public CPP::Interactor   {
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
    void setAlarm(Alarms& alms, const std::string& node, int type, time_t when, const std::string& dsc="");

  public:
    /// Initializing constructor
    InternalMonitor(FarmMonitor* parent, const std::string& title);

    /// Standard destructor
    virtual ~InternalMonitor();

    /// Return service prefix for usage of the bridge
    static const std::string& svcPrefix();

    /// Return service prefix for usage of the bridge
    static void setSvcPrefix(const std::string& pref);
    
    /// Log message with tag
    std::ostream& log(const std::string& tag,const std::string& node="");

    /// Check if data snapshor should be analyzed
    virtual bool useSnapshot()  const;

    /// Disconnect from services: Only destructor may be called afterwards
    virtual void disconnect();

    /// Set timeout error
    virtual void setTimeoutError();

    /// Extract data for monitoring
    virtual void extractData(const Nodeset& /* ns */) {}

    /// Analyse monitored data
    virtual void analyzeData() {}

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
    virtual void handle(const CPP::Event& ev);
  };

  /**@class FarmMonitor ROMon.h GaudiOnline/FarmMonitor.h
   *
   *   Monitoring monitor for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class FarmMonitor : public InternalMonitor  {
  protected:
    friend class InternalMonitor;

    enum { HLT_MODE, RECO_MODE, CTRL_MODE };
    typedef std::map<std::string, InternalMonitor*> SubMonitors;
    typedef std::vector<std::string>  Farms;
    typedef std::vector<Alarm*>       TypeAlarms;
    typedef std::map<int,TypeAlarms>  AlarmsByType; 
    typedef std::map<std::string,std::pair<size_t,AlarmsByType> > AlarmsBySource;
    typedef std::map<int,Alarm*>      AlarmMap;

    std::string                       m_match;
    std::string*                      m_current;
    Alarm*                            m_currAlarm;
    int                               m_time;
    int                               m_mode;
    int                               m_serviceID;
    int                               m_summaryID;
    int                               m_hartBeatID;
    int                               m_commandID;
    int                               m_update_time_max;

    std::auto_ptr<PartitionListener>  m_listener;
    SubMonitors                       m_farmMonitors;
    Farms                             m_farms;

    AlarmsByType                      m_activeAlarms;
    AlarmsByType                      m_allAlarms;

    AlarmMap                          m_sumAlarms;
    int                               m_runState;
    long                              m_farmEvents;

    /// Update alarms from a subfarm
    void removeAlarmsBySubfarm(const std::string& sf_name);
    /// Update alarms from a subfarm
    void updateAlarms(const std::string& subfarm, Alarms& alarms);

    void publishAlarms();
    void publishRegularAlarms(AlarmsByType& alarms);
    void publishTypeAlarms(const AlarmsByType& alarms);
    void publishSourceAlarms(const AlarmsBySource& alarms);

    void getTypeAlarms(AlarmsByType& alms, const AlarmsByType& alarms);
    void getSourceAlarms(AlarmsByType& alms, const AlarmsBySource& alarms);

  public:
    /// Standard constructor
    FarmMonitor(int argc, char** argv);

    /// Standard destructor
    virtual ~FarmMonitor();

    /// Access max update time difference
    int updateTimeMax() const { return m_update_time_max; }

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
    virtual void handle(const CPP::Event& ev);

    /// Connect to data sources
    void connect(const std::vector<std::string>& farms);

    /// DIM command service callback
    virtual void update(const void* data);

    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalMonitor::update(data,len); }

    /// DIM command service callback
    static void dnsDataHandler(void* tag, void* address, int* size);

    /// DIM command service callback
    static void feedData(void* tag, void** address, int* size, int* first);

    /// DIM command service callback
    static void feedSummary(void* tag, void** address, int* size, int* first);

    /// DIM command service callback
    static void commandHandler(void* tag, void* address, int* size);

  };
}      // End namespace ROMon

#include <iostream>
std::ostream& operator<<(std::ostream& os, const ROMon::Alarm& a);

#endif /* ROMON_FARMMONITOR_H */
