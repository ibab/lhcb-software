// $Id: TrackInitFit.cpp,v 1.2 2009-11-14 16:20:57 kholubye Exp $
// Include files 

// from Gaudi
#include "TrackInitFit.h" 


//-----------------------------------------------------------------------------
// Implementation file for class : TrackInitFit
//
// 2009-11-14 : Kostyantyn Holubyev 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TrackInitFit );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackInitFit::TrackInitFit( const std::string& type,
                            const std::string& name,
                            const IInterface* parent)
  : GaudiTool ( type, name , parent )
    , m_initTrack(0)
    , m_fitTrack(0)
{
 declareInterface<ITrackFitter>(this);
 declareProperty("Init", m_initToolName = "TrackStateInitTool");
 declareProperty("Fit", m_fitToolName = "TrackMasterFitter");
}
//=============================================================================
// Destructor
//=============================================================================
TrackInitFit::~TrackInitFit() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackInitFit::initialize() {

  info() << "==> Initialize " << endreq;
  
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  m_initTrack = tool<ITrackStateInit>( m_initToolName, "Init", this);
  m_fitTrack  = tool<ITrackFitter>(m_fitToolName, "Fit", this);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackInitFit::fit( LHCb::Track& track, LHCb::ParticleID pid ) {
    
  if (msgLevel( MSG::DEBUG )) 
    debug() << "==> Execute " << endreq;
  
  // simply init
  StatusCode sc = m_initTrack->fit(track);
  if( sc.isFailure() ) 
    Warning("TrackStateInitTool failed",sc,0).ignore();

  // and fit
  sc = m_fitTrack->fit(track, pid);
  
  return sc;
  
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackInitFit::finalize() {

  return GaudiTool::finalize();  // must be called after all other actions
}

//=============================================================================
