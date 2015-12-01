// $Id: BufferDisplay.cpp,v 1.5 2010-10-15 07:42:00 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/TorrentNodeDisplay.cpp,v 1.5 2010-10-15 07:42:00 frankb Exp $

#ifndef ROMON_TORRENTNODEDISPLAY_H
#define ROMON_TORRENTNODEDISPLAY_H 1

// Framework includes
#include "ROMon/InternalDisplay.h"
#include "ROMon/TorrentMon.h"

// C++ include files
#include <string>

// Forward declarations
namespace BitTorrent  {
  class TorrentStatus;
  class SubfarmTorrentStatus;
}

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class TorrentNodeDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display showing all processes on a given node.
   *
   *   @author M.Frank
   */
  class TorrentNodeDisplay : public InternalDisplay {
    /// Node name
    std::string  m_node;
  public:
    /// Initializing constructor
    TorrentNodeDisplay(InternalDisplay* parent, const std::string& title, const std::string& node, int height=60,int width=155);
    /// Standard destructor
    virtual ~TorrentNodeDisplay();
    /// Update display content
    virtual void update(const void* data);
    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }
    /// Update display content
    void updateContent(const BitTorrent::SubfarmTorrentStatus& status);
  };

}        /* End namespace ROMon         */
#endif   /* ROMON_TORRENTNODEDISPLAY_H  */
