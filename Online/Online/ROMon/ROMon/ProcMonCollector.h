// $Id: ProcMonCollector.h,v 1.1 2008-04-22 15:22:28 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/ProcMonCollector.h,v 1.1 2008-04-22 15:22:28 frankb Exp $
#ifndef ROMON_CPUMONCOLLECTOR_H
#define ROMON_CPUMONCOLLECTOR_H 1

// Framework include files
#include "ROMon/ROMonGblBuffer.h"
#include "ROMon/FMCMonListener.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {


  /**@class ProcMonCollector ProcMonCollector.h GaudiOnline/ProcMonCollector.h
   *
   * Readout monitor DIM server for a single node
   *
   * @author M.Frank
   */
  class ProcMonCollector : public ROMonGblBuffer, virtual public ROUpdateHandler {
  protected:
    FMCMonListener m_ntasks;
    FMCMonListener m_info;
    bool           m_needUpdate;
  public:
    /// Standard constructor
    ProcMonCollector(int argc, char** argv);
    /// Default destructor
    virtual ~ProcMonCollector() {}
    /// Start monitoring activity
    virtual int monitor();
    /// Update handler
    virtual void update();
    /// Help printout in case of -h /? or wrong arguments
    static void help();
  };
}      // End namespace ROMon
#endif /* ROMON_CPUMONCOLLECTOR_H */

