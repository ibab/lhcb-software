// $Id: FarmStatClusterDisplay.h,v 1.2 2010-10-12 17:47:05 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/FarmStatClusterDisplay.h,v 1.2 2010-10-12 17:47:05 frankb Exp $
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
  class FarmStatClusterLine : public Interactor  {
  public:
    typedef std::map<std::string,std::map<std::string,std::vector<float> > > _CI;
    typedef std::map<std::string,std::map<std::string,std::vector<int> > >   _MI;
  protected:
    int                m_mbm, m_cpu;
    size_t             m_position;
    FarmStatDisplay*   m_parent;
    std::string        m_name;
    _CI                m_cpuData;
    _MI                m_mbmData;

  public:
    FarmStatClusterLine(FarmStatDisplay* p, int pos, const std::string& n);
    virtual ~FarmStatClusterLine();
    const std::string& name() const           { return m_name;      }
    /// Access to CPU data information
    const _CI& cpuData() const                { return m_cpuData;   }
    /// Access to MBM data information
    const _MI& mbmData() const                { return m_mbmData;   }
    /// The line position in the display
    int position() const                      { return m_position;  }
    /// Access to parent
    FarmStatDisplay* parent() const           {  return m_parent;   }
    /// Start DIM services for this subfarm line
    int start();
    /// Interactor overload: Display callback handler
    virtual void handle(const Event& ev);
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
    std::string  m_name;
  public:
    /// Initializing constructor
    FarmStatClusterDisplay(InternalDisplay* parent, const std::string& node, int height=60,int width=172);
    /// Standard destructor
    virtual ~FarmStatClusterDisplay();
    /// Access display by cluster name
    const std::string& name() const { return m_name; }
    /// Update display content
    virtual void update(const void* data);
    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }
  };

}      // End namespace ROMon
#endif /* ONLINE_ROMON_FARMSTATCLUSTERDISPLAY_H */
