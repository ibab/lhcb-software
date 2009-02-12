// $Id: FarmMonitor.h,v 1.1 2009-02-12 13:09:52 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/FarmMonitor.h,v 1.1 2009-02-12 13:09:52 frankb Exp $
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
  class AlarmTag;
  class Alarm;

  class AlarmTag {
  public:
    int code;
    std::string subfarm, node;

    AlarmTag(int c, const std::string& s, const std::string& n) : code(c), subfarm(s), node(n) {    }
    AlarmTag(const AlarmTag& a) : code(a.code), subfarm(a.subfarm), node(a.node) {   }
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

    Alarm(int c, time_t w, const std::string& s, const std::string& n, const std::string& dsc) 
      : AlarmTag(c,s,n), when(w), description(dsc) {    }
    Alarm(const Alarm& a) 
      : AlarmTag(a), when(a.when), description(a.description) {
    }
    Alarm& operator=(const Alarm& a) {
      if ( &a != this ) {
	this->AlarmTag::operator=(a);
	when = a.when;
	description = a.description;
      }
      return *this;
    }
  };

  typedef std::vector<Alarm> Alarms;


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
  public:

    /// Access to parent to send interrupts
    FarmMonitor* parent() const         { return m_parent;     }
    /// Access subfarm name
    const std::string& name() const     { return m_name;       }
    /// Access title string
    const std::string& title() const    { return m_title;      }
    /// Access to last update time
    time_t lastUpdate() const           { return m_lastUpdate; }

  public:
    /// Initializing constructor
    InternalMonitor(FarmMonitor* parent, const std::string& title);
    /// Standard destructor
    virtual ~InternalMonitor();

    /// Log message with tag
    std::ostream& log(const std::string& tag,const std::string& node="");

    /// Disconnect from services: Only destructor may be called afterwards
    void disconnect();
    /// Update monitor content
    virtual void update(const void* data) = 0;
    /// Update monitor content
    virtual void update(const void* data, size_t len);
    /// Check monitor for errors
    virtual void check(time_t /* stamp */) {}
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
    SubMonitors                       m_farmMonitors;
    std::auto_ptr<PartitionListener>  m_listener;
    std::string                       m_partition;
    std::string                       m_match;
    /// vector with all farm monitors
    Farms                             m_farms;
    int                               m_mode;
    typedef std::vector<Alarm*>       TypeAlarms;
    typedef std::map<int,TypeAlarms>  AlarmsByType;
    AlarmsByType                      m_alarms;
    AlarmsByType                      m_newAlarms;
    AlarmsByType                      m_clrAlarms;


    /// Update alarms from a subfarm
    void updateAlarms(const std::string& subfarm, Alarms& alarms);

public:
    /// Standard constructor
    FarmMonitor(int argc, char** argv);
    /// Standard destructor
    virtual ~FarmMonitor();
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
  };
}      // End namespace ROMon
#endif /* ROMON_FARMMONITOR_H */
