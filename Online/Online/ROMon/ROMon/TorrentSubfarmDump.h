// $Id: SubfarmDump.h,v 1.3 2010-10-14 06:44:04 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/SubfarmDump.h,v 1.3 2010-10-14 06:44:04 frankb Exp $
#ifndef ROMON_TORRENTSUBFARMDUMP_H
#define ROMON_TORRENTSUBFARMDUMP_H 1

// Framework include files

/*
 *   ROMon namespace declaration
 */
namespace BitTorrent {

  // Forward declarations
  class SubfarmTorrentStatus;

  /**@class SubfarmDump SubfarmDump.h GaudiOnline/SubfarmDump.h
   *
   * Readout monitor DIM server for a collective item
   *
   * @author M.Frank
   */
  class TorrentSubfarmDump {

  protected:
    int            m_id;
    int            m_delay;

  public:
    /// Standard constructor
    TorrentSubfarmDump(int argc, char** argv);
    /// Default destructor
    virtual ~TorrentSubfarmDump();
    /// Update handler
    virtual void update(const SubfarmTorrentStatus& info);
    /// Help printout in case of -h /? or wrong arguments
    static void help();
  };
}      // End namespace ROMon

#endif /* ROMON_TORRENTSUBFARMDUMP_H */

