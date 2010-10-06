// $Id: MBMPublish.h,v 1.1 2010-10-06 11:31:23 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/MBMPublish.h,v 1.1 2010-10-06 11:31:23 frankb Exp $
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

class Interactor;

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  class MBMPublish;
  class ProcFarm;
  class CPUfarm;
  class Nodeset;

  /**@class MBMInfoSource ROMon.h GaudiOnline/MBMPublish.h
   *
   *   Monitor base class handling standard actions.
   *
   *   @author M.Frank
   */
  class MBMInfoSource : public Interactor   {
  protected:
    MBMPublish*       m_parent;
    std::string       m_name;
    std::string       m_mbmData;
    std::string       m_cpuData;
    int               m_mbm, m_mbmSvc;
    int               m_cpu, m_cpuSvc;
    time_t            m_lastUpdate;
    bool              m_mbmChanged;
    bool              m_cpuChanged;
  public:
    /// Access to parent to send interrupts
    MBMPublish* parent() const          { return m_parent;       }

    /// Access subfarm name
    const std::string& name() const     { return m_name;         }

    /// Access to last update time
    time_t lastUpdate() const           { return m_lastUpdate;   }

    virtual void check(time_t now);
  public:
    /// Initializing constructor
    MBMInfoSource(MBMPublish* parent, const std::string& title);

    /// Standard destructor
    virtual ~MBMInfoSource();

    /// Log message with tag
    std::ostream& log(const std::string& tag,const std::string& node="");

    /// Connect to publishing dim service
    virtual void connect();

    /// Disconnect from services: Only destructor may be called afterwards
    virtual void disconnect();

    /// Publish summary information
    void publish();

    /// Update monitor content
    virtual void updateCPU(const CPUfarm& cs);

    /// Update display content
    virtual void updateMBM(const Nodeset& ns);

    /// DIM command service callback
    static void mbmHandler(void* tag, void* address, int* size);

    /// DIM command service callback
    static void cpuHandler(void* tag, void* address, int* size);

    /// Interactor overload: Monitor callback handler
    virtual void handle(const Event& ev);

    /// DIM command service callback
    static void feedString(void* tag, void** address, int* size, int* first);
  };

  /**@class MBMPublish ROMon.h GaudiOnline/MBMPublish.h
   *
   *   Monitoring monitor for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class MBMPublish : public MBMInfoSource  {
  protected:
    friend class MBMInfoSource;

    enum { HLT_MODE, RECO_MODE, CTRL_MODE };
    typedef std::map<std::string, MBMInfoSource*> MBMMonitors;
    typedef std::vector<std::string>  Farms;

    std::string                       m_match;
    std::string*                      m_current;
    int                               m_time;
    int                               m_mode;
    int                               m_update_time_max;
    MBMMonitors                       m_farmMonitors;
    Farms                             m_farms;

    int                               m_runState;
    long                              m_farmEvents;

public:
    /// Standard constructor
    MBMPublish(int argc, char** argv);

    /// Standard destructor
    virtual ~MBMPublish();

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
    
    /// Get farm monitor from cursor position
    MBMInfoSource* currentMonitor();

    /// Accessor to sub-monitors of main panel
    MBMMonitors& subMonitors() {  return m_farmMonitors; }

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

    /// DIM command service callback
    static void feedSummary(void* tag, void** address, int* size, int* first);

    /// DIM command service callback
    static void commandHandler(void* tag, void* address, int* size);

  };
}      // End namespace ROMon
#endif /* ROMON_FARMMONITOR_H */
