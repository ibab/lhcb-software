// $Id: BenchmarkSubfarmDisplay.h,v 1.3 2010-10-14 13:30:08 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Base class for displays for readout monitoring 
//               in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/BenchmarkSubfarmDisplay.h,v 1.3 2010-10-14 13:30:08 frankb Exp $
#ifndef ONLINE_ROMON_BENCHMARKSUBFARMDISPLAY_H
#define ONLINE_ROMON_BENCHMARKSUBFARMDISPLAY_H

// Framework includes
#include "ROMon/InternalDisplay.h"

// C++ include files
#include <map>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  // Forward declarations
  class BootNodeStatusset;

  /**@class BenchmarkSubfarmDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display showing all processes on a given node.
   *
   *   @author M.Frank
   */
  class BenchmarkSubfarmDisplay : public InternalDisplay {
    /// Node name
    std::string  m_name;

    struct ClientStat {
      double    timesqr;
      double    time;
      long long nevt;
      long long first;
      unsigned int tm[2];

      ClientStat() : timesqr(0), time(0), nevt(0), first(0) {}
      ClientStat(const ClientStat& c) : timesqr(c.timesqr), time(c.time), nevt(c.nevt), first(c.first) {}
      ClientStat& operator=(const ClientStat& c) {
	timesqr=c.timesqr; time=c.time; nevt=c.nevt; first=c.first;
	return *this;
      }
    };

    typedef std::map<std::string, ClientStat>        ClientBufferStats;
    typedef std::map<std::string, ClientBufferStats> NodeClientStats;
    typedef std::map<std::string, NodeClientStats>   SubfarmStats;
    typedef std::map<std::string, SubfarmStats>      FarmStats;

    FarmStats   m_stat;
    std::string m_partition;
  public:
    /// Initializing constructor
    BenchmarkSubfarmDisplay(InternalDisplay* parent, const std::string& node, int height=55,int width=132);
    /// Standard destructor
    virtual ~BenchmarkSubfarmDisplay();
    /// Access display by cluster name
    const std::string& name() const { return m_name; }
    /// Explicit connect to data services
    void connect();
    /// Update display content
    virtual void update(const void* data);
    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }
  };
  /// External creator function
  InternalDisplay* createBenchmarkSubfarmDisplay(InternalDisplay* parent, const std::string& title);
  /// External creator function
  InternalDisplay* createBenchmarkFarmDisplay(InternalDisplay* parent, const std::string& title);
  /// External creator function
  InternalDisplay* createBenchmarkNodeDisplay(InternalDisplay* parent, const std::string& title);
}      // End namespace ROMon
#endif /* ONLINE_ROMON_BENCHMARKSUBFARMDISPLAY_H */
