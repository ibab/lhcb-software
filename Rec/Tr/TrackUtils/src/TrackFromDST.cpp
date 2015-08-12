// Include files 

// local
#include "TrackFromDST.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackFromDST
//
// 2006-09-18 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackFromDST )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackFromDST::TrackFromDST( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_debugLevel(false)
{
  declareProperty( "TracksInContainer", 
                   m_tracksInContainer = LHCb::TrackLocation::Default );
}

//=============================================================================
// Destructor
//=============================================================================
TrackFromDST::~TrackFromDST() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackFromDST::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;
  
  m_debugLevel = msgLevel( MSG::DEBUG );
  
  // Map the track history flags to the corresponding output containers
  // ------------------------------------------------------------------
  m_map[ LHCb::Track::PatSeeding    ] = LHCb::TrackLocation::Seed;
  m_map[ LHCb::Track::PatVelo       ] = LHCb::TrackLocation::Velo; 
  m_map[ LHCb::Track::PatFastVelo   ] = LHCb::TrackLocation::Velo;
  m_map[ LHCb::Track::PatVeloTT     ] = LHCb::TrackLocation::VeloTT;
  m_map[ LHCb::Track::PatForward    ] = LHCb::TrackLocation::Forward;
  m_map[ LHCb::Track::PatMatch      ] = LHCb::TrackLocation::Match;
  m_map[ LHCb::Track::TsaTrack      ] = LHCb::TrackLocation::Seed;
  m_map[ LHCb::Track::PatDownstream ] = LHCb::TrackLocation::Downstream;
  // -- Upgrade types
  m_map[ LHCb::Track::PrForward     ] = LHCb::TrackLocation::Forward;
  m_map[ LHCb::Track::PrDownstream  ] = LHCb::TrackLocation::Downstream;
  m_map[ LHCb::Track::PrSeeding     ] = LHCb::TrackLocation::Seed;
  m_map[ LHCb::Track::PrMatch       ] = LHCb::TrackLocation::Match;
  m_map[ LHCb::Track::PrVeloUT      ] = LHCb::TrackLocation::VeloTT;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackFromDST::execute()
{  
  if ( m_debugLevel ) debug() << "==> Execute" << endmsg;
  
  // Get all the tracks from in input container
  // ------------------------------------------
  LHCb::Tracks* inTracks = get<LHCb::Tracks>( m_tracksInContainer );
  
  if ( m_debugLevel ) debug() << "# of Tracks in input container \""
                              << m_tracksInContainer
                              << "\" = " << inTracks -> size() << endmsg;
  
  // Separate the tracks according to their History flag
  // ---------------------------------------------------
  TracksMap tracksMap;
  tracksMap = classifyTracks( inTracks );
  
  // Output the tracks to their corresponding containers
  // ---------------------------------------------------
  for ( TracksMap::const_iterator itTMap = tracksMap.begin();
        itTMap != tracksMap.end(); ++itTMap ) {
    outputTracks( (*itTMap).second, m_map[(*itTMap).first] );
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Classify the input tracks according to their History
//=============================================================================
TracksMap TrackFromDST::classifyTracks( LHCb::Tracks*& inTracks )
{
  TracksMap tMap;

  for ( std::map<const LHCb::Track::History,std::string>::iterator
          iter = m_map.begin();
        iter != m_map.end(); ++iter ) {
    tMap[(*iter).first] = std::vector<LHCb::Track*>();
  }
  
  LHCb::Tracks::iterator iTrack = inTracks -> begin();
  for( ; iTrack != inTracks -> end(); ++iTrack ) { 
    TracksMap::iterator it = tMap.find( (*iTrack)->history() );
     if ( it != tMap.end() ) {
       (it->second).push_back( *iTrack );
     } else 
       // in for backward compatiblity, PatDownstream tracks were
       // called PatKShort tracks before
       if ( (*iTrack)->history() == LHCb::Track::PatKShort ) {
	 tMap[LHCb::Track::PatDownstream].push_back( *iTrack );
       } else {
         if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
           debug() << "Invalid track type " << (*iTrack)->history() << endmsg;
       }  
  }

  return tMap;
}

//=============================================================================
//  Output a given vector of tracks to its corresponding TES path
//=============================================================================
void TrackFromDST::outputTracks( const std::vector<LHCb::Track*>& tracks,
                                 const std::string path )
{  
  // Create and fill the output container
  // ------------------------------------
  LHCb::Tracks* tracksOutCont = new LHCb::Tracks();
  
  for( std::vector<LHCb::Track*>::const_iterator iTrack = tracks.begin();
       iTrack != tracks.end(); ++iTrack ) {
    tracksOutCont -> add( (*iTrack) -> cloneWithKey() );
  }
  
  // Store the tracks in the TES
  // ---------------------------
  put( tracksOutCont, path );
  
  if ( m_debugLevel )
    debug() << "Stored " << tracksOutCont -> size() 
            << " tracks in " << path << endmsg;  
}

//=============================================================================
