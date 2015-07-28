// $Id: StorageDisplay.h,v 1.9 2010-09-17 09:47:12 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/StorageDisplay.h,v 1.9 2010-09-17 09:47:12 frankb Exp $
#ifndef ROMON_STORAGEDISPLAY_H
#define ROMON_STORAGEDISPLAY_H 1

// Framework includes
#include "ROMon/ClusterDisplay.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  // Forward declarations
  struct DataWriterInfo;

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

    /// Reference to the HLT receiver display
    MonitorDisplay* m_hltRec;

    /// Reference node selector display if used as sub-dispay
    MonitorDisplay* m_select;

    /// Partition name for projection(s)
    std::string     m_partName;

    struct Stream {
      std::string node, source;
      int sent, written, received;
      Stream() : sent(0), written(0), received(0) {}
      Stream(const Stream& s) 
        : node(s.node), source(s.source), sent(s.sent), written(s.written), received(s.received) {}
      Stream& operator=(const Stream& s) { 
        node     = s.node; 
        source   = s.source;
        sent     = s.sent;
        written  = s.written;
        received = s.received;
        return *this;
      }
    };

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

    /// Display the Stream information
    void showStreams(const Nodeset& ns);

    /// Show the information about MBM buffers on the storage nodes
    void showBuffers(const Nodeset& ns);

    /// Show the information about the HLT receiver tasks
    void showHLT(const Nodeset& ns);

    /// Update all displays
    virtual void updateDisplay(const Nodeset& ns);

    /// Update display content
    virtual void updateDisplay(const Node& n) 
    { this->ROMonDisplay::updateDisplay(n);   }
  };
  /// Static abstract object creator.
  ClusterDisplay*  createStorageDisplay(int width, int height, int posx, int posy, int argc, char** argv);
}      // End namespace ROMon
#endif /* ROMON_STORAGEDISPLAY_H */

