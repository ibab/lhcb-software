// $Id: TorrentSubfarmDisplay.h,v 1.4 2010-10-19 15:36:26 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/TorrentSubfarmDisplay.h,v 1.4 2010-10-19 15:36:26 frankb Exp $
#ifndef ROMON_TORRENTSUBFARMDISPLAY_H
#define ROMON_TORRENTSUBFARMDISPLAY_H 1

// Framework includes
#include "ROMon/ClusterDisplay.h"
#include "ROMon/TorrentMon.h"

// C++ include files
#include <set>

/*
 *   ROMon namespace declaration
 */
namespace BitTorrent   {

  /**@class TorrentSubfarmDisplay ROMon/TorrentSubfarmDisplay.h
   *
   *   Monitoring display for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class TorrentSubfarmDisplay : public ROMon::ClusterDisplay  {
  protected:
    /// Typedef for exclded nodes container
    typedef std::set<std::string> StrSet;

    /// reference to the node display
    MonitorDisplay*      m_nodes;

    /// Variable size data buffer
    Descriptor           m_clusterData;

    /// Excluded nodes
    StrSet               m_excluded;
    StrSet               m_nodeSet;

    std::string          m_name;
    /// Initialize window
    void init(int argc, char** arv);

  public:
    /// Initializing constructor
    TorrentSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv);

    /// Standard constructor
    TorrentSubfarmDisplay(int argc, char** argv);

    /// Standard destructor
    virtual ~TorrentSubfarmDisplay();

    /// Number of nodes in the dataset
    size_t numNodes()                      { return m_nodeSet.size(); }

    /// Retrieve cluster name from cluster display
    virtual std::string clusterName() const;

    /// Retrieve node name from cluster display by offset
    virtual std::string nodeName(size_t offset);

    /// Access to data buffer
    virtual Descriptor& data()             {    return m_clusterData; }

    /// Access to data buffer
    virtual const Descriptor& data() const {    return m_clusterData; }

    /// Access Node display
    MonitorDisplay* nodeDisplay() const    {    return m_nodes;       }

    /// Show the display header information (title, time, ...)
    void showHeader();

    /// Display the node information
    void showNodes(const SubfarmTorrentStatus& stat);

    /// Update all displays
    virtual void update();

    /// DIM command service callback
    static void excludedHandler(void* tag, void* address, int* size);
  };
  /// Static abstract object creator.
  ROMon::ClusterDisplay*  createTorrentSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv);
}      // End namespace ROMon
#endif /* ROMON_TORRENTSUBFARMDISPLAY_H */

