// $Id: ClusterDisplay.h,v 1.1 2008-08-27 19:15:55 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/ClusterDisplay.h,v 1.1 2008-08-27 19:15:55 frankb Exp $
#ifndef ROMON_CLUSTERDISPLAY_H
#define ROMON_CLUSTERDISPLAY_H 1

// Framework includes
#include "ROMon/ROMonDisplay.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class ClusterDisplay ROMon.h GaudiOnline/ClusterDisplay.h
   *
   *   Monitoring display for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class ClusterDisplay : public ROMonDisplay  {
  protected:

    /// reference to the node display
    MonitorDisplay* m_nodes;

    /// Reference to the display showing the MOORE tasks
    MonitorDisplay* m_reco;

    /// Reference to the display showing the event builder tasks
    MonitorDisplay* m_receivers;

    /// Reference to the display showing the event sender tasks
    MonitorDisplay* m_senders;

    /// Initialize window
    void init(int argc, char** arv);
  public:
    /// Standard constructor
    ClusterDisplay() : ROMonDisplay(){}

    /// Initializing constructor
    ClusterDisplay(int width, int height) : ROMonDisplay(width,height){}

    /// Standard destructor
    virtual ~ClusterDisplay() {}

    /// Number of nodes in the dataset
    virtual size_t numNodes() = 0;

    /// Retrieve node name from cluster display by offset
    virtual std::string nodeName(size_t offset) = 0;

    /// Access Node display
    virtual MonitorDisplay* nodeDisplay() const = 0;
  };
}      // End namespace ROMon
#endif /* ROMON_CLUSTERDISPLAY_H */

