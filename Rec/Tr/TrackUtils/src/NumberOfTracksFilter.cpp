// Include files 

#include "Event/Track.h" 
// local
#include "NumberOfTracksFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : NumberOfTracksFilter
//
// 2008-03-05 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( NumberOfTracksFilter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
NumberOfTracksFilter::NumberOfTracksFilter( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("TrackLocations", m_tracksPath );
  declareProperty("MinTracks", m_minTracks = 0 );
  declareProperty("MaxTracks", m_maxTracks = -1 );
}
//=============================================================================
// Destructor
//=============================================================================
NumberOfTracksFilter::~NumberOfTracksFilter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode NumberOfTracksFilter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (msgLevel(MSG::DEBUG)) debug()  << "==> Initialize" << endmsg;

  if ( m_tracksPath.empty() ){
    if ( context() != "HLT" ) m_tracksPath.push_back( LHCb::TrackLocation::Default ) ;
  }
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Tracks will be taken from " ;
    for ( std::vector<std::string>::const_iterator t = m_tracksPath.begin() ;
          t!=m_tracksPath.end() ; ++t)  debug() << *t << " " ;
    debug() << endmsg ;
    debug() << "Will require at least " << m_minTracks << " tracks" << endmsg ;
    if ( m_maxTracks > -1 ) debug() << "Will require at most " << m_maxTracks << " tracks" << endmsg ;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode NumberOfTracksFilter::execute() {

  if (msgLevel(MSG::VERBOSE)) verbose()  << "==> Execute" << endmsg;

  int nT = 0 ;

  for ( std::vector<std::string>::const_iterator p = m_tracksPath.begin() ;
        p!=m_tracksPath.end() ; ++p) {

    LHCb::Track::Container* inTracks = getIfExists< LHCb::Track::Container>(*p);
    if ( NULL == inTracks ){
      Warning("No tracks at "+(*p),1).ignore();
    } else {
      if (msgLevel(MSG::VERBOSE)) verbose() << "Container " << *p << " contains "
                                            << inTracks->size() << " Tracks" << endmsg ;
      nT += inTracks->size() ;
    }
  }
  if (msgLevel(MSG::DEBUG)) debug() << "Found " << nT << " tracks" << endmsg ;

  if ( nT < m_minTracks ) setFilterPassed(false); // bad
  else if ( ( m_maxTracks > -1 ) & ( nT > m_maxTracks )) setFilterPassed(false); // bad
  else setFilterPassed(true);// good

  return StatusCode::SUCCESS;
}
