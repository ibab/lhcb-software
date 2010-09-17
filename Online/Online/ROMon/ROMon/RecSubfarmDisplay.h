// $Id: RecSubfarmDisplay.h,v 1.7 2010-09-17 09:47:12 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/RecSubfarmDisplay.h,v 1.7 2010-09-17 09:47:12 frankb Exp $
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

    struct TaskIO {
      int in, out;
      size_t st_in, st_out;
      TaskIO() : in(0), out(0), st_in(0), st_out(0) {}
      TaskIO(const TaskIO& m) : in(m.in), out(m.out), st_in(m.st_in), st_out(m.st_out) {}
      TaskIO& operator=(const TaskIO& m) { in=m.in; out=m.out; st_in=m.st_in; st_out=m.st_out; return *this;}
    };

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

    /// Update display content
    virtual void updateDisplay(const Node& n) 
    { this->ROMonDisplay::updateDisplay(n);   }
  };
  /// Static abstract object creator.
  ClusterDisplay*  createRecSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv);
}      // End namespace ROMon
#endif /* ROMON_RECSUBFARMDISPLAY_H */

