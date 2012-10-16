// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "Event/KalmanFitResult.h"

// local
#include "FstSelectForwardTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FstSelectForwardTracks
//
// 2012-10-09 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FstSelectForwardTracks )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FstSelectForwardTracks::FstSelectForwardTracks( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputTracksName",        m_inputTracksName  = "Sft/Track/Forward" );
  declareProperty( "PVName",                 m_pvName           = "Sft/Vertex/Primary" );
  declareProperty( "OutputTracksName",       m_outputTracksName = "Sft/Track/ForwardFst" );
  declareProperty( "MinIP",                  m_minIP            = 0.100 * Gaudi::Units::mm );
  declareProperty( "MaxIP",                  m_maxIP            = 3.000 * Gaudi::Units::mm );
  declareProperty( "MinIPChi2",              m_minIPChi2        = 9. );
  declareProperty( "MaxIPChi2",              m_maxIPChi2        = 100000. );
  declareProperty( "MinPt",                  m_minPt            = 1.500 * Gaudi::Units::GeV );
  declareProperty( "MaxChi2Ndf",             m_maxChi2Ndf            = 2.0 ); // set negative to deactivate
}
//=============================================================================
// Destructor
//=============================================================================
FstSelectForwardTracks::~FstSelectForwardTracks() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FstSelectForwardTracks::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_minIP2 = m_minIP * m_minIP;
  m_maxIP2 = m_maxIP * m_maxIP;

  info() << "Input tracks    : " << m_inputTracksName  << endmsg
         << "Output tracks   : " << m_outputTracksName << endmsg
         << "MinIP           : " << m_minIP << endmsg
         << "MaxIP           : " << m_maxIP << endmsg
         << "MinIPChi2       : " << m_minIPChi2 << endmsg
         << "MaxIPChi2       : " << m_maxIPChi2 << endmsg
         << "MinPt           : " << m_minPt << endmsg
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
StatusCode FstSelectForwardTracks::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  LHCb::Tracks* forward  = get<LHCb::Tracks>( m_inputTracksName );
  LHCb::RecVertices* pvs = get<LHCb::RecVertices>( m_pvName );
  LHCb::Tracks* selected = new LHCb::Tracks();
  put( selected, m_outputTracksName );
  
  for ( LHCb::Tracks::iterator itT = forward->begin();
        forward->end() != itT; ++itT ) {
    ++m_nTracks;
    LHCb::Track* track = (*itT);
    if ( track->checkFlag( LHCb::Track::Invalid ) ) continue;
    if ( m_minPt > track->pt() ) continue;
    ++m_nPtOK;
    if ( m_maxChi2Ndf > 0 ){
      if( NULL != track->fitResult() ) {
        if (track->chi2()/track->nDoF() > m_maxChi2Ndf) continue;
      } else {
        continue;
      }
    }
    ++m_nChi2OK;
    float bestIP2 = 1.e9;
    float bestIPChi2 = 1.e9;
    
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

      //== Compute IPChi2 : compute z of best Chi2, error takes into account the PV error
      float wx2 = 1. / ( track->firstState().errX2() + (*itPV)->covMatrix()(0,0));
      float wy2 = 1. / ( track->firstState().errY2() + (*itPV)->covMatrix()(1,1));
      float num = tx * ( pos.x() - xv ) * wx2 + ty * (pos.y() - yv ) * wy2;
      float den = tx * tx * wx2 + ty * ty * wy2;
      float zMin = num/den;
      dx = pos.x() + (zMin - pos.z()) * tx - xv;
      dy = pos.y() + (zMin - pos.z()) * ty - yv;
      float ipChi2 = dx * dx * wx2 + dy * dy * wy2;
      if ( ipChi2 < m_maxIPChi2 ) {
        if ( ipChi2 < bestIPChi2 ) bestIPChi2 = ipChi2;
      }
    }
    if ( bestIP2 > m_maxIP2 || bestIP2 < m_minIP2 ) continue;
    ++m_nIPOK;
    if ( bestIPChi2 < m_maxIPChi2 &&
         bestIPChi2 > m_minIPChi2    ) {
      ++m_nIPSOK;
      LHCb::Track* goodTrack = track->clone();
      selected->add( goodTrack );
    }
  }
  setFilterPassed( 0 != selected->size() );
  
  debug() << "Selected " << selected->size() << " Forward tracks from " << pvs->size() << " PV." << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FstSelectForwardTracks::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  float eff1 = 100. * float( m_nPtOK )   / float( m_nTracks );
  float eff2 = 100. * float( m_nChi2OK ) / float( m_nTracks );
  float eff3 = 100. * float( m_nIPOK )   / float( m_nTracks );
  float eff4 = 100. * float( m_nIPSOK )  / float( m_nTracks );
  
  info() << format( "From %7d tracks:", m_nTracks ) << endmsg;
  info() << format( "      %6d with Pt OK,     %7.2f%%", m_nPtOK,   eff1 ) << endmsg;
  info() << format( "      %6d with chi2 OK,   %7.2f%%", m_nChi2OK, eff2 ) << endmsg;
  info() << format( "      %6d with IP OK,     %7.2f%%", m_nIPOK,   eff3 ) << endmsg;
  info() << format( "      %6d with IPS OK,    %7.2f%%", m_nIPSOK,  eff4 ) << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
