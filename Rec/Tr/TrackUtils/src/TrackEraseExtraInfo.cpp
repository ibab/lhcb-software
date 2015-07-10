
// Gaudi
#include "GaudiKernel/AlgFactory.h"

// track interfaces
#include "Event/Track.h"

#include "TrackEraseExtraInfo.h"


using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackEraseExtraInfo )
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackEraseExtraInfo::TrackEraseExtraInfo(const std::string& name,
					 ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  
  std::vector<int> tmp = {
    LHCb::Track::PatQuality,
    LHCb::Track::Cand1stQPat, 
    LHCb::Track::Cand2ndQPat, 
    LHCb::Track::Cand1stChi2Mat,
    LHCb::Track::Cand2ndChi2Mat, 
    LHCb::Track::MatchChi2,
    LHCb::Track::TsaLikelihood, 
    LHCb::Track::nPRVeloRZExpect,
    LHCb::Track::nPRVelo3DExpect
  };
  
  
  declareProperty("InputLocation",  m_inputLocation  = TrackLocation::Default);
  declareProperty("ErasableInfo",   m_erasableInfo   = tmp);
  declareProperty("PrintExtraInfo", m_printExtraInfo = false);

}
//=============================================================================
// Destructor
//=============================================================================
TrackEraseExtraInfo::~TrackEraseExtraInfo()
{
  // destructor
}
//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackEraseExtraInfo::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( UNLIKELY(msgLevel(MSG::DEBUG)) ) debug() << "==> initialize" << endmsg;
  
  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackEraseExtraInfo::execute(){

  Tracks* inCont = getIfExists<Tracks>(m_inputLocation);
  if( inCont == nullptr){
    Warning( "Input container "+m_inputLocation+" does not exist", StatusCode::SUCCESS, 20 );
    return StatusCode::SUCCESS;
  }
  
  // -- Print extra info which is on track
  if( UNLIKELY( m_printExtraInfo )){

    for( LHCb::Track* track : *inCont){

      info() << "ExtraInfo for track: " << track->type() << " : " << track->key() << endmsg;

      const LHCb::Track::ExtraInfo extraInfo = track->extraInfo();
      for ( LHCb::Track::ExtraInfo::const_iterator i = extraInfo.begin();
            i != extraInfo.end(); ++i ){
        const LHCb::Track::AdditionalInfo addInfo =
          static_cast<LHCb::Track::AdditionalInfo>(i->first);
        info() << " " << addInfo << "=" << i->second << endmsg;
      }
    }
  }
  // --
      
  // -- loop 
  for (Tracks::iterator iterT = inCont->begin(); iterT != inCont->end(); ++iterT) {
    for(int i : m_erasableInfo) (*iterT)->eraseInfo( i );
  }
  
  return StatusCode::SUCCESS;
  
}

