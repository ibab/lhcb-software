// $Id: SpaceMuonMatch.cpp,v 1.8 2007-12-11 12:06:09 sandra Exp $
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

  m_matchToolPointer=tool<IMatchTVeloTracks>( "MatchTVeloTracks" );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode SpaceMuonMatch::execute() {

  debug() << "==> Execute" << endmsg;
  debug()<<" velo "<<m_inputTracks->size()<<" T track "<<m_inputTracks2->size()
        <<endreq;

  Tracks* muontracks = new Tracks();
  muontracks->reserve(50);

  int tt=0;
  for ( std::vector<Track*>::const_iterator itT = m_inputTracks->begin();
        m_inputTracks->end() != itT; itT++ ) {
    
    Track* pTrack = (*itT);
    if( pTrack->checkFlag( Track::Backward ) ) continue; // skip backward tracks
    debug()<<" new velo track "<<tt<<endreq;

    for ( std::vector<Track*>::const_iterator itMuon = m_inputTracks2->begin();
          m_inputTracks2->end() != itMuon; itMuon++ ) {
      Track* muon=(*itMuon);
      Track* outputTrack=new Track();
      float x_dist, y_dist; 
      StatusCode sc=m_matchToolPointer->match3dVelo(*pTrack,*muon,*outputTrack, x_dist, y_dist);
      if(StatusCode::SUCCESS == sc) {
        debug() << "Matched " << endmsg;

       //Recalculate the xdist with 2d and the LHCBID
        SmartRefVector< Track > anceRZ=pTrack->ancestors();
        Track* ppConvert = (*(anceRZ.begin()));
        float x_dist2dRecalc= 99999999;
        StatusCode sc2 = m_matchToolPointer->match2dVelo(*ppConvert,*muon, x_dist2dRecalc);        

        muontracks->insert(outputTrack); 
        setFilterPassed(true);
        //float x_dist2d = (*(anceRZ.begin()))->info(LHCb::HltEnums::Muon2DxDist,-1);
        //debug() << "xdist 2d from ancestor " << x_dist2d << endmsg;
        outputTrack->addInfo(HltNames::particleInfoID("Muon2DxDist"),x_dist2dRecalc);
        outputTrack->addInfo(HltNames::particleInfoID("Muon3DxDist"),x_dist);
        outputTrack->addInfo(HltNames::particleInfoID("Muon3DyDist"),y_dist);
        double tDist = (*itMuon)->info(LHCb::HltEnums::MuonTdist,-1);
        outputTrack->addInfo(HltNames::particleInfoID("MuonTdist"),tDist );


        debug() << "x_dist2d Recalc " << x_dist2dRecalc << endmsg;
        debug() << "x_dist " <<x_dist  << endmsg;
        debug() << "y_dist " <<y_dist  << endmsg;
        debug() << "tDist " <<tDist  << endmsg;


        // Add T track LHCBIDs
        std::vector< LHCb::LHCbID > list_lhcb=(*itMuon)->lhcbIDs();
        for(std::vector< LHCb::LHCbID >::iterator itLHCBID=list_lhcb.begin();
                                        itLHCBID<list_lhcb.end();itLHCBID++){
             outputTrack->addToLhcbIDs(*itLHCBID);
        }
       outputTrack->setType(Track::Long);
       // Add the qOverP for the states other than the firstState
       const std::vector< LHCb::State*>& allStates = outputTrack->states();
       debug() << " N states " << allStates.size() << endmsg;
       for(std::vector< LHCb::State* >::const_iterator itState=allStates.begin();itState<allStates.end();itState++){
          if((*itState)->qOverP() == 0)(*itState)->setQOverP(outputTrack->firstState().qOverP());
       }//for(std::vector< LHCb::State* >::iterator itState=allStates.begin()

        m_outputTracks->push_back(outputTrack);
      }
            
    }
    tt++;
  }  
//  HltAlgorithm::endExecute();
  put(muontracks,m_outputMuonTracksName);
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



