// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "Event/KalmanFitResult.h"

// from Loki
#include "KalmanFilter/FastVertex.h"

// local
#include "FstSelectForwardTracksPartTwo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FstSelectForwardTracksPartTwo
//
// 2012-10-09 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FstSelectForwardTracksPartTwo )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FstSelectForwardTracksPartTwo::FstSelectForwardTracksPartTwo( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputTracksName",        m_inputTracksName  = "Sft/Track/Forward" );
  declareProperty( "PVName",                 m_pvName           = "Sft/Vertex/Primary" );
  declareProperty( "OutputTracksName",       m_outputTracksName = "Sft/Track/ForwardFst" );
  declareProperty( "MinIP",                  m_minIP            = 0.100 * Gaudi::Units::mm );
  declareProperty( "MaxIP",                  m_maxIP            = 3.000 * Gaudi::Units::mm );
  declareProperty( "MinIPChi2",              m_minIPChi2        = 9. );
  declareProperty( "MaxIPChi2",              m_maxIPChi2        = -1 ); // negative deactivates the cut
  declareProperty( "MaxChi2Ndf",             m_maxChi2Ndf       = 2.0 ); // set negative to deactivate
  declareProperty( "DoNothing",              m_doNothing        = false);
}
//=============================================================================
// Destructor
//=============================================================================
FstSelectForwardTracksPartTwo::~FstSelectForwardTracksPartTwo() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FstSelectForwardTracksPartTwo::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  if (m_doNothing) {
    info() << "This algorithm is turned off via the DoNothing property." << endmsg;
    return StatusCode::SUCCESS;
  }

  m_minIP2 = m_minIP * m_minIP;
  m_maxIP2 = m_maxIP * m_maxIP;

  info() << "Input tracks    : " << m_inputTracksName  << endmsg
         << "Output tracks   : " << m_outputTracksName << endmsg
         << "MinIP           : " << m_minIP << endmsg
         << "MaxIP           : " << m_maxIP << endmsg
         << "MinIPChi2       : " << m_minIPChi2 << endmsg
         << "MaxIPChi2       : " << m_maxIPChi2 << endmsg
         << "MaxChi2Ndf      : " << m_maxChi2Ndf << endmsg;

  m_nTracks = 0;
  m_nPtOK   = 0;
  m_nChi2OK = 0;
  m_nIPOK   = 0;
  m_nIPSOK  = 0;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FstSelectForwardTracksPartTwo::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::Tracks* forward  = get<LHCb::Tracks>( m_inputTracksName );
  LHCb::RecVertices* pvs = get<LHCb::RecVertices>( m_pvName );
  LHCb::Tracks* selected = new LHCb::Tracks();
  put( selected, m_outputTracksName );

  if (m_doNothing) {
    // After bookkeeping is done, let the event pass. Also write tracks properly.
    
    for (LHCb::Tracks::iterator itT = forward->begin(); forward->end() != itT; ++itT) {
      LHCb::Track* track = (*itT)->clone();
      selected->add(track);
    }

    setFilterPassed(true);
    return StatusCode::SUCCESS;
  }
  
  for ( LHCb::Tracks::iterator itT = forward->begin();
        forward->end() != itT; ++itT ) {
    LHCb::Track* track = (*itT);
    if ( track->checkFlag( LHCb::Track::Invalid ) ) continue;
    if ( m_maxChi2Ndf > 0 ){
      if( NULL == track->fitResult() ) {
        continue;
      }
    }
    ++m_nTracks;
    if ( m_maxChi2Ndf > 0 ){
      if (track->chi2PerDoF() >= m_maxChi2Ndf) continue;
    }
    ++m_nChi2OK;
    float bestIP = 1.e9;
    float bestIPChi2 = 1.e9;
    
    for ( LHCb::RecVertices::const_iterator itPV = pvs->begin(); pvs->end() > itPV; ++itPV ) {
      LHCb::RecVertex* vertex = (*itPV);
      double impact = 0 ;
      double ipchi2 = 0 ;
      StatusCode sc = LoKi::FastVertex::distance(track, vertex, impact, ipchi2, true);

      if (impact < bestIP) bestIP = impact;
      //== if one PV with low IP -> reject.
      if (bestIP <= m_minIP) break;

      if (ipchi2 < bestIPChi2) bestIPChi2 = ipchi2;
      //== if one PV with low IPchi2 -> reject.
      if (bestIPChi2 <= m_minIPChi2) break;
    }
    if (bestIP > m_maxIP || bestIP <= m_minIP) continue;
    ++m_nIPOK;
    if (((bestIPChi2 < m_maxIPChi2)||(m_maxIPChi2 < 0)) &&
        bestIPChi2 > m_minIPChi2) {
      ++m_nIPSOK;
      LHCb::Track* goodTrack = track->clone();
      selected->add(goodTrack);
    }
  }
  setFilterPassed(0 != selected->size());
  
  debug() << "Selected " << selected->size() << " Forward tracks from " << pvs->size() << " PV." << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FstSelectForwardTracksPartTwo::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  if (m_doNothing) {
    return GaudiAlgorithm::finalize();
  }

  float eff1 = 100. * float( m_nPtOK )   / float( m_nTracks );
  float eff2 = 100. * float( m_nChi2OK ) / float( m_nTracks );
  float eff3 = 100. * float( m_nIPOK )   / float( m_nTracks );
  float eff4 = 100. * float( m_nIPSOK )  / float( m_nTracks );
  
  info() << format( "From %7d tracks:", m_nTracks ) << endmsg;
  info() << format( "      %6d with chi2 OK,   %7.2f%%", m_nChi2OK, eff2 ) << endmsg;
  info() << format( "      %6d with IP OK,     %7.2f%%", m_nIPOK,   eff3 ) << endmsg;
  info() << format( "      %6d with IPS OK,    %7.2f%%", m_nIPSOK,  eff4 ) << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
