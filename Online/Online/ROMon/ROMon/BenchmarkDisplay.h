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
  struct Nodeset;

  /**@class BenchmarkDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display showing all processes on a given node.
   *
   *   @author M.Frank
   */
  class BenchmarkDisplay : public InternalDisplay {
  public:
    enum { NODE_STATS=1, SUBFARM_STATS=2, FARM_STATS=3 };
    struct ClientStat {
      double    timesqr;
      double    time;
      long long nevt;
      long long last;
      unsigned int tm[2];
      
      ClientStat()
	: timesqr(0), time(0), nevt(0), last(0)
      { tm[0]=tm[1]=0;}
      ClientStat(const ClientStat& c) 
	: timesqr(c.timesqr), time(c.time), nevt(c.nevt), last(c.last)
      {tm[0]=c.tm[0];tm[1]=c.tm[1];}

      ClientStat& operator=(const ClientStat& c) {
	timesqr = c.timesqr; 
	time    = c.time; 
	nevt    = c.nevt; 
	last    = c.last;
	tm[0]   = c.tm[0]; 
	tm[1]   = c.tm[1];
	return *this;
      }
      long long entries() const { return nevt;                        }
      double mean()       const { return time/double(nevt);           }
      double mean2()      const { return timesqr/double(nevt);        }
      double sigma()      const { return sqrt(mean2()-mean()*mean()); }
    };

    typedef std::map<std::string, ClientStat>        BufferStats;
    typedef std::map<std::string, BufferStats>       NodeStats;
    typedef std::map<std::string, NodeStats>         SubfarmStats;
    typedef std::map<std::string, SubfarmStats>      FarmStats;

  private:
    int          m_mode;
    /// Time stamp of last farm display update
    time_t       m_last;
    /// Node name
    std::string  m_name;
    std::string  m_partition;
    
    FarmStats    m_stat;

    void makeNodeStatLine(char* txt, const char* nnam, const NodeStats& ns) const;

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
