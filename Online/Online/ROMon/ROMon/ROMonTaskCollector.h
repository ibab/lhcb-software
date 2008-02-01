// $Id: ROMonTaskCollector.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/ROMonTaskCollector.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
#ifndef ROMON_ROMONTASKCOLLECTOR_H
#define ROMON_ROMONTASKCOLLECTOR_H 1

// Framework include files
#include "ROMon/ROMonServer.h"
#include "ROMon/ROMonGblBuffer.h"
#include "ROMon/RODimFSMListener.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class ROMonTaskCollector ROMon.h GaudiOnline/ROMon.h
   *
   * Readout monitor DIM server for a single node
   *
   * @author M.Frank
   */
  class ROMonTaskCollector : public ROMonGblBuffer {
  protected:
    ROMonServer       m_mbmInfo;
    RODimFSMListener  m_fsmInfo;

  public:
    /// Standard constructor
    ROMonTaskCollector(int argc, char** argv);
    /// Default destructor
    virtual ~ROMonTaskCollector() {}
    /// Start monitoring activity
    virtual int monitor();
    /// Help printout in case of -h /? or wrong arguments
    static void help();
  };
}      // End namespace ROMon
#endif /* ROMON_ROMONTASKCOLLECTOR_H */

