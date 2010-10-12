// $Id: BootClusterDisplay.h,v 1.2 2010-10-12 17:47:04 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/BootClusterDisplay.h,v 1.2 2010-10-12 17:47:04 frankb Exp $
#ifndef ONLINE_ROMON_BOOTCLUSTERDISPLAY_H
#define ONLINE_ROMON_BOOTCLUSTERDISPLAY_H

// Framework includes
#include "ROMon/InternalDisplay.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  // Forward declarations
  class BootNodeStatusset;

  /**@class BootClusterDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display showing all processes on a given node.
   *
   *   @author M.Frank
   */
  class BootClusterDisplay : public InternalDisplay {
    /// Node name
    std::string  m_name;
  public:
    /// Initializing constructor
    BootClusterDisplay(InternalDisplay* parent, const std::string& node, int height=55,int width=132);
    /// Standard destructor
    virtual ~BootClusterDisplay();
    /// Access display by cluster name
    const std::string& name() const { return m_name; }
    /// Update display content
    virtual void update(const void* data);
    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }
  };

}      // End namespace ROMon
#endif /* ONLINE_ROMON_BOOTCLUSTERDISPLAY_H */
