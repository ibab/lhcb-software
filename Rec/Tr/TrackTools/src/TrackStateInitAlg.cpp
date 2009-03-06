#include "TrackStateInitAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackStateInitAlg
//
// 2009-03-02 : Pavel Krokovny
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( TrackStateInitAlg ) ;

TrackStateInitAlg::TrackStateInitAlg( const std::string& name,
				      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("TrackLocation", m_trackLocation = LHCb::TrackLocation::Default);
  declareProperty("ClearStates", clearStates = true);
}

StatusCode TrackStateInitAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;
  m_trackTool = tool<ITrackStateInit>( "TrackStateInitTool" );
  return StatusCode::SUCCESS ;
}

TrackStateInitAlg::~TrackStateInitAlg() {}

StatusCode TrackStateInitAlg::execute()
{
  LHCb::Tracks* tracks = get<LHCb::Tracks>(m_trackLocation);
  for (LHCb::Tracks::const_iterator it = tracks->begin(); 
       tracks->end() != it; ++it) {
    LHCb::Track* track = *it;
    StatusCode sc = m_trackTool->fit(*track, clearStates);
    if ( sc.isFailure() ) return sc;
  }
  return StatusCode::SUCCESS ;
}

