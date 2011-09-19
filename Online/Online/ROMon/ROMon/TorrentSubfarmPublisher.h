// $Id: SubfarmPublisher.h,v 1.3 2010-10-14 06:44:04 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/SubfarmPublisher.h,v 1.3 2010-10-14 06:44:04 frankb Exp $
#ifndef ROMON_TORRENTSUBFARMPUBLISHER_H
#define ROMON_TORRENTSUBFARMPUBLISHER_H 1

// Framework include files
#include "ROMon/FMCMonListener.h"

/*
 *   ROMon namespace declaration
 */
namespace BitTorrent {

  /**@class TorrentSubfarmPublisher TorrentSubfarmPublisher.h GaudiOnline/TorrentSubfarmPublisher.h
   *
   * Readout monitor DIM server for a collective item
   *
   * @author M.Frank
   */
  class TorrentSubfarmPublisher : virtual public ROMon::ROUpdateHandler {

  public:
    struct _BaseSvc {
      virtual ~_BaseSvc() {}
      virtual void start() = 0;
      virtual void load() = 0;
      virtual void update() = 0;
      virtual void print() const = 0;
    };
    typedef ROMon::FMCMonListener _Torrents;
  protected:
    _Torrents      m_torrents;
    int            m_print;
    bool           m_verbose;
    _BaseSvc*      m_service[1];
    int            m_delay;

  public:
    /// Standard constructor
    TorrentSubfarmPublisher(int argc, char** argv);
    /// Default destructor
    virtual ~TorrentSubfarmPublisher();
    /// Start monitoring activity
    virtual int monitor();
    /// Update handler
    virtual void update(void* param);
    /// Help printout in case of -h /? or wrong arguments
    static void help();
  };
}      // End namespace BitTorrent

#endif /* ROMON_TORRENTSUBFARMPUBLISHER_H */

