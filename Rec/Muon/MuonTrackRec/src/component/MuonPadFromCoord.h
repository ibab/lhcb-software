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


class MuonPadFromCoord : public GaudiTool, virtual public IMuonPadRec , virtual public IIncidentListener 
{
public: 
  /// Standard constructor
  MuonPadFromCoord( const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  virtual ~MuonPadFromCoord( ); ///< Destructor

  virtual StatusCode buildLogicalPads(const std::vector<MuonLogHit*>* myhits );
  virtual const std::vector<MuonLogPad*>* pads();


  // from GaudiTool
  virtual StatusCode 	initialize ();
  virtual StatusCode 	finalize ();

  // from IIncidentListener
  virtual void handle ( const Incident& incident );    
private:
  
  std::vector<LHCb::MuonTileID*> m_tiles;
  std::vector<MuonLogHit*> m_hits;
  std::vector<MuonLogPad*> m_pads;
  void clearPads();
  bool m_padsReconstructed;
};
#endif // MUONPADFROMCOORD_H
