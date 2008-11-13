// $Id: NodeStatsPublisher.h,v 1.2 2008-11-13 12:13:32 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/NodeStatsPublisher.h,v 1.2 2008-11-13 12:13:32 frankb Exp $
#ifndef ROMON_NODESTATSPUBLISHER_H
#define ROMON_NODESTATSPUBLISHER_H 1

// Framework include files
#include "ROMon/ROMonGblBuffer.h"
#include "ROMon/FMCMonListener.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class NodeStatsPublisher NodeStatsPublisher.h GaudiOnline/NodeStatsPublisher.h
   *
   * Readout monitor DIM server for a single node
   *
   * @author M.Frank
   */
  class NodeStatsPublisher : virtual public ROUpdateHandler {

  public:
    struct _BaseSvc {
      virtual ~_BaseSvc() {}
      virtual void start() = 0;
      virtual void load() = 0;
      virtual void update() = 0;
      virtual void print() const = 0;
    };

  protected:
    FMCMonListener m_stat;
    FMCMonListener m_mbm;
    int            m_print;
    bool           m_verbose;
    _BaseSvc*      m_service[4];
    bool           m_needUpdate;
    int            m_mbmDelay;
    int            m_statDelay;

  public:
    /// Standard constructor
    NodeStatsPublisher(int argc, char** argv);
    /// Default destructor
    virtual ~NodeStatsPublisher();
    /// Start monitoring activity
    virtual int monitor();
    /// Update handler
    virtual void update();
    /// Help printout in case of -h /? or wrong arguments
    static void help();
  };
}      // End namespace ROMon
#endif /* ROMON_NODESTATSPUBLISHER_H */

