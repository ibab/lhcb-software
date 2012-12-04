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

  m_nEvents = 0;
  m_nTotTracks = 0;
  m_goodIPTracks = 0;
  m_nSelTracks = 0;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FstSelectVeloTracks::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  ++m_nEvents;
  
  LHCb::Tracks* velo = get<LHCb::Tracks>( m_inputTracksName );
  LHCb::RecVertices* pvs = get<LHCb::RecVertices>( m_pvName );
  LHCb::Tracks* selected = new LHCb::Tracks();
  put( selected, m_outputTracksName );

  std::vector<LHCb::Track*> largeIP;
  
  for ( LHCb::Tracks::iterator itT = velo->begin();
        velo->end() != itT; ++itT ) {
    LHCb::Track* track = (*itT);
    if ( track->checkFlag( LHCb::Track::Invalid ) ) continue;
    if ( track->checkFlag( LHCb::Track::Backward ) ) continue;
    ++m_nTotTracks;
    if ( track->nLHCbIDs() < m_minVeloClusters ) continue;
    float bestIP2 = 1.e9;
    Gaudi::XYZPoint pos = track->position();
    float tx = track->slopes().x();
    float ty = track->slopes().y();
    float x0 = pos.x() - tx * pos.z();
    float y0 = pos.y() - ty * pos.z();
    float den2 = 1. + tx*tx + ty*ty;
    for ( LHCb::RecVertices::const_iterator itPV = pvs->begin(); pvs->end() > itPV; ++itPV ) {
      float xv = (*itPV)->position().x();
      float yv = (*itPV)->position().y();
      float zv = (*itPV)->position().z();
      float dx = x0 + zv * tx - xv;
      float dy = y0 + zv * ty - yv;
      float dist2 = (dx * dx + dy * dy) / den2;
      if ( dist2 < bestIP2 ) {
        bestIP2 = dist2;
        if ( bestIP2 < m_minIP2 ) break;
      }
    }
    if ( bestIP2 < m_maxIP2 &&
         bestIP2 > m_minIP2 ) {
      largeIP.push_back( track );
      ++m_goodIPTracks;
    }
  }

  for ( std::vector<LHCb::Track*>::iterator itT1 = largeIP.begin(); largeIP.end() != itT1; ++itT1 ) {
    if ( m_validateWithTT ) {
      if ( !m_validationTool->accept( **itT1 ) ) continue;
    }
    
    LHCb::Track* goodTrack = (*itT1)->clone();
    selected->add( goodTrack );
    ++m_nSelTracks;
  }

  setFilterPassed( 0 != selected->size() );
  
  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "Selected " << selected->size() << " Velo tracks from " << pvs->size() << " PV." << endmsg;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FstSelectVeloTracks::finalize() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  info() << format( "From %8d tracks:", m_nTotTracks ) << endmsg;
  info() << format( "     %8d tracks (%5.1f%%) with large IP",
                    m_goodIPTracks, 100. * float( m_goodIPTracks)/float( m_nTotTracks) ) << endmsg;
  info() << format( "     %8d tracks (%5.1f%%) selected",
                    m_nSelTracks, 100. * float( m_nSelTracks)/float( m_nTotTracks) ) << endmsg;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
