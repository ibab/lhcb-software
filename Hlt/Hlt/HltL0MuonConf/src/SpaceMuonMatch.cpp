// $Id: SpaceMuonMatch.cpp,v 1.3 2007-06-25 20:59:31 hernando Exp $
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
  // declareProperty( "Input2TracksName", m_input2TracksName);
 
 declareProperty("OutputMuonTracksName"   ,
                  m_outputMuonTracksName );

}
//=======================================================x======================
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
  //  m_inputTracks2= &(m_hltDataStore->tracks(m_input2TracksName));

  m_outputMuonTracks =
    m_patDataStore->createTrackContainer( m_outputMuonTracksName, 20 );
  m_matchToolPointer=tool<IMatchTVeloTracks>( "MatchTVeloTracks" );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode SpaceMuonMatch::execute() {

  debug() << "==> Execute" << endmsg;
  setFilterPassed(false);
  debug()<<" pat "<<m_inputTracks->size()<<" hlt "<<m_inputTracks2->size()
        <<endreq;
  int tt=0;
  for ( std::vector<Track*>::const_iterator itT = m_inputTracks->begin();
        m_inputTracks->end() != itT; itT++ ) {
    
    
    Track* pTrack = (*itT);
//    pTrack->setFlag(Track::PIDSelected,false);
//    pTrack->setFlag(Track::L0Candidate,false);
    if( pTrack->checkFlag( Track::Backward ) ) continue; // skip backward tracks
    debug()<<" new pat track "<<tt<<endreq;
//if(pTrack->checkFlag(Track::L0Candidate))

    for ( std::vector<Track*>::const_iterator itMuon = m_inputTracks2->begin();
          m_inputTracks2->end() != itMuon; itMuon++ ) {
      Track* muon=(*itMuon);
      Track* outputTrack=new Track();
      
      StatusCode sc=m_matchToolPointer->match3dVelo(*pTrack,*muon,*outputTrack);
      debug()<<" dopo muon "<<sc<<endreq;      
      if(StatusCode::SUCCESS == sc) {
        Track* outTr = m_outputMuonTracks->newEntry();  
        outTr->copy(*outputTrack);
        setFilterPassed(true);
        if(muon->checkFlag(Track::L0Candidate)){     
          outTr->setFlag(Track::PIDSelected,true);
          outTr->setFlag(Track::L0Candidate,true);
        }else{
          outTr->setFlag(Track::PIDSelected,false);
          outTr->setFlag(Track::L0Candidate,false);
        }
        m_outputTracks->push_back(outTr);

      }
      delete outputTrack;       
            
    }
  }
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



