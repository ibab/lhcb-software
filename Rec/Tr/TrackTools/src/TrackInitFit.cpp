// Include files 

// from Gaudi
#include "TrackInitFit.h" 

// from TrackEvent
#include "Event/TrackFunctor.h"
#include "Event/StateParameters.h"
#include "Event/Track.h"
#include "Event/FitNode.h"
#include "Event/State.h"


//-----------------------------------------------------------------------------
// Implementation file for class : TrackInitFit
//
// 2009-11-14 : Kostyantyn Holubyev 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TrackInitFit )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackInitFit::TrackInitFit( const std::string& type,
                            const std::string& name,
                            const IInterface* parent)
  : base_class ( type, name , parent )
{
 declareInterface<ITrackFitter>(this);
 declareProperty("Init", m_initToolName = "TrackStateInitTool");
 declareProperty("Fit", m_fitToolName = "TrackMasterFitter");
}
//=============================================================================
// Destructor
//=============================================================================
TrackInitFit::~TrackInitFit() = default;

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackInitFit::initialize() {

  info() << "==> Initialize " << endmsg;
  
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  m_initTrack = tool<ITrackStateInit>( m_initToolName, "Init", this);
  m_fitTrack  = tool<ITrackFitter>(m_fitToolName, "Fit", this);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackInitFit::fit( LHCb::Track& track, LHCb::ParticleID pid ) const {
    
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute " << endmsg;
  
  // simply init
  StatusCode sc = m_initTrack->fit(track);
  if( sc.isFailure() ) 
    Warning("TrackStateInitTool failed",sc,0).ignore();

  // and fit
  return m_fitTrack->fit(track, pid);
}

//=============================================================================
