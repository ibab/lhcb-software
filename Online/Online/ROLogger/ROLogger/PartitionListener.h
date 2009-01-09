// $Id: PartitionListener.h,v 1.6 2009-01-09 10:31:20 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROLogger/ROLogger/PartitionListener.h,v 1.6 2009-01-09 10:31:20 frankb Exp $
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
    /// Reference to parent for communication
    Interactor* m_parent;
    /// Listener service name
    std::string m_name;
    /// Listener facility
    std::string m_facility;
    /// Dim service ID for calibration nodes
    int         m_calibNodesDP;
    /// Dim service ID for subfarms
    int         m_subFarmDP;
    /// Dim service ID for subfarm nodes
    int         m_nodesDP;
    /// Dim service ID for monitoring slice
    int         m_monSliceDP;
    /// Dim service ID for storage nodes
    int         m_storSliceDP;
    /// Dim service ID for reconstruction nodes (if availible)
    int         m_recSliceDP;
    
  public:
    /// Standard constructor with object setup through parameters
    PartitionListener(Interactor* parent,const std::string& name, const std::string& facility="gaudi");
    /// Standard destructor
    virtual ~PartitionListener();
    /// Access name
    const std::string& name() const {   return m_name; }
    /// DIM command service callback
    static void subFarmHandler(void* tag, void* address, int* size);
    /// DIM command service callback
    static void nodeHandler(void* tag, void* address, int* size);
    /// DIM command service callback
    static void calibNodeHandler(void* tag, void* address, int* size);
    /// DIM command service callback
    static void storSliceHandler(void* tag, void* address, int* size);
    /// DIM command service callback
    static void monSliceHandler(void* tag, void* address, int* size);
    /// DIM command service callback
    static void recSliceHandler(void* tag, void* address, int* size);
  };
}      /* End namespace ROLogger */
#endif /* ROLOGGER_PARTITIONLISTENER_H */
