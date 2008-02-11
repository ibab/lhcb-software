// $Id: SubfarmDisplay.h,v 1.1 2008-02-11 20:00:43 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/SubfarmDisplay.h,v 1.1 2008-02-11 20:00:43 frankm Exp $
#ifndef ROMON_SUBFARMDISPLAY_H
#define ROMON_SUBFARMDISPLAY_H 1

// Framework includes
#include "ROMon/ROMonDisplay.h"

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
  class SubfarmDisplay : public ROMonDisplay  {
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

  public:
    /// Standard constructor
    SubfarmDisplay(int argc, char** argv);

    /// Standard destructor
    virtual ~SubfarmDisplay();

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

