// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "Event/KalmanFitResult.h"

// local
#include "FstSelectForwardTracksPartOne.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FstSelectForwardTracksPartOne
//
// 2012-10-09 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FstSelectForwardTracksPartOne )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FstSelectForwardTracksPartOne::FstSelectForwardTracksPartOne( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputTracksName",        m_inputTracksName  = "Sft/Track/Forward" );
  declareProperty( "PVName",                 m_pvName           = "Sft/Vertex/Primary" );
  declareProperty( "OutputTracksName",       m_outputTracksName = "Sft/Track/ForwardFst" );
  declareProperty( "MinIP",                  m_minIP            = 0.100 * Gaudi::Units::mm );
  declareProperty( "MaxIP",                  m_maxIP            = 3.000 * Gaudi::Units::mm );
  declareProperty( "MinPt",                  m_minPt            = 1.500 * Gaudi::Units::GeV );
  declareProperty( "MinP",                   m_minP            = 1.500 * Gaudi::Units::GeV );
  declareProperty( "MaxChi2Ndf",             m_maxChi2Ndf       = 2.0 ); // set negative to deactivate
}
//=============================================================================
// Destructor
//=============================================================================
FstSelectForwardTracksPartOne::~FstSelectForwardTracksPartOne() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FstSelectForwardTracksPartOne::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_minIP2 = m_minIP * m_minIP;
  m_maxIP2 = m_maxIP * m_maxIP;

  info() << "Input tracks    : " << m_inputTracksName  << endmsg
         << "Output tracks   : " << m_outputTracksName << endmsg
         << "MinIP           : " << m_minIP << endmsg
         << "MaxIP           : " << m_maxIP << endmsg
         << "MinPt           : " << m_minPt << endmsg
         << "MinP            : " << m_minP << endmsg;

  m_nTracks = 0;
  m_nPtOK   = 0;
  m_nChi2OK = 0;
  m_nIPOK   = 0;
  m_nIPSOK  = 0;
  m_nInput  = 0;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FstSelectForwardTracksPartOne::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  LHCb::Tracks* forward  = get<LHCb::Tracks>( m_inputTracksName );
  LHCb::RecVertices* pvs = get<LHCb::RecVertices>( m_pvName );
  LHCb::Tracks* selected = new LHCb::Tracks();
  put( selected, m_outputTracksName );

  if (forward->size() > 0) {
    m_nInput++;
  }
  
  typedef LHCb::Track::LHCbIDContainer IDs;
  for ( LHCb::Tracks::iterator itT = forward->begin();
        forward->end() != itT; ++itT ) {
    LHCb::Track* track = (*itT);
    if ( track->checkFlag( LHCb::Track::Invalid ) ) continue;
    ++m_nTracks;

    // Minimum number of T station hits required
    const IDs& ids = track->lhcbIDs();
    int nIDs(0);
    for (IDs::const_iterator id=ids.begin(); ids.end() != id; ++id) {
      if (id->isIT()) {
        nIDs += 2;
      }
      else if (id->isOT()) {
        ++nIDs;
      }
    }
    if (nIDs <= 16) continue;

    if ( !((m_minPt < track->pt()) && (m_minP < track->p())) ) continue;
    ++m_nPtOK;
 
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
      if ( dist2   < bestIP2  ) bestIP2 = dist2;
      if ( bestIP2 < m_minIP2 ) break;               //== if one PV with low IP -> reject.
      }
    if ( bestIP2 > m_maxIP2 || bestIP2 < m_minIP2 ) continue;
    ++m_nIPOK;
    LHCb::Track* goodTrack = track->clone();
    selected->add( goodTrack );
  }
  setFilterPassed( 0 != selected->size() );
  
  debug() << "Selected " << selected->size() << " Forward tracks from " << pvs->size() << " PV." << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FstSelectForwardTracksPartOne::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  float eff1 = 100. * float( m_nPtOK )   / float( m_nTracks );
  float eff2 = 100. * float( m_nChi2OK ) / float( m_nTracks );
  float eff3 = 100. * float( m_nIPOK )   / float( m_nTracks );
  float eff4 = 100. * float( m_nIPSOK )  / float( m_nTracks );
  
  info() << format( "From %7d (%i) tracks:", m_nTracks, m_nInput ) << endmsg;
  info() << format( "      %6d with Pt&P OK,   %7.2f%%", m_nPtOK,   eff1 ) << endmsg;
  info() << format( "      %6d with IP OK,     %7.2f%%", m_nIPOK,   eff3 ) << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
