// $Id: FarmStatClusterDisplay.h,v 1.3 2010-10-14 13:30:08 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/FarmStatClusterDisplay.h,v 1.3 2010-10-14 13:30:08 frankb Exp $
#ifndef ONLINE_ROMON_FARMSTATCLUSTERDISPLAY_H
#define ONLINE_ROMON_FARMSTATCLUSTERDISPLAY_H

// Framework includes
#include "ROMon/InternalDisplay.h"

// C++ include files
#include <map>
#include <vector>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  // Forward declarations
  class FarmStatDisplay;
  class FarmStatClusterLine;

  /**@class FarmStatClusterLine ROMon.h GaudiOnline/FarmStatDisplay.h
   *
   *   Display entry for the boot status of one subfarm
   *
   *   @author M.Frank
   */
  class FarmStatClusterLine : public CPP::Interactor  {
  public:
    typedef std::map<std::string,std::map<std::string,std::vector<float> > > _CI;
    typedef std::map<std::string,std::map<std::string,std::vector<int> > >   _MI;
  protected:
    int                m_mbm, m_cpu;
    size_t             m_position;
    CPP::Interactor*   m_parent;
    std::string        m_name;
    _CI                m_cpuData;
    _MI                m_mbmData;

  public:
    /// Initializing constructor
    FarmStatClusterLine(CPP::Interactor* p, int pos, const std::string& n);
    /// Standard destructor
    virtual ~FarmStatClusterLine();
    /// Access to cluster name
    const std::string& name() const           { return m_name;      }
    /// Access to CPU data information
    const _CI& cpuData() const                { return m_cpuData;   }
    /// Access to MBM data information
    const _MI& mbmData() const                { return m_mbmData;   }
    /// The line position in the display
    int position() const                      { return m_position;  }
    /// Access to parent
    CPP::Interactor* parent() const           {  return m_parent;   }
    /// Start DIM services for this subfarm line
    int start();
    /// Interactor overload: Display callback handler
    virtual void handle(const CPP::Event& ev);
    /// DIM command service callback
    static void mbmHandler(void* tag, void* address, int* size);
    /// DIM command service callback
    static void cpuHandler(void* tag, void* address, int* size);
  };

  /**@class FarmStatClusterDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display showing all processes on a given node.
   *
   *   @author M.Frank
   */
  class FarmStatClusterDisplay : public InternalDisplay {
    /// Node name
    std::string          m_name;
    /// Re-use cluster line as information provider
    FarmStatClusterLine* m_line;
  private:
    /// Private copy constructor
    FarmStatClusterDisplay(const FarmStatClusterDisplay&) : InternalDisplay() {}
    /// Private assignment operator
    FarmStatClusterDisplay& operator=(const FarmStatClusterDisplay&) { return *this; }
  public:
    /// Initializing constructor
    FarmStatClusterDisplay(InternalDisplay* parent, const std::string& node, int height=68,int width=172);
    /// Standard destructor
    virtual ~FarmStatClusterDisplay();
    /// Access display by cluster name
    const std::string& name() const { return m_name; }
    /// Explicit connect to data services
    virtual void connect();
    /// Interactor overload: Display callback handler
    virtual void handle(const CPP::Event& ev);
    /// Update display content
    virtual void update(const void* data);
    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }
  };

  /// External creator function
  InternalDisplay* createFarmStatsDisplay(InternalDisplay* parent, const std::string& title);

}      // End namespace ROMon
#endif /* ONLINE_ROMON_FARMSTATCLUSTERDISPLAY_H */
