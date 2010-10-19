// $Id: CtrlSubfarmDisplay.h,v 1.4 2010-10-19 15:36:26 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/CtrlSubfarmDisplay.h,v 1.4 2010-10-19 15:36:26 frankb Exp $
#ifndef ROMON_CTRLSUBFARMDISPLAY_H
#define ROMON_CTRLSUBFARMDISPLAY_H 1

// Framework includes
#include "ROMon/ClusterDisplay.h"
#include "ROMon/TaskSupervisor.h"

// C++ include files
#include <set>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class CtrlSubfarmDisplay ROMon.h GaudiOnline/CtrlSubfarmDisplay.h
   *
   *   Monitoring display for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class CtrlSubfarmDisplay : public ClusterDisplay  {
  protected:
    /// Typedef for exclded nodes container
    typedef std::set<std::string> StrSet;

    /// reference to the node display
    MonitorDisplay*      m_nodes;

    /// Extracted cluster information for all contained nodes
    Cluster              m_cluster;

    /// Variable size data buffer
    Descriptor           m_clusterData;

    /// Excluded nodes
    StrSet               m_excluded;

    /// Initialize window
    void init(int argc, char** arv);

  public:
    /// Initializing constructor
    CtrlSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv);

    /// Standard constructor
    CtrlSubfarmDisplay(int argc, char** argv);

    /// Standard destructor
    virtual ~CtrlSubfarmDisplay();

    /// Number of nodes in the dataset
    size_t numNodes()                   { return m_cluster.nodes.size(); }

    /// Retrieve cluster name from cluster display
    virtual std::string clusterName() const;

    /// Retrieve node name from cluster display by offset
    virtual std::string nodeName(size_t offset);

    /// Access to data buffer
    virtual Descriptor& data()             {    return m_clusterData; }

    /// Access to data buffer
    virtual const Descriptor& data() const {    return m_clusterData; }

    /// Allow client access to cluster data
    const Cluster& cluster() const      { return m_cluster; }

    /// Access Node display
    MonitorDisplay* nodeDisplay() const { return m_nodes; }

    /// Show the display header information (title, time, ...)
    void showHeader();

    /// Display the node information
    void showNodes();

    /// Update all displays
    virtual void update();

    /// DIM command service callback
    static void excludedHandler(void* tag, void* address, int* size);
  };
  /// Static abstract object creator.
  ClusterDisplay*  createCtrlSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv);
}      // End namespace ROMon
#endif /* ROMON_CTRLSUBFARMDISPLAY_H */

