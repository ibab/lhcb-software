// $Id: ROMonServer.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/ROMonServer.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
#ifndef ROMON_ROMONSERVER_H
#define ROMON_ROMONSERVER_H 1

#include "MBM/bmdef.h"
#include "ROMon/ROMon.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  // Forward declarations
  class ROMonInfo;

  /**@class ROMonServer ROMonServer.h ROMon/ROMonServer.h
   *
   * Readout monitor server
   *
   * @author M.Frank
   */
 class ROMonServer  {
  protected:
    /// Time sleep in milliseconds between updates
    int m_delay;

  public:
    /// Standard constructor
    ROMonServer(int sleep_delay) : m_delay(sleep_delay) {}
    /// Default destructor
    virtual ~ROMonServer() {}
    /// Dump buffer information to memory
    void dumpBufferInfo(const char* bm_name, BMID dsc, MBMBuffer* mbm);
    /// Handle update
    virtual int handle(void* buff, size_t buff_len);
    /// Start monitoring activity
    int monitor(void* buff, size_t buff_len);
  };
}      // End namespace ROMon
#endif /* ROMON_ROMONSERVER_H */

