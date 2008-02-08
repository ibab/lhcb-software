// $Id: StorageDisplay.h,v 1.3 2008-02-08 18:19:34 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/StorageDisplay.h,v 1.3 2008-02-08 18:19:34 frankm Exp $
#ifndef ROMON_STORAGEDISPLAY_H
#define ROMON_STORAGEDISPLAY_H 1

// Framework includes
#include "ROMon/ROMonDisplay.h"

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
  class StorageDisplay : public ROMonDisplay  {
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

    /// Partition name for projection(s)
    std::string     m_partName;

  public:
    /// Standard constructor
    StorageDisplay(int argc, char** argv);

    /// Standard destructor
    virtual ~StorageDisplay();

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

