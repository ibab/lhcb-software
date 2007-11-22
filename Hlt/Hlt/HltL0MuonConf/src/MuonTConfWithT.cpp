// $Id: MuonTConfWithT.cpp,v 1.8 2007-11-22 11:05:36 sandra Exp $
// Include files 

// from Gaudi

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
// local
#include "MuonTConfWithT.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonTConfWithT
//
// 2007-02-14 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonTConfWithT );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonTConfWithT::MuonTConfWithT( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  declareProperty("OutputMuonTracksName"   ,
                  m_outputMuonTracksName = "Hlt/Tracks/ConfirmedTMuon");
  declareProperty("debugInfo", m_debugInfo = false);
  
}
//=============================================================================
// Destructor
//=============================================================================
MuonTConfWithT::~MuonTConfWithT() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MuonTConfWithT::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  
  m_prepareMuonSeed = tool<IPrepareMuonTSeed>("PrepareMuonTSeed");
  
  m_TsaConfirmTool=tool<ITrackConfirmTool>( "TsaConfirmTool", this );
  
  m_totMuonAccepted = 0;
  m_countEvent = 0;
  m_totMuonInput = 0; 
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonTConfWithT::execute() {
  
  debug() << "==> Execute" << endmsg;
  
  debug() << "muon segments size "<< m_inputTracks2->size() << endmsg;
  //debug() << "muon segments size "<< m_patInputTracks->size()<< endmsg;
  m_countEvent++;

//Container of T tracks that will come out for each L0muon track    
  std::vector<LHCb::Track*>* foundTracksTmp = new std::vector<LHCb::Track*>;
  foundTracksTmp->reserve(50);

//Container with all T tracks
  Tracks* muontracks = new Tracks();
  muontracks->reserve(50);

  debug() << "L0 confirmed tracks size " << m_inputTracks->size()<< endmsg;
  
  int muonCounter=0;  
 // m_totMuonInput=m_totMuonInput+m_patInputTracks->size();
  m_totMuonInput=m_totMuonInput+m_inputTracks2->size();
  int nummu=0;  
  for ( std::vector<Track*>::const_iterator itT = m_inputTracks2->begin();
        m_inputTracks2->end() != itT; itT++ ) {
//   for ( std::vector<Track*>::const_iterator itT = m_patInputTracks->begin();
//        m_patInputTracks->end() != itT; itT++ ) {  
    
    Track* pTrack = (*itT);
    
    std::vector<LHCbID> muonTiles= pTrack->lhcbIDs();
    MuonTileID tileM2=muonTiles[0].muonID();
    MuonTileID tileM3=muonTiles[1].muonID();
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
      std::vector<LHCbID>::iterator it;
      for(it=lista.begin();it<lista.end();it++){
	if(it->isMuon()){
	  MuonTileID tile=it->muonID();
	  if(tile.station()==2)L0tileM3=tile;
	}
      }
      debug() << "tile M3 from T track " 
	      << L0tileM3.station() << " " << L0tileM3 << endmsg;
      if(tileM3==L0tileM3)L0clone=true;
      
    }//for(std::vector<LHCb::Track*>::iterator itL0Mu
    nummu++;		
    if(L0clone){
      debug() << "skipping " << endmsg; 
      continue;
    }
    m_totMuonAccepted++;  
    
    muonCounter++;
    
    if(m_debugInfo) info()<<"Processing muon number "<<muonCounter<<endmsg;
    
    LHCb::State myState;
    StatusCode sctmp = m_prepareMuonSeed->prepareSeed( *(pTrack) , myState );
    if( sctmp.isFailure() ){
      err()<<"Failed to prepare the seed"<<endmsg;
    }
    //play with seeding tool
    m_TsaConfirmTool->tracks( myState , *foundTracksTmp );
    for( std::vector<LHCb::Track*>::iterator iterTrack = foundTracksTmp->begin();
	 iterTrack != foundTracksTmp->end() ;
	 iterTrack++ ) {
      LHCb::Track* fitTrack =  (*iterTrack)->clone();
      fitTrack->addToAncestors (*itT);
      
      fitTrack->addToLhcbIDs(tileM2);
      fitTrack->addToLhcbIDs(tileM3);
      
      fitTrack->setFlag(Track::L0Candidate,false);
      muontracks->insert(fitTrack);
      m_outputTracks->push_back(fitTrack);
      
    }
    
    if(m_debugInfo)
      info()<<"Tool found : "<<foundTracksTmp->size()
	    <<" tracks for the seed number "<<muonCounter<<" !"<<endmsg;
    foundTracksTmp->clear();
    
    
  }//end loop muons
  
  delete foundTracksTmp;
  put(muontracks,m_outputMuonTracksName);
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonTConfWithT::finalize() {
  
  debug() << "==> Finalize" << endmsg;
  float rate_input=m_totMuonInput/m_countEvent;
  float rate_accepted=m_totMuonAccepted/m_countEvent;
  info()<<" average muon tracks in input "<< rate_input<<endreq;
  info()<<" average muon tracks after clone rejection "<< 
    rate_accepted<<endreq;
  
  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
