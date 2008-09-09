// $Id: SpaceMuonMatch.cpp,v 1.11 2008-09-09 06:37:05 graven Exp $
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
  , m_selections(*this)
{
 
 declareProperty("OutputMuonTracksName"   ,
                  m_outputMuonTracksName );
  m_selections.declareProperties();

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

  m_selections.retrieveSelections();
  m_selections.registerSelection();
  m_matchToolPointer=tool<IMatchTVeloTracks>( "MatchTVeloTracks" );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode SpaceMuonMatch::execute() {

  debug() << "==> Execute" << endmsg;
  debug()<<" velo "<<m_selections.input<1>()->size()<<" T track "<<m_selections.input<2>()->size()
        <<endreq;

  Tracks* muontracks = new Tracks();
  muontracks->reserve(50);
  put(muontracks,m_outputMuonTracksName);

  int tt=0;
  for ( std::vector<Track*>::const_iterator itT = m_selections.input<1>()->begin();
        m_selections.input<1>()->end() != itT; itT++ ) {
    
    Track* pTrack = (*itT);
    if( pTrack->checkFlag( Track::Backward ) ) continue; // skip backward tracks
    debug()<<" new velo track "<<tt<<endreq;

    for ( std::vector<Track*>::const_iterator itMuon = m_selections.input<2>()->begin();
          m_selections.input<2>()->end() != itMuon; itMuon++ ) {
      Track* muon=(*itMuon);
      Track* outputTrack=new Track();
      double x_dist, y_dist; 
      StatusCode sc=m_matchToolPointer->match3dVelo(*pTrack,*muon,*outputTrack, x_dist, y_dist);
      if(StatusCode::SUCCESS == sc) {
        debug() << "Matched " << endmsg;

       //Recalculate the xdist with 2d and the LHCBID
        SmartRefVector< Track > anceRZ=pTrack->ancestors();
        Track* ppConvert = (*(anceRZ.begin()));
        double x_dist2dRecalc= 99999999;
        StatusCode sc2 = m_matchToolPointer->match2dVelo(*ppConvert,*muon, x_dist2dRecalc);        

        //float x_dist2d = (*(anceRZ.begin()))->info(LHCb::HltEnums::Muon2DxDist,-1);
        //debug() << "xdist 2d from ancestor " << x_dist2d << endmsg;
        outputTrack->addInfo(hltInfoID("Muon2DxDist"),x_dist2dRecalc);
        outputTrack->addInfo(hltInfoID("Muon3DxDist"),x_dist);
        outputTrack->addInfo(hltInfoID("Muon3DyDist"),y_dist);
        double tDist = (*itMuon)->info(hltInfoID("MuonTdist"),-1);
        outputTrack->addInfo(hltInfoID("MuonTdist"),tDist );


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

        muontracks->insert(outputTrack); 
        m_selections.output()->push_back(outputTrack);
      }
            
    }
    tt++;
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



