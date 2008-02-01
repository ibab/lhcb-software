// $Id: ROMonInfoDump.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/ROMonInfoDump.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
#ifndef ROMON_ROMONINFODUMP_H
#define ROMON_ROMONINFODUMP_H 1

// Framework include files
#include "dic.hxx"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class ROMonInfoDump ROMon.h GaudiOnline/ROMon.h
   *
   *
   * @author M.Frank
   */
  class ROMonInfoDump : public DimInfoHandler {
  protected:
    DimInfo*    m_info;
  public:
    /// Standard constructor
    ROMonInfoDump(int argc, char** argv);
    /// Default destructor
    virtual ~ROMonInfoDump();
    /// Help printout in case of -h /? or wrong arguments
    static void help();
    /// DIM info callback
    virtual void infoHandler();
  };

}      // End namespace ROMon
#endif /* ROMON_ROMONINFODUMP_H */

