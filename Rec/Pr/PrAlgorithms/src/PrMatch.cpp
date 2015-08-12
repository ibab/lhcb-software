// Include files 

// local
#include "PrMatch.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrMatch
//
// 2013-11-15 : Michel De Cian, migration to Upgrade
//
// 2007-02-07 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PrMatch )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrMatch::PrMatch( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_matchTool(NULL)
{
  declareProperty( "VeloInput"       , m_veloLocation  = LHCb::TrackLocation::Velo );
  declareProperty( "SeedInput"       , m_seedLocation  = LHCb::TrackLocation::Seed );
  declareProperty( "MatchOutput"     , m_matchLocation = LHCb::TrackLocation::Match );
}
//=============================================================================
// Destructor
//=============================================================================
PrMatch::~PrMatch() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrMatch::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  m_matchTool = tool<IMatchTool>("PrMatchTool", this);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrMatch::execute()
{
  LHCb::Tracks* matchs = new LHCb::Tracks;
  put(matchs, m_matchLocation);
  matchs->reserve(200);
 
  LHCb::Tracks* velos  = getIfExists<LHCb::Tracks>( m_veloLocation );
  if ( nullptr == velos ) {
    error() << "Track container '" << m_veloLocation << "' does not exist" <<endmsg;
    return StatusCode::SUCCESS;
  }
  
  LHCb::Tracks* seeds  = getIfExists<LHCb::Tracks>( m_seedLocation ); 
  if ( nullptr == seeds ) {
    error() << "Track container '" << m_seedLocation << "' does not exist" <<endmsg;
    return StatusCode::SUCCESS;
  }

  StatusCode sc = m_matchTool->match( *velos , *seeds , *matchs);
  
  if(sc.isFailure()) Warning("PrMatchTool failed",sc).ignore();
  
  return sc;
}
 
//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrMatch::finalize() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
