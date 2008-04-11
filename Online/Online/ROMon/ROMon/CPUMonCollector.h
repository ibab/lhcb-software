// $Id: CPUMonCollector.h,v 1.1 2008-04-11 12:12:36 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/CPUMonCollector.h,v 1.1 2008-04-11 12:12:36 frankb Exp $
#ifndef ROMON_CPUMONCOLLECTOR_H
#define ROMON_CPUMONCOLLECTOR_H 1

// Framework include files
#include "ROMon/ROMonGblBuffer.h"
#include "ROMon/CPUMonListener.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {


  /**@class CPUMonCollector CPUMonCollector.h GaudiOnline/CPUMonCollector.h
   *
   * Readout monitor DIM server for a single node
   *
   * @author M.Frank
   */
  class CPUMonCollector : public ROMonGblBuffer, virtual public ROUpdateHandler {
  protected:
    CPUMonListener m_ncpu;
    CPUMonListener m_info;
    CPUMonListener m_stat;
    bool           m_needUpdate;
  public:
    /// Standard constructor
    CPUMonCollector(int argc, char** argv);
    /// Default destructor
    virtual ~CPUMonCollector() {}
    /// Start monitoring activity
    virtual int monitor();
    /// Update handler
    virtual void update();
    /// Help printout in case of -h /? or wrong arguments
    static void help();
  };
}      // End namespace ROMon
#endif /* ROMON_CPUMONCOLLECTOR_H */

