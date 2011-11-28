// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Track.h"
#include "Event/RecVertex.h"

// local
#include "FastPVMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FastPVMonitor
//
// 2011-11-22 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FastPVMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FastPVMonitor::FastPVMonitor( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "MinIPForTrack",    m_minIPForTrack  = 0.150 * Gaudi::Units::mm );
  declareProperty( "MaxIPForTrack",    m_maxIPForTrack  = 3.000 * Gaudi::Units::mm );
}
//=============================================================================
// Destructor
//=============================================================================
FastPVMonitor::~FastPVMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FastPVMonitor::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_nEvent    = 0;
  m_nVertices = 0;
  m_nLargeIP  = 0;

  m_s0  = 0.;
  m_sx  = 0.;
  m_sy  = 0.;
  m_sz  = 0.;
  m_sx2 = 0.;
  m_sy2 = 0.;
  m_sz2 = 0.;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FastPVMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::Tracks* tracks   = get<LHCb::Tracks>( LHCb::TrackLocation::Velo );
  LHCb::RecVertices* pvs = get<LHCb::RecVertices>( LHCb::RecVertexLocation::Primary );

  m_nEvent    += 1;
  m_nVertices += pvs->size();
  LHCb::RecVertices::iterator itPv;
  for ( itPv = pvs->begin(); pvs->end() != itPv ; ++itPv ) {
    double xv = (*itPv)->position().x();
    double yv = (*itPv)->position().y();
    double zv = (*itPv)->position().z();
    m_s0  += 1.;
    m_sx  += xv;
    m_sy  += yv;
    m_sz  += zv;
    m_sx2 += xv * xv;
    m_sy2 += yv * yv;
    m_sz2 += zv * zv;
    if ( msgLevel( MSG::DEBUG ) ) {
           info() << format( "x%7.3f y%7.3f z%7.2f nTr%5d  errx%7.3f errY %7.3f errZ %7.3f",
                             xv, yv, zv, (*itPv)->tracks().size(),
                             sqrt((*itPv)->covMatrix()(0,0)),
                             sqrt((*itPv)->covMatrix()(1,1)),
                             sqrt((*itPv)->covMatrix()(2,2)) ) << endmsg;
    }
  }
    
  // Number of tracks with IP between min and max values
  for ( LHCb::Tracks::iterator itT = tracks->begin(); tracks->end() != itT ; ++itT ) {
    if ( (*itT)->checkFlag( LHCb::Track::Backward ) ) continue;
    Gaudi::XYZPoint point = (*itT)->position();
    Gaudi::XYZVector dir  = (*itT)->slopes();
    double lowestIP = 1.e9;
    //    LHCb::RecVertex* best = NULL;
    for (  itPv = pvs->begin(); pvs->end() != itPv ; ++itPv ) {
      double dx = point.x() + ( (*itPv)->position().z() - point.z() ) * dir.x() - (*itPv)->position().x();
      double dy = point.y() + ( (*itPv)->position().z() - point.z() ) * dir.y() - (*itPv)->position().y();
      double ip2 = dx*dx + dy*dy;
      if ( ip2 < lowestIP ) {
        lowestIP = ip2;
        // best = *itPv; // variable set but never used
      }
    }
    lowestIP  = sqrt( lowestIP );
    if ( lowestIP > m_minIPForTrack &&
         lowestIP < m_maxIPForTrack    ) {
      if ( msgLevel( MSG::DEBUG ) ) {
        info() << format( "Large IP Track %3d ip %7.3f ", (*itT)->key(), lowestIP ) 
               << endmsg;
      }
      m_nLargeIP++;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FastPVMonitor::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  info() << format( "Produced %7.3f vertices per event", float( m_nVertices) / float( m_nEvent ) ) << endmsg;
  double trackRate = double( m_nLargeIP ) / double ( m_nEvent );
  info() << format( "Number of large ( %5.3f to %5.3f mm) IP tracks/event = %7.2f",
                    m_minIPForTrack, m_maxIPForTrack, trackRate )
         << endmsg;

  double xm = m_sx / m_s0;
  double ym = m_sy / m_s0;
  double zm = m_sz / m_s0;
  info() << format( "  X position %7.3f sigma %7.3f", xm, sqrt( m_sx2/m_s0 - xm * xm ) ) << endmsg;
  info() << format( "  Y position %7.3f sigma %7.3f", ym, sqrt( m_sy2/m_s0 - ym * ym ) ) << endmsg;
  info() << format( "  Z position %7.3f sigma %7.3f", zm, sqrt( m_sz2/m_s0 - zm * zm ) ) << endmsg;
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
