// Include files 

// local
#include "PatMatch.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatMatch
//
// 2007-02-07 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PatMatch )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatMatch::PatMatch( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_matchTool(NULL)
{
  declareProperty( "VeloInput"       , m_veloLocation  = LHCb::TrackLocation::Velo );
  declareProperty( "SeedInput"       , m_seedLocation  = LHCb::TrackLocation::Seed );
  declareProperty( "MatchOutput"     , m_matchLocation = LHCb::TrackLocation::Match );
  declareProperty( "ToolName"        , m_matchToolName = "PatMatchTool" );
}
//=============================================================================
// Destructor
//=============================================================================
PatMatch::~PatMatch() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatMatch::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  m_matchTool = tool<IMatchTool>( m_matchToolName, this);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatMatch::execute()
{
  LHCb::Tracks* matchs = new LHCb::Tracks;
  put(matchs, m_matchLocation);
  matchs->reserve(200);
 
  LHCb::Tracks* velos  = getIfExists<LHCb::Tracks>( m_veloLocation );
  LHCb::Tracks* seeds  = getIfExists<LHCb::Tracks>( m_seedLocation ); 
  
  if( velos == nullptr ){
    error() << "Could not find Velo tracks at: " << m_veloLocation << endmsg;
    return StatusCode::SUCCESS;
  }
  
  if( seeds == nullptr ){
    error() << "Could not find Seed tracks at: " << m_seedLocation << endmsg;
    return StatusCode::SUCCESS;
  }


  StatusCode sc = m_matchTool->match( *velos , *seeds , *matchs);
  
  if(sc.isFailure()) Warning("PatMatchTool failed",sc).ignore();
  
  return sc;
}
 
//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatMatch::finalize() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
