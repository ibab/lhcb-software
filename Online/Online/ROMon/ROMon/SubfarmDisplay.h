// $Id: SubfarmDisplay.h,v 1.6 2009-01-09 10:30:18 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/SubfarmDisplay.h,v 1.6 2009-01-09 10:30:18 frankb Exp $
#ifndef ROMON_SUBFARMDISPLAY_H
#define ROMON_SUBFARMDISPLAY_H 1

// Framework includes
#include "ROMon/ClusterDisplay.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class SubfarmDisplay ROMon.h GaudiOnline/SubfarmDisplay.h
   *
   *   Monitoring display for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class SubfarmDisplay : public ClusterDisplay  {
  protected:

    /// reference to the node display
    MonitorDisplay* m_nodes;

    /// Reference to the display showing the MOORE tasks
    MonitorDisplay* m_moores;

    /// Reference to the display showing the event builder tasks
    MonitorDisplay* m_builders;

    /// Reference to the display showing the event holder tasks
    MonitorDisplay* m_holders;

    /// Reference to the display showing the event sender tasks
    MonitorDisplay* m_senders;

    /// Initialize window
    void init(int argc, char** arv);
  public:
    /// Initializing constructor
    SubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv);

    /// Standard constructor
    SubfarmDisplay(int argc, char** argv);

    /// Standard destructor
    virtual ~SubfarmDisplay();

    /// Number of nodes in the dataset
    virtual size_t numNodes();

    /// Retrieve cluster name from cluster display
    virtual std::string clusterName() const;

    /// Retrieve node name from cluster display by offset
    virtual std::string nodeName(size_t offset);

    /// Access Node display
    virtual MonitorDisplay* nodeDisplay() const {    return m_nodes; }

    /// Show the display header information (title, time, ...)
    void showHeader(const Nodeset& ns);

    /// Display the node information
    void showNodes(const Nodeset& ns);

    /// Show the event builder information
    void showTasks(const Nodeset& ns);

    /// Update all displays
    virtual void updateDisplay(const Nodeset& ns);
  };
}      // End namespace ROMon
#endif /* ROMON_SUBFARMDISPLAY_H */

