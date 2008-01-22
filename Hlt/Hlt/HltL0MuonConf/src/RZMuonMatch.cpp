// $Id: RZMuonMatch.cpp,v 1.7 2008-01-22 09:58:06 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Event/HltEnums.h"
// local
#include "RZMuonMatch.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RZMuonMatch
//
// 2007-02-21 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RZMuonMatch );
using namespace LHCb;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RZMuonMatch::RZMuonMatch( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
RZMuonMatch::~RZMuonMatch() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode RZMuonMatch::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  
  m_matchToolPointer=tool<IMatchTVeloTracks>( "MatchTVeloTracks" );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RZMuonMatch::execute() {

  debug() << "==> Execute" << endmsg;
//  setFilterPassed(false);
//  HltAlgorithm::beginExecute();
  debug()<<"velo " << m_inputTracks2->size()<<"T track "<<m_inputTracks->size()
        <<endreq;
  int tt=0;
  
  for ( std::vector<Track*>::const_iterator itT = m_inputTracks2->begin();
        m_inputTracks2->end() != itT; itT++ ) {
    
    Track* pTr2d = (*itT);
    pTr2d->setFlag(Track::PIDSelected,false);
    pTr2d->setFlag(Track::L0Candidate,false);
    if( pTr2d->checkFlag( Track::Backward ) ) continue; // skip backward tracks
    debug()<<" new velo track "<<tt<<endreq;
    tt++;
    

    for ( std::vector<Track*>::const_iterator itMuon = m_inputTracks->begin();
          m_inputTracks->end() != itMuon; itMuon++ ) {  
      
      debug()<<" enter loop new muon "<<m_inputTracks->size()<<endreq;
      Track* muon=(*itMuon);
      double x_dist;
      StatusCode sc=m_matchToolPointer->match2dVelo(*pTr2d,*muon, x_dist );
    
      if(StatusCode::SUCCESS == sc) {
        debug()<<" success "<<endreq;
        debug() << "x_dist " << x_dist << endmsg;     
        m_outputTracks->push_back(pTr2d);
              
        pTr2d->setFlag(Track::PIDSelected,true);
        pTr2d->addInfo(HltEnums::Muon2DxDist,x_dist); 

        
        if(m_debug){
          debug() << " Selected track" << endreq;
        }
        
        setFilterPassed(true);
        break;
      
      }
    }
    tt++;
  }  
  
//  HltAlgorithm::endExecute();
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RZMuonMatch::finalize() {

  debug() << "==> Finalize" << endmsg;

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================


