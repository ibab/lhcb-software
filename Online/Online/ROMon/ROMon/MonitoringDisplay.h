// $Id: MonitoringDisplay.h,v 1.1 2008-02-08 21:20:35 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/MonitoringDisplay.h,v 1.1 2008-02-08 21:20:35 frankm Exp $
#ifndef ROMON_STORAGEDISPLAY_H
#define ROMON_STORAGEDISPLAY_H 1

// Framework includes
#include "ROMon/ROMonDisplay.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class MonitoringDisplay ROMon.h GaudiOnline/MonitoringDisplay.h
   *
   *   Monitoring display for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class MonitoringDisplay : public ROMonDisplay  {
  protected:

    /// Reference to the Relay info display
    MonitorDisplay* m_relay;

    /// reference to the node display
    MonitorDisplay* m_nodes;

    /// Reference to the display showing monitoring tasks
    MonitorDisplay* m_tasks;

    /// Partition name for projection(s)
    std::string     m_partName;

    /// Name pof relay node
    std::string     m_relayNode;

  public:
    /// Standard constructor
    MonitoringDisplay(int argc, char** argv);

    /// Standard destructor
    virtual ~MonitoringDisplay();

    void showHeader(const Nodeset& ns);

    /// Display the node information
    void showNodes(const Nodeset& ns);

    /// Show the task information
    void showTasks(const Nodeset& ns);

    /// Show the relay information
    void showRelay(const Nodeset& ns);

    /// Update all displays
    virtual void updateDisplay(const Nodeset& ns);
  };
}      // End namespace ROMon
#endif /* ROMON_STORAGEDISPLAY_H */

