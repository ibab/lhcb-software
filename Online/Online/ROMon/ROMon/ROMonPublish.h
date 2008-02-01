// $Id: ROMonPublish.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/ROMonPublish.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
#ifndef ROMON_ROMONPUBLISH_H
#define ROMON_ROMONPUBLISH_H 1

// Framework include files
#include "ROMon/ROMonGblBuffer.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class ROMonPublish ROMon.h GaudiOnline/ROMon.h
   *
   * Readout monitor information publisher
   *
   * @author M.Frank
   */
  class ROMonPublish : public ROMonGblBuffer {
  public:
    /// Standard constructor
    ROMonPublish(int argc , char** argv);
    /// Default destructor
    virtual ~ROMonPublish() {}
    /// Help printout in case of -h /? or wrong arguments
    static void help();
    /// Start monitoring activity
    virtual int monitor();
    /// Publish the collected information
    virtual void publish() = 0;
  };
}      // End namespace ROMon
#endif /* ROMON_ROMONPUBLISH_H */

