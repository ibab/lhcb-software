// $Id: RecSubfarmDisplay.h,v 1.6 2009-04-17 13:16:37 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/RecSubfarmDisplay.h,v 1.6 2009-04-17 13:16:37 frankb Exp $
#ifndef ROMON_RECSUBFARMDISPLAY_H
#define ROMON_RECSUBFARMDISPLAY_H 1

// Framework includes
#include "ROMon/ClusterDisplay.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class RecSubfarmDisplay ROMon.h GaudiOnline/RecSubfarmDisplay.h
   *
   *   Monitoring display for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class RecSubfarmDisplay : public ClusterDisplay  {
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
    /// Initializing constructor
    RecSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv);

    /// Standard constructor
    RecSubfarmDisplay(int argc, char** argv);

    /// Standard destructor
    virtual ~RecSubfarmDisplay();

    /// Number of nodes in the dataset
    size_t numNodes();

    /// Retrieve cluster name from cluster display
    std::string clusterName() const;

    /// Retrieve node name from cluster display by offset
    virtual std::string nodeName(size_t offset);

    /// Access Node display
    MonitorDisplay* nodeDisplay() const {    return m_nodes; }

    /// Show the display header information (title, time, ...)
    void showHeader(const Nodeset& ns);

    /// Display the node information
    void showNodes(const Nodeset& ns);

    /// Show the event builder information
    void showTasks(const Nodeset& ns);

    /// Update all displays
    virtual void updateDisplay(const Nodeset& ns);
  };
  /// Static abstract object creator.
  ClusterDisplay*  createRecSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv);
}      // End namespace ROMon
#endif /* ROMON_RECSUBFARMDISPLAY_H */

