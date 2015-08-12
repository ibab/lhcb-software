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
  : GaudiTool ( type, name , parent ),   m_padsReconstructed(false)
{
  declareInterface<IMuonPadRec>(this);
  m_tiles.clear(); m_tiles.reserve(50000);
  m_hits.clear(); m_hits.reserve(50000);
  m_pads.clear(); m_pads.reserve(50000);
}

MuonPadFromCoord::~MuonPadFromCoord() {
  clearPads();
} 

void MuonPadFromCoord::clearPads() 
{
  std::vector<MuonTileID*>::iterator it;
  for (it=m_tiles.begin() ; it != m_tiles.end(); it++) {
    delete (*it);
  }
  m_tiles.clear();
  std::vector<MuonLogHit*>::iterator il;
  for (il=m_hits.begin() ; il != m_hits.end(); il++) {
    delete (*il);
  }
  m_hits.clear();
  std::vector<MuonLogPad*>::iterator ih;
  for (ih=m_pads.begin() ; ih != m_pads.end(); ih++) {
    delete (*ih);
  }
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
    err() << " ==> Cannot retrieve MuonCoords " << endreq;
    return StatusCode::FAILURE;
  }
  MuonLogHit* log1;
  MuonLogHit* log2;
  // loop over the coords



  MuonCoords::const_iterator iCoord;
  for ( iCoord = coords->begin() ; iCoord != coords->end() ; iCoord++ ){
    MuonTileID* tile = new MuonTileID((*iCoord)->key());
    unsigned int station = tile->station();
    unsigned int region = tile->region();
    double uncross = (station == 0 || ((station>2)&&(region==0))) ? false : (*iCoord)->uncrossed();

    if(uncross) {
      m_tiles.push_back(tile);
      log1 = new MuonLogHit(tile);
      m_hits.push_back(log1);
      m_pads.push_back(new MuonLogPad(log1));
    }
    else {
      const std::vector<MuonTileID > thetiles= (*iCoord)->digitTile();
      tile = new MuonTileID(thetiles[0]);
      m_tiles.push_back(tile);
      log1 = new MuonLogHit(tile);
      m_hits.push_back(log1);
      if (thetiles.size() == 1) {        
        MuonLogPad* lpad=new MuonLogPad(log1);
        lpad->settruepad();
        m_pads.push_back(lpad);
      }
      else {
        tile = new MuonTileID(thetiles[1]);
        m_tiles.push_back(tile);
        log2 = new MuonLogHit(tile);
        m_hits.push_back(log2);
        m_pads.push_back(new MuonLogPad(log1,log2));
      }
    } 
  }  
  debug() << "Got from MuonCoords " << m_hits.size()<<" MuonLogHits and " <<
    m_pads.size() << " MuonLogPads" << endmsg;
  m_padsReconstructed=true;
  return StatusCode::SUCCESS;
}


