// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Track.h"
#include "Event/RecVertex.h"
// local
#include "FstSelectVeloTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FstSelectVeloTracks
//
// 2012-10-08 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FstSelectVeloTracks )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FstSelectVeloTracks::FstSelectVeloTracks( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_validationTool( 0 )
{
  declareProperty( "InputTracksName",        m_inputTracksName  = "Sft/Track/Velo" );
  declareProperty( "PVName",                 m_pvName           = "Sft/Vertex/Primary" );
  declareProperty( "OutputTracksName",       m_outputTracksName = "Sft/Track/VeloFst" );
  declareProperty( "MinVeloClusters",        m_minVeloClusters  = 8 );
  declareProperty( "MinIP",                  m_minIP            = 0.100 * Gaudi::Units::mm );
  declareProperty( "MaxIP",                  m_maxIP            = 3.000 * Gaudi::Units::mm );
  declareProperty( "ValidateWithTT",         m_validateWithTT   = false );
}
//=============================================================================
// Destructor
//=============================================================================
FstSelectVeloTracks::~FstSelectVeloTracks() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FstSelectVeloTracks::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_minIP2 = m_minIP * m_minIP;
  m_maxIP2 = m_maxIP * m_maxIP;

  info() << "Input tracks    : " << m_inputTracksName  << endmsg
         << "Output tracks   : " << m_outputTracksName << endmsg
         << "MinVeloClusters : " << m_minVeloClusters  << endmsg
         << "MinIP           : " << m_minIP << endmsg
         << "MaxIP           : " << m_maxIP << endmsg;
  if ( m_validateWithTT ) {
    m_validationTool = tool<ITrackSelector>( "FastTTValidationTool" );
    info() << "ValidateWithTT  is active" << endmsg;
  }
  

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FstSelectVeloTracks::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::Tracks* velo = get<LHCb::Tracks>( m_inputTracksName );
  LHCb::RecVertices* pvs = get<LHCb::RecVertices>( m_pvName );
  LHCb::Tracks* selected = new LHCb::Tracks();
  put( selected, m_outputTracksName );
  
  for ( LHCb::Tracks::iterator itT = velo->begin();
        velo->end() != itT; ++itT ) {
    LHCb::Track* track = (*itT);
    if ( track->checkFlag( LHCb::Track::Invalid ) ) continue;
    if ( track->checkFlag( LHCb::Track::Backward ) ) continue;
    if ( track->nLHCbIDs() < m_minVeloClusters ) continue;
    float bestIP2 = 1.e9;
    Gaudi::XYZPoint pos = track->position();
    float tx = track->slopes().x();
    float ty = track->slopes().y();
    float den2 = 1. + tx*tx + ty*ty;
    for ( LHCb::RecVertices::const_iterator itPV = pvs->begin(); pvs->end() > itPV; ++itPV ) {
      float xv = (*itPV)->position().x();
      float yv = (*itPV)->position().y();
      float zv = (*itPV)->position().z();
      
      float dx = pos.x() + (zv - pos.z()) * tx - xv;
      float dy = pos.y() + (zv - pos.z()) * ty - yv;
      float dist2 = (dx * dx + dy * dy) / den2;
      if ( dist2 < m_maxIP2 ) {
        if ( dist2 < bestIP2 ) bestIP2 = dist2;
      }
    }
    if ( bestIP2 < m_maxIP2 &&
         bestIP2 > m_minIP2 ) {
      if ( m_validateWithTT ) {
        if ( !m_validationTool->accept( *track ) ) continue;
      }
      LHCb::Track* goodTrack = track->clone();
      selected->add( goodTrack );
    }
  }
  setFilterPassed( 0 != selected->size() );
  
  debug() << "Selected " << selected->size() << " Velo tracks from " << pvs->size() << " PV." << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FstSelectVeloTracks::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
