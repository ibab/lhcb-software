// $Id: SpaceMuonMatch.cpp,v 1.1.1.1 2007-05-22 10:20:32 asatta Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "SpaceMuonMatch.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SpaceMuonMatch
//
// 2007-02-21 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( SpaceMuonMatch );
using namespace LHCb;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SpaceMuonMatch::SpaceMuonMatch( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
 declareProperty( "xMatchCut",   m_x_cut      ); 
 declareProperty( "xMatchCut",   m_y_cut      ); 
 declareProperty( "xMatchCut",   m_space_cut      ); 
 declareProperty( "zMagCenter",   z_magnet_center      );
 declareProperty( "ptkickConstant",  m_ptkickConstant  =  1263.0      );
 declareProperty( "Input2TracksName", m_input2TracksName);
 
 declareProperty("OutputMuonTracksName"   ,
                  m_outputMuonTracksName );

}
//=============================================================================
// Destructor
//=============================================================================
SpaceMuonMatch::~SpaceMuonMatch() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode SpaceMuonMatch::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  m_x_cut=30;
  m_y_cut=40;
  m_space_cut=60;
  
  z_magnet_center=5300;
  m_input2Tracks= &(m_hltDataStore->tracks(m_input2TracksName));

  m_outputMuonTracks =
    m_patDataStore->createTrackContainer( m_outputMuonTracksName, 20 );
  m_states            = m_patDataStore->states();
  m_myState = new State();

 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode SpaceMuonMatch::execute() {

  debug() << "==> Execute" << endmsg;
  setFilterPassed(false);
  HltAlgorithm::beginExecute();
  debug()<<" pat "<<m_inputTracks->size()<<" hlt "<<m_input2Tracks->size()
        <<endreq;
  int tt=0;
  for ( std::vector<Track*>::const_iterator itT = m_inputTracks->begin();
        m_inputTracks->end() != itT; itT++ ) {
    
    
    Track* pTrack = (*itT);
    pTrack->setFlag(Track::PIDSelected,false);
    pTrack->setFlag(Track::L0Candidate,false);
    if( pTrack->checkFlag( Track::Backward ) ) continue; // skip backward tracks
    debug()<<" new pat track "<<tt<<endreq;
    tt++;
    Track* Ttrack;
    
    StatusCode sc=tryMatch(pTrack,Ttrack);
    
    if(StatusCode::SUCCESS == sc) {
      debug()<<" success "<<endreq;

      Track* outTr = m_outputMuonTracks->newEntry();
      outTr->copy(*pTrack);
      debug()<<" selected track q"<<endreq;
      m_myState=&(Ttrack->firstState()); 
      debug()<<" selected track qq"<<endreq;
      
      outTr->addToStates( *m_myState);  
       debug()<<" selected track qqq"<<endreq;
      float qoverp=calcP(pTrack,Ttrack);
      debug()<<" selected track qqqq"<<endreq;
      if(qoverp!=0)outTr->firstState().setQOverP(1.0/qoverp);   
      outTr->setHistory( Track::TrackMatching );
  
      outTr->addToAncestors( pTrack );
      outTr->addToAncestors( Ttrack );
      
      debug()<<" selected track qqqqale"<<endreq;

      setFilterPassed(true);


      
      m_outputTracks->push_back(outTr);

      pTrack->setFlag(Track::PIDSelected,true);
      pTrack->setFlag(Track::L0Candidate,true);
      debug()<<" selected track "<<endreq;
      
      if(m_debug){
        debug() << " Selected track" << endreq;
      }
      
      setFilterPassed(true);
    }
  }
  



  
  
  HltAlgorithm::endExecute();
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode SpaceMuonMatch::finalize() {

  debug() << "==> Finalize" << endmsg;

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================



StatusCode SpaceMuonMatch::tryMatch(Track* track,Track* & Ttrack)
{

  debug()<<" enter "<<track<<endreq;
  
  double trackDxDz = track->firstState().tx();
  double trackDyDz = track->firstState().ty();
  double trackX = track->firstState().x();
  double trackY = track->firstState().y();
  double trackZ = track->firstState().z();
 
 ;
 // double x_velo_mag = 0;
  double yVelo_mag= 0;
  double xVelo_mag= 0;
  //  float x_cut;
  
  //double  xmin = 99999.;
  xVelo_mag=trackDxDz*(z_magnet_center-trackZ)+trackX;
  
  
  for ( std::vector<Track*>::const_iterator itMuon = m_input2Tracks->begin();
        m_input2Tracks->end() != itMuon; itMuon++ ) {
  
  
    
    debug()<<" enter loop new muon "<<endreq;
    
    //  std::vector<Track* >::const_iterator pMuon;
    
    
    Track* muon=(*itMuon);
    float xT=muon->firstState().x();
    float yT=muon->firstState().y();
    float zT=muon->firstState().z();
    float slopexT=muon->firstState().tx();
    // float slopeyT=muon->firstState().ty();
  
  
    
    yVelo_mag = trackY +  (trackDyDz) * (zT - trackZ);
    float xMuon_magnet=slopexT*(z_magnet_center-zT)+xT;
    float x_dist=xVelo_mag - xMuon_magnet;
    float y_dist=yVelo_mag-yT;
    if(fabs(x_dist)<m_x_cut&&fabs(y_dist)<m_y_cut){
      Ttrack=muon;
      

      return StatusCode::SUCCESS;


      
    }
    
    
  }
  



  debug()<<" end of match"<<endreq;
  
  return StatusCode::FAILURE;
}

float SpaceMuonMatch::calcP(Track* velotrack,Track* Ttrack)
{
  double trackDxDz = velotrack->firstState().tx();
  //  double trackDyDz = track->firstState().ty();
  //double trackX = track->firstState().x();
  //double trackY = track->firstState().y();
  //double trackZ = track->firstState().z();
  double slopeX=Ttrack->firstState().tx();
  return m_ptkickConstant/(slopeX-trackDxDz);
  
}
