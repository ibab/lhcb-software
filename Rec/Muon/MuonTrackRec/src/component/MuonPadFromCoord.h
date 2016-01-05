// $Id: MuonPadFromCoord.h,v 1.5 2010-02-10 19:20:17 ggiacomo Exp $
#ifndef MUONPADFROMCOORD_H 
#define MUONPADFROMCOORD_H 1
#include <map>
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "MuonInterfaces/IMuonPadRec.h"            // Interface

/** @class MuonPadFromCoord MuonPadFromCoord.h
 *  converts MuonCoord to MuonLogPad objects 
 *
 *  @author G.Graziani
 *  @date   2014-03-11
 */

namespace LHCB {
  class MuonTileID;
}


class MuonPadFromCoord : public extends<GaudiTool, IMuonPadRec , IIncidentListener>
{
public: 
  /// Standard constructor
  MuonPadFromCoord( const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  ~MuonPadFromCoord( ) override; ///< Destructor

  StatusCode buildLogicalPads(const std::vector<MuonLogHit*>* myhits ) override;
  const std::vector<MuonLogPad*>* pads() override;


  // from GaudiTool
  StatusCode 	initialize () override;
  StatusCode 	finalize () override;

  // from IIncidentListener
  void handle ( const Incident& incident ) override;
private:
  
  std::vector<LHCb::MuonTileID> m_tiles;
  std::vector<std::unique_ptr<MuonLogHit>> m_hits;
  std::vector<std::unique_ptr<MuonLogPad>> m_pads;
  void clearPads();
  bool m_padsReconstructed = false;
};
#endif // MUONPADFROMCOORD_H
