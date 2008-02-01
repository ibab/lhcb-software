// $Id: ROMonNodeCollector.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/ROMonNodeCollector.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
#ifndef ROMON_ROMONNODECOLLECTOR_H
#define ROMON_ROMONNODECOLLECTOR_H 1

// Framework include files
#include "ROMon/ROMonGblBuffer.h"
#include "ROMon/RODimNodeListener.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class ROMonNodeCollector ROMon.h GaudiOnline/ROMon.h
   *
   * Readout monitor DIM server for a single node
   *
   * @author M.Frank
   */
  class ROMonNodeCollector : public ROMonGblBuffer {
  protected:
    RODimNodeListener m_info;

  public:
    /// Standard constructor
    ROMonNodeCollector(int argc, char** argv);
    /// Default destructor
    virtual ~ROMonNodeCollector() {}
    /// Start monitoring activity
    virtual int monitor();
    /// Help printout in case of -h /? or wrong arguments
    static void help();
  };
}      // End namespace ROMon
#endif /* ROMON_ROMONNODECOLLECTOR_H */

