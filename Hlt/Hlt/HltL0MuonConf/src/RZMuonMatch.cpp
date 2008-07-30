// $Id: RZMuonMatch.cpp,v 1.8 2008-07-30 13:42:04 graven Exp $
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
  , m_selections(*this)
{
    m_selections.declareProperties();
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

  m_selections.retrieveSelections();
  m_selections.registerSelection();
  
  m_matchToolPointer=tool<IMatchTVeloTracks>( "MatchTVeloTracks" );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RZMuonMatch::execute() {

  debug() << "==> Execute" << endmsg;
  debug()<<" velo "   << m_selections.input<2>()->size()
         <<" T track "<< m_selections.input<1>()->size()
         <<endreq;
  
  for ( std::vector<Track*>::const_iterator itT = m_selections.input<2>()->begin();
        m_selections.input<2>()->end() != itT; itT++ ) {
    
    Track* pTr2d = (*itT);
    pTr2d->setFlag(Track::PIDSelected,false);
    pTr2d->setFlag(Track::L0Candidate,false);
    if( pTr2d->checkFlag( Track::Backward ) ) continue; // skip backward tracks
    debug()<<" new velo track "<<(itT-m_selections.input<2>()->begin())<<endreq;
    

    for ( std::vector<Track*>::const_iterator itMuon = m_selections.input<1>()->begin();
          m_selections.input<1>()->end() != itMuon; itMuon++ ) {  
      
      debug()<<" enter loop new muon "<<m_selections.input<1>()->size()<<endreq;
      Track* muon=(*itMuon);
      double x_dist;
      StatusCode sc=m_matchToolPointer->match2dVelo(*pTr2d,*muon, x_dist );
    
      if(StatusCode::SUCCESS == sc) {
        debug()<<" success "<<endreq;
        debug() << "x_dist " << x_dist << endmsg;     
        m_selections.output()->push_back(pTr2d);
              
        pTr2d->setFlag(Track::PIDSelected,true);
        pTr2d->addInfo(HltEnums::Muon2DxDist,x_dist); 

        
        if(m_debug) debug() << " Selected track" << endreq;
        
        //TODO ?? only first match is considered ???
        break;
      
      }
    }
  }  
  return StatusCode::SUCCESS;
}


