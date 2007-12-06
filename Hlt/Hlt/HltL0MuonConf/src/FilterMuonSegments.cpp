// $Id: FilterMuonSegments.cpp,v 1.1 2007-12-06 16:12:15 hernando Exp $
// Include files 

// from Gaudi

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
// local
#include "FilterMuonSegments.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterMuonSegments
//
// 2007-11-29 : Sandra Amato
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FilterMuonSegments );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterMuonSegments::FilterMuonSegments( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator ){ }
//=============================================================================
// Destructor
//=============================================================================
FilterMuonSegments::~FilterMuonSegments() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FilterMuonSegments::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FilterMuonSegments::execute() {
  
  debug() << "==> Execute" << endmsg;
  setFilterPassed(false);

  if (m_inputTracks->size()<1) return StatusCode::SUCCESS;
  
  if (m_inputTracks2->size()<1) return StatusCode::SUCCESS;


  debug() << "muon segments size "<< m_inputTracks2->size() << endmsg;


  debug() << "L0 confirmed tracks size " << m_inputTracks->size()<< endmsg;
  
  for ( std::vector<Track*>::const_iterator itT = m_inputTracks2->begin();
        m_inputTracks2->end() != itT; itT++ ) {
    
    Track* pTrack = (*itT);
    MuonTileID tileM2=0;
    MuonTileID tileM3=0;
    std::vector<LHCbID> muonTiles= pTrack->lhcbIDs();
    for( std::vector<LHCbID>::iterator itlhcbid = muonTiles.begin(); 
              itlhcbid < muonTiles.end(); itlhcbid++){
      MuonTileID tileMuonSegment = itlhcbid->muonID();
      if(tileMuonSegment.station() == 1) tileM2= tileMuonSegment;
      if(tileMuonSegment.station() == 2) tileM3= tileMuonSegment;
    } 
    debug() << "tile ID from muon segment station " << tileM2.station() 
            << " " << tileM2 <<" station " << tileM3.station() << " " <<
      tileM3 << endmsg;   
    //reject the muon tracks that share the M3 pad with one T Track from L0Muon    
    bool L0clone=false;
    for(std::vector<LHCb::Track*>::iterator itL0Mu = m_inputTracks->begin();
	itL0Mu!= m_inputTracks->end();
	itL0Mu++){ 
      std::vector<LHCb::LHCbID> lista= (*itL0Mu)->lhcbIDs ();
      debug() << "lista size " << lista.size() << endmsg;
      MuonTileID L0tileM3;
      for(std::vector<LHCbID>::iterator it=lista.begin();it<lista.end();it++){
	if(it->isMuon()){
	  MuonTileID tile=it->muonID();
	  if(tile.station()==2)L0tileM3=tile;
	}
      }
      debug() << "tile M3 from T track " 
	      << L0tileM3.station() << " " << L0tileM3 << endmsg;
      if(tileM3==L0tileM3)L0clone=true;
      
    }//for(std::vector<LHCb::Track*>::iterator itL0Mu
    if(L0clone){
      debug() << "skipping " << endmsg; 
      continue;
    }
    m_outputTracks->push_back(*itT);

    debug() << "saved "<< endmsg; 
    setFilterPassed(true);

    
    
  }//end loop muons
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FilterMuonSegments::finalize() {
  
  debug() << "==> Finalize" << endmsg;
  
  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
