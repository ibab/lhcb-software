#include "TrackStateInitAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackStateInitAlg
//
// 2009-03-02 : Pavel Krokovny
//-----------------------------------------------------------------------------

#ifdef _WIN32
#pragma warning ( disable : 4355 ) // This used in initializer list, needed for ToolHandles
#endif

DECLARE_ALGORITHM_FACTORY( TrackStateInitAlg )

TrackStateInitAlg::TrackStateInitAlg( const std::string& name,
				      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_trackTool("TrackStateInitTool",this)
{
  declareProperty("TrackLocation", m_trackLocation = LHCb::TrackLocation::Default);
  declareProperty("ClearStates", clearStates = true);
  declareProperty("StateInitTool", m_trackTool) ;
}

StatusCode TrackStateInitAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;
  sc = m_trackTool.retrieve() ;
  return sc ;
}

StatusCode TrackStateInitAlg::finalize() {
  m_trackTool.release().ignore() ;
  return GaudiAlgorithm::finalize() ;
}

TrackStateInitAlg::~TrackStateInitAlg() {}

StatusCode TrackStateInitAlg::execute()
{
  LHCb::Tracks* tracks = get<LHCb::Tracks>(m_trackLocation);
  for (LHCb::Tracks::const_iterator it = tracks->begin(); 
       tracks->end() != it; ++it) {
    LHCb::Track* track = *it;
    StatusCode sc = m_trackTool->fit(*track, clearStates);
    if( sc.isFailure() ) {
      // Warn any downstream code that this track is bad
      track->setFitStatus( LHCb::Track::FitFailed ) ;
      Warning("TrackStateInitTool fit failed",sc,0).ignore();
    }
  }
  return StatusCode::SUCCESS ;
}

