// $Id: PartitionListener.h,v 1.1 2008-04-30 08:39:24 frankb Exp $
//====================================================================
//  ROLogger
//--------------------------------------------------------------------
//
//  Package    : ROLogger
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/PartitionListener.h,v 1.1 2008-04-30 08:39:24 frankb Exp $
#ifndef ROLOGGER_PARTITIONLISTENER_H
#define ROLOGGER_PARTITIONLISTENER_H

// C++ include files
#include <string>
#include "CPP/Interactor.h"

/*
 *   ROLogger namespace declaration
 */
namespace ROLogger {

  /**@class PartitionListener PartitionListener.h ROLogger/PartitionListener.h
   *
   *   DIM error logger
   *
   *   @author M.Frank
   */
  class PartitionListener : public Interactor {
  protected:
    int         m_subFarmDP;
    int         m_nodesDP;
    Interactor* m_parent;

  public:
    /// Standard constructor with object setup through parameters
    PartitionListener(Interactor* parent,const std::string& name);
    /// Standard destructor
    virtual ~PartitionListener();
    /// DIM command service callback
    static void subFarmHandler(void* tag, void* address, int* size);
    /// DIM command service callback
    static void nodeHandler(void* tag, void* address, int* size);
  };
}      /* End namespace ROLogger */
#endif /* ROLOGGER_PARTITIONLISTENER_H */
