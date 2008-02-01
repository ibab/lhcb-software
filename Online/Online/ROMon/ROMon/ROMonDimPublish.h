// $Id: ROMonDimPublish.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/ROMonDimPublish.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
#ifndef ROMON_ROMONDIMPUBLISH_H
#define ROMON_ROMONDIMPUBLISH_H 1

// Framework include files
#include "ROMon/ROMonPublish.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class ROMonDimPublish ROMon.h GaudiOnline/ROMon.h
   *
   * Readout monitor server
   *
   * @author M.Frank
   */
  class ROMonDimPublish : public ROMonPublish  {
  protected:
    unsigned int m_service;
    static void feedData(void* tag, void** address, int* size, int* first);
  public:
    /// Standard constructor
    ROMonDimPublish(int argc , char** argv);
    /// Default destructor
    virtual ~ROMonDimPublish();
    /// Publish the collected information
    virtual void publish();
  };

}      // End namespace ROMon
#endif /* ROMON_ROMONDIMPUBLISH_H */

