#include <vector>

#include "GaudiKernel/IIncidentSvc.h" 
#include "Kernel/MuonTileID.h"
#include "Event/MuonCoord.h"

// local
#include "MuonInterfaces/MuonLogHit.h"
#include "MuonInterfaces/MuonLogPad.h"
#include "MuonPadFromCoord.h"
using namespace LHCb;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : MuonPadFromCoord
//
//  2014-03-11: G.Graziani 
//              interface from MuonCoords to MuonLogPad objects
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MuonPadFromCoord )


MuonPadFromCoord::MuonPadFromCoord( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : base_class ( type, name , parent )
{
  declareInterface<IMuonPadRec>(this);
  m_tiles.clear(); m_tiles.reserve(50000);
  m_hits.clear(); m_hits.reserve(50000);
  m_pads.clear(); m_pads.reserve(50000);
}

MuonPadFromCoord::~MuonPadFromCoord() = default;

void MuonPadFromCoord::clearPads() 
{
  m_tiles.clear();
  m_hits.clear();
  m_pads.clear();
}

void MuonPadFromCoord::handle ( const Incident& incident )
{ 
  if ( IncidentType::EndEvent == incident.type() ) {
    if ( msgLevel(MSG::DEBUG) )debug() << "End Event: clear pads"<<endmsg;
    clearPads() ;
    m_padsReconstructed = false;
  }
}




//=============================================================================
StatusCode 	MuonPadFromCoord::initialize ()
{
  StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc;
  incSvc()->addListener( this, IncidentType::EndEvent );
  return StatusCode::SUCCESS ;
}


StatusCode 	MuonPadFromCoord::finalize ()
{
  return   GaudiTool::finalize() ;
   
}

const std::vector<MuonLogPad*>* MuonPadFromCoord::pads() { 
  if (! m_padsReconstructed) 
    buildLogicalPads(NULL);
  return (const std::vector<MuonLogPad*>*) (&m_pads);
}



StatusCode MuonPadFromCoord::buildLogicalPads(const std::vector<MuonLogHit*>* ) {
  MuonCoords* coords = get<MuonCoords>(MuonCoordLocation::MuonCoords);
  if ( !coords ) {
    err() << " ==> Cannot retrieve MuonCoords " << endmsg;
    return StatusCode::FAILURE;
  }
  // loop over the coords

  for (const auto&  coord : *coords) {
    MuonTileID tile = MuonTileID(coord->key());
    unsigned int station = tile.station();
    unsigned int region = tile.region();
    double uncross = (station == 0 || ((station>2)&&(region==0))) ? false : coord->uncrossed();
    int time1= (int) coord->digitTDC1();
    int time2= (int) coord->digitTDC2();

    if(uncross) {
      m_tiles.push_back(tile);
      m_hits.emplace_back( new MuonLogHit(tile) );
      m_hits.back()->setTime(time1);
      m_pads.emplace_back(new MuonLogPad(m_hits.back().get()));
    } else {
      const std::vector<MuonTileID > thetiles= coord->digitTile();
      tile = MuonTileID(thetiles[0]);
      m_tiles.push_back(tile);
      m_hits.emplace_back(new MuonLogHit(tile));
      auto hit1 = m_hits.back().get();
      m_hits.back()->setTime(time1);
      if (thetiles.size() == 1) {        
        m_pads.emplace_back(new MuonLogPad(hit1));
        m_pads.back()->settruepad();
      } else {
        tile = MuonTileID(thetiles[1]);
        m_tiles.push_back(tile);
        m_hits.emplace_back( new MuonLogHit(tile) );
        m_hits.back()->setTime(time2);
        m_pads.emplace_back(new MuonLogPad(hit1,m_hits.back().get()));
      }
    }
  }
  debug() << "Got from MuonCoords " << m_hits.size()<<" MuonLogHits and " <<
    m_pads.size() << " MuonLogPads" << endmsg;
  m_padsReconstructed=true;
  return StatusCode::SUCCESS;
}


