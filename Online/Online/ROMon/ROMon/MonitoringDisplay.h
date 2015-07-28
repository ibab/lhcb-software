// $Id: MonitoringDisplay.h,v 1.7 2010-09-17 09:47:12 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/MonitoringDisplay.h,v 1.7 2010-09-17 09:47:12 frankb Exp $
#ifndef ROMON_MONITORINGDISPLAY_H
#define ROMON_MONITORINGDISPLAY_H 1

// Framework includes
#include "ROMon/ClusterDisplay.h"

// C++ include files
#include <map>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  // Forward declarations
  class MBMBuffer;

  /**@class MonitoringDisplay ROMon.h GaudiOnline/MonitoringDisplay.h
   *
   *   Monitoring display for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class MonitoringDisplay : public ClusterDisplay  {
  protected:
    /// Reference to the Relay info display
    MonitorDisplay*          m_relay;

    /// reference to the node display
    MonitorDisplay*          m_nodes;

    /// Reference to the display showing monitoring tasks
    MonitorDisplay*          m_tasks;

    /// Reference node selector display if used as sub-dispay
    MonitorDisplay*          m_select;

    /// Partition name for projection(s)
    std::string              m_partName;

    /// Name pof relay node
    std::string              m_relayNode;

  private:
    /// Private copy constructor
    MonitoringDisplay(const MonitoringDisplay&) : ClusterDisplay(0,0) {}
    /// Private assignment operator
    MonitoringDisplay& operator=(const MonitoringDisplay&) { return *this; }

  public:
    struct Stream {
      int received;
      std::map<std::string,int> to;
      const MBMBuffer* buffer;
      Stream() : received(0), buffer(0) {}
      Stream(const Stream& s) : received(s.received), to(s.to), buffer(s.buffer) {}
      Stream& operator=(const Stream& s) { received=s.received; to = s.to; buffer = s.buffer; return *this;}
    };

    /// Standard constructor
    MonitoringDisplay(int argc, char** argv);

    /// Initializing constructor for using display as sub-display
    MonitoringDisplay(int width, int height, int posx, int posy, int argc, char** argv);

    /// Standard destructor
    virtual ~MonitoringDisplay();

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

    /// Display the node information
    void showNodes(const Nodeset& ns);

    /// Show the task information
    void showTasks(const Nodeset& ns);

    /// Show the relay information
    void showRelay(const Nodeset& ns);

    /// Update selection window information
    void showSelect(const Nodeset& ns);

    /// Update all displays
    virtual void updateDisplay(const Nodeset& ns);

    /// Update display content
    virtual void updateDisplay(const Node& n) { this->ROMonDisplay::updateDisplay(n); }
  };
  /// Static abstract object creator.
  ClusterDisplay*  createMonitoringDisplay(int width, int height, int posx, int posy, int argc, char** argv);
}      // End namespace ROMon
#endif /* ROMON_MONITORINGDISPLAY_H */

