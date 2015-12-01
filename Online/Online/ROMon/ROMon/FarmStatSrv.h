// $Id: FarmStatSrv.h,v 1.2 2010-10-06 21:55:00 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/FarmStatSrv.h,v 1.2 2010-10-06 21:55:00 frankb Exp $
#ifndef ROMON_FARMSTATSRV_H
#define ROMON_FARMSTATSRV_H 1

// Framework includes
#include "CPP/Interactor.h"

// C++ include files
#include <map>
#include <ctime>
#include <vector>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  // Forward declarations
  class SubfarmStatCollector;
  class FarmStatSrv;
  class CPUfarm;
  class Nodeset;

  /**@class SubfarmStatCollector ROMon.h GaudiOnline/FarmStatSrv.h
   *
   *   Monitor base class handling standard actions.
   *
   *   @author M.Frank
   */
  class SubfarmStatCollector : public CPP::Interactor   {
  protected:
    FarmStatSrv*      m_parent;
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
    FarmStatSrv* parent() const         { return m_parent;       }

    /// Access subfarm name
    const std::string& name() const     { return m_name;         }

    /// Access to last update time
    time_t lastUpdate() const           { return m_lastUpdate;   }

    virtual void check(time_t now);
  public:
    /// Initializing constructor
    SubfarmStatCollector(FarmStatSrv* parent, const std::string& title);

    /// Standard destructor
    virtual ~SubfarmStatCollector();

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
    virtual void handle(const CPP::Event& ev);

    /// DIM command service callback
    static void feedString(void* tag, void** address, int* size, int* first);
  };

  /**@class FarmStatSrv ROMon.h GaudiOnline/FarmStatSrv.h
   *
   *   Monitoring monitor for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class FarmStatSrv : public CPP::Interactor  {
  protected:
    typedef std::map<std::string, SubfarmStatCollector*> SubfarmMonitors;
    typedef std::vector<std::string>  Farms;

    int                               m_dns;
    lib_rtl_lock_t                    m_lock;
    bool                              m_haveMBM;
    bool                              m_haveCPU;

    std::string                       m_name;
    std::string                       m_match;
    SubfarmMonitors                   m_farmMonitors;
    Farms                             m_farms;

  public:
    /// Standard constructor
    FarmStatSrv(int argc, char** argv);

    /// Standard destructor
    virtual ~FarmStatSrv();

    /// Access subfarm name
    const std::string& name() const     { return m_name;     }

    /// Access to setup property: haveMBM
    bool haveMBM() const                { return m_haveMBM;  }

    /// Access to setup property: haveMBM
    bool haveCPU() const                { return m_haveCPU;  }

    /// Interactor overload: Monitor callback handler
    virtual void handle(const CPP::Event& ev);

    /// Connect to data sources
    void connect(const std::vector<std::string>& farms);

    /// DIM command service callback
    virtual void updateDns(const void* data);

    /// DIM command service callback
    static void dnsDataHandler(void* tag, void* address, int* size);
  };
}      // End namespace ROMon
#endif /* ROMON_FARMSTATSRV_H */
