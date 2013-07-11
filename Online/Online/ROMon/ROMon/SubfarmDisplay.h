// $Id: SubfarmDisplay.h,v 1.9 2010-09-17 09:47:12 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/SubfarmDisplay.h,v 1.9 2010-09-17 09:47:12 frankb Exp $
#ifndef ROMON_SUBFARMDISPLAY_H
#define ROMON_SUBFARMDISPLAY_H 1

// Framework includes
#include "ROMon/ClusterDisplay.h"
// C/C++ include files
#include <map>

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

    /// Partition name
    std::string     m_partition;

    /// Counter with minimal values
    std::map<std::string,std::map<std::string,std::pair<long,long> > > m_minimal;

    long m_measure;

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

    /// Update all displays
    virtual void updateDisplay(const Nodeset& ns);

    /// Update display content
    virtual void updateDisplay(const Node& n) 
    { this->ROMonDisplay::updateDisplay(n); }
  };

  /// Static abstract object creator.
  ClusterDisplay*  createSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv);
}      // End namespace ROMon
#endif /* ROMON_SUBFARMDISPLAY_H */

