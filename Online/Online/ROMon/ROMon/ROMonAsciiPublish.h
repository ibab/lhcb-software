// $Id: ROMonAsciiPublish.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/ROMonAsciiPublish.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
#ifndef ROMON_ROMONASCIIPUBLISH_H
#define ROMON_ROMONASCIIPUBLISH_H 1

// Framework include files
#include "ROMon/ROMonPublish.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class ROMonAsciiPublish ROMon.h GaudiOnline/ROMon.h
   *
   * Readout monitor information publisher to stdout
   *
   * @author M.Frank
   */
  class ROMonAsciiPublish : public ROMonPublish  {
  public:
    /// Standard constructor
    ROMonAsciiPublish(int argc , char** argv) : ROMonPublish(argc,argv) {}
    /// Default destructor
    virtual ~ROMonAsciiPublish() {}
    /// Publish the collected information
    virtual void publish();
  };

}      // End namespace ROMon
#endif /* ROMON_ROMONASCIIPUBLISH_H */

