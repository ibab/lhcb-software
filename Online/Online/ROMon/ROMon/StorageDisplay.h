// $Id: StorageDisplay.h,v 1.5 2009-01-09 10:30:18 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/StorageDisplay.h,v 1.5 2009-01-09 10:30:18 frankb Exp $
#ifndef ROMON_STORAGEDISPLAY_H
#define ROMON_STORAGEDISPLAY_H 1

// Framework includes
#include "ROMon/ClusterDisplay.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class StorageDisplay ROMon.h GaudiOnline/StorageDisplay.h
   *
   *   Monitoring display for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class StorageDisplay : public ClusterDisplay  {
  protected:

    /// Reference to the MBMBuffer diaplay
    MonitorDisplay* m_buffers;

    /// reference to the Streams display
    MonitorDisplay* m_streams;

    /// Reference to the logger window
    MonitorDisplay* m_logging;

    /// Reference to the display showing current files
    MonitorDisplay* m_files;

    /// Reference to the HLT receiver display
    MonitorDisplay* m_hltRec;

    /// Reference node selector display if used as sub-dispay
    MonitorDisplay* m_select;

    /// Partition name for projection(s)
    std::string     m_partName;

  public:
    /// Standard constructor
    StorageDisplay(int argc, char** argv);

    /// Initializing constructor for using display as sub-display
    StorageDisplay(int width, int height, int posx, int posy, int argc, char** argv);

    /// Standard destructor
    virtual ~StorageDisplay();

    /// Initialize the display
    void init(int flag, int argc, char** argv);

    /// Number of nodes in the dataset
    virtual size_t numNodes();

    /// Retrieve cluster name from cluster display
    virtual std::string clusterName() const;

    /// Retrieve node name from cluster display by offset
    virtual std::string nodeName(size_t offset);

    /// Access Node display
    virtual MonitorDisplay* nodeDisplay() const;

    /// Update selector information
    void showSelector(const Nodeset& ns);

    /// Show the display header information (title, time, ...)
    void showHeader(const Nodeset& ns);

    /// Show the data logging status
    void showLogging();

    /// Show the file information
    void showFiles();

    /// Display the Stream information
    void showStreams(const Nodeset& ns);

    /// Show the information about MBM buffers on the storage nodes
    void showBuffers(const Nodeset& ns);

    /// Show the information about the HLT receiver tasks
    void showHLT(const Nodeset& ns);

    /// Update all displays
    virtual void updateDisplay(const Nodeset& ns);
  };
}      // End namespace ROMon
#endif /* ROMON_STORAGEDISPLAY_H */

