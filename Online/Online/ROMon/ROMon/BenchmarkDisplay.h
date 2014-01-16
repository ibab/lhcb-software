// $Id: BenchmarkDisplay.h,v 1.3 2010-10-14 13:30:08 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/BenchmarkDisplay.h,v 1.3 2010-10-14 13:30:08 frankb Exp $
#ifndef ONLINE_ROMON_BENCHMARKDISPLAY_H
#define ONLINE_ROMON_BENCHMARKDISPLAY_H

// Framework includes
#include "ROMon/InternalDisplay.h"

// C++ include files
#include <ctime>
#include <cmath>
#include <map>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  // Forward declarations
  class BootNodeStatusset;
  class FarmBenchStats;
  class Nodeset;

  /**@class BenchmarkDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display showing all processes on a given node.
   *
   *   @author M.Frank
   */
  class BenchmarkDisplay : public InternalDisplay {
  public:
    enum { NODE_STATS=1, SUBFARM_STATS=2, FARM_STATS=3 };

  private:
    ///Flag containing execution mode
    int             m_mode;
    /// Time stamp of last farm display update
    time_t          m_last;
    /// Node name
    std::string     m_name;
    /// Partition name
    std::string     m_partition;
    /// Pointer with data to benchmark structure
    FarmBenchStats* m_stat;

  private:
    /// Private copy constructor
    BenchmarkDisplay(const BenchmarkDisplay&) : InternalDisplay() {}
    /// Private assignment operator
    BenchmarkDisplay& operator=(const BenchmarkDisplay&) { return *this; }
  public:
    /// Initializing constructor
    BenchmarkDisplay(InternalDisplay* parent, int mode, const std::string& node, int height=62,int width=132);
    /// Standard destructor
    virtual ~BenchmarkDisplay();
    /// Access display by cluster name
    const std::string& name() const { return m_name; }
    /// Explicit connect to data services
    void connect();
    /// Update node display content
    int updateNode(const Nodeset& ns);
    /// Update subfarm display content
    int updateSubfarm(const Nodeset& ns);
    /// Update farm display content
    int updateFarm(const Nodeset& ns);
    /// Update statistics
    void updateStats(const Nodeset& ns);
    /// Update display content
    virtual void update(const void* data);
    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }
  };
  /// External creator function
  InternalDisplay* createBenchmarkDisplay(InternalDisplay* parent, int mode, const std::string& title);
}      // End namespace ROMon
#endif /* ONLINE_ROMON_BENCHMARKDISPLAY_H */
