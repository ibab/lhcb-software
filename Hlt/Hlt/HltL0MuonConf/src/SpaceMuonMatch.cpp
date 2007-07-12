// $Id: SpaceMuonMatch.cpp,v 1.4 2007-07-12 17:45:06 asatta Exp $
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
 //declareProperty( "Input2TracksName", m_input2TracksName);
 
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
//  m_input2Tracks= &(m_hltDataStore->tracks(m_input2TracksName));

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
//  setFilterPassed(false);
//  HltAlgorithm::beginExecute();
  debug()<<" pat "<<m_inputTracks->size()<<" hlt "<<m_inputTracks2->size()
        <<endreq;
  int tt=0;
  for ( std::vector<Track*>::const_iterator itT = m_inputTracks->begin();
        m_inputTracks->end() != itT; itT++ ) {
    
    Track* pTrack = (*itT);
    if( pTrack->checkFlag( Track::Backward ) ) continue; // skip backward tracks
    debug()<<" new pat track "<<tt<<endreq;

    for ( std::vector<Track*>::const_iterator itMuon = m_inputTracks2->begin();
          m_inputTracks2->end() != itMuon; itMuon++ ) {
      Track* muon=(*itMuon);
      Track* outputTrack=new Track();
      float x_dist, y_dist; 
      StatusCode sc=m_matchToolPointer->match3dVelo(*pTrack,*muon,*outputTrack, x_dist, y_dist);
      debug()<<" dopo muon "<<sc<<endreq;      
      if(StatusCode::SUCCESS == sc) {
        debug() << "Matched " << endmsg;

       //Recalculate the xdist with 2d and the LHCBID
        SmartRefVector< Track > anceRZ=pTrack->ancestors();
        Track* ppConvert = (*(anceRZ.begin()));
        float x_dist2dRecalc= 99999999;
        StatusCode sc2 = m_matchToolPointer->match2dVelo(*ppConvert,*muon, x_dist2dRecalc);        
        if (sc2 == StatusCode::SUCCESS ){
            debug() << "x_dist2d recalculated " << x_dist2dRecalc << endmsg;
        }
        else{
            debug() << "Not matched with 2d " << endmsg;
        }

        Track* outTr = m_outputMuonTracks->newEntry();  
        outTr->copy(*outputTrack);
        setFilterPassed(true);
        //float x_dist2d = (*(anceRZ.begin()))->info(LHCb::HltEnums::Muon2DxDist,-1);
        //debug() << "xdist 2d from ancestor " << x_dist2d << endmsg;
        //outTr->addInfo(HltNames::particleInfoID("Muon2DxDist"),x_dist2d);
        outTr->addInfo(HltNames::particleInfoID("Muon2DxDist"),x_dist2dRecalc);
        outTr->addInfo(HltNames::particleInfoID("Muon3DxDist"),x_dist);
        outTr->addInfo(HltNames::particleInfoID("Muon3DyDist"),y_dist);

        std::vector< LHCb::LHCbID > list_lhcb=(*itMuon)->lhcbIDs();

        for(std::vector< LHCb::LHCbID >::iterator iM1=list_lhcb.begin();iM1<list_lhcb.end();iM1++){
          if(iM1->isMuon()){
             outTr->addToLhcbIDs(iM1->muonID());
//             debug()<<" adding tile "<<iM1->muonID()<<iM1->muonID().station()<<endreq;
//             debug()<<"station " << iM1->muonID().station()<<" is from L0 "<<muon->checkFlag(Track::L0Candidate) <<endreq;

          }
        }


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
//  HltAlgorithm::endExecute();
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



