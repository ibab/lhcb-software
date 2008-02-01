// $Id: RODimFSMListener.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/RODimFSMListener.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
#ifndef ROMON_ROMONDIMFSMLISTENER_H
#define ROMON_ROMONDIMFSMLISTENER_H 1

// Framework includes
#include "ROMon/RODimListener.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class RODimFSMListener ROMon.h GaudiOnline/ROMon.h
   *
   *   DIM FSM status listener to collect FSM information from tasks
   *
   *   @author M.Frank
   */
  class RODimFSMListener : public RODimListener {
  public:
    /// Standard constructor
    RODimFSMListener(bool verbose = false) : RODimListener(verbose) {}
    /// Standard destructor
    virtual ~RODimFSMListener();
    /// Add handler for a given message source
    virtual void addHandler(const std::string& node,const std::string& svc);
    /// Remove handler for a given message source
    virtual void removeHandler(const std::string& node, const std::string& svc);
    /// DimInfoHandler overload
    static void infoHandler(void* tag, void* address, int* size);
  };
}      // End namespace ROMon
#endif /* ROMON_ROMONDIMFSMLISTENER_H */

