// Include files

// from Gaudi
#include "Event/RecVertex.h"
#include "Event/MCVertex.h"
// local
#include "VertexChecker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VertexChecker
//
// 2011-11-16 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VertexChecker )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  VertexChecker::VertexChecker( const std::string& name,
                                ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputLocation",    m_inputLocation  = LHCb::RecVertexLocation::Primary );
  declareProperty( "DeltaZForMatch",   m_deltaZForMatch = 1.000 * Gaudi::Units::mm );
  declareProperty( "MaxRadius",        m_maxRadius      = 3.000 * Gaudi::Units::mm );
  declareProperty( "MinIPForTrack",    m_minIPForTrack  = 0.150 * Gaudi::Units::mm );
  declareProperty( "MaxIPForTrack",    m_maxIPForTrack  = 3.000 * Gaudi::Units::mm );
  declareProperty( "NbBin",            m_nbBin          = 10 );
  declareProperty( "BinSizeMC",        m_binSizeMC      = 20 );
  declareProperty( "BinSizeRec",       m_binSizeRec     = 5  );
}
//=============================================================================
// Destructor
//=============================================================================
VertexChecker::~VertexChecker() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode VertexChecker::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_mcVertices.resize(  m_nbBin, 0 );
  m_mcFound.resize(     m_nbBin, 0 );
  m_recVertices.resize( m_nbBin, 0 );
  m_recFake.resize(     m_nbBin, 0 );

  m_s0  = 0.;
  m_sx  = 0.;
  m_sx2 = 0.;
  m_sy  = 0.;
  m_sy2 = 0.;
  m_sz  = 0.;
  m_sz2 = 0.;
  m_nbLargeIP = 0;
  m_nEvent = 0;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VertexChecker::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  bool debug = msgLevel( MSG::DEBUG );

  LHCb::MCVertices* mcVertices = get<LHCb::MCVertices>(LHCb::MCVertexLocation::Default );
  LHCb::RecVertices* myVertices = get<LHCb::RecVertices>(LHCb::RecVertexLocation::Primary );

  LHCb::Tracks* veloTracks = get<LHCb::Tracks>(LHCb::TrackLocation::Velo );

  std::vector<LHCb::MCVertex*> mcPvs;

  for ( LHCb::MCVertices::iterator itV = mcVertices->begin(); mcVertices->end() != itV; ++itV ) {
    if ( (*itV)->isPrimary() ) {
      if ( (*itV)->products().size() < 3 ) continue;
      if ( (*itV)->position().rho() > m_maxRadius ) continue;
      mcPvs.push_back( *itV );
    }
  }
  std::sort( mcPvs.begin(), mcPvs.end(), LowerByZ() );

  std::vector<LHCb::MCVertex*>::iterator itMc = mcPvs.begin();
  LHCb::RecVertices::iterator itPv = myVertices->begin();

  while ( itMc != mcPvs.end() || itPv != myVertices->end() ) {
    if ( itMc != mcPvs.end() && itPv != myVertices->end() ) {
      double dx = (*itMc)->position().x() - (*itPv)->position().x();
      double dy = (*itMc)->position().y() - (*itPv)->position().y();
      double dz = (*itMc)->position().z() - (*itPv)->position().z();
      if ( fabs( dz ) < m_deltaZForMatch ) {
        if ( debug )  info() << format ( " === Matched   dx%7.3f dy%7.3f  z%7.3f  dz%7.3f",
                                         dx, dy, (*itMc)->position().z(), dz ) << endmsg;
        int n = mcBin( *itMc );
        m_mcVertices[n]++;
        m_mcFound[n]++;
        int m = recBin( *itPv );
        m_recVertices[m]++;
        ++itMc;
        ++itPv;
        m_s0  += 1.;
        m_sx  += dx;
        m_sx2 += dx * dx;
        m_sy  += dy;
        m_sy2 += dy * dy;
        m_sz  += dz;
        m_sz2 += dz * dz;

      } else if ( dz > 0 ) {
        if ( (*itPv)->position().rho() < m_maxRadius ) {
          if ( debug ) info() << format( " +++ fake reco  x%7.3f  y%7.3f  z%7.3f   n %3d",
                                         (*itPv)->position().x(),
                                         (*itPv)->position().y(),
                                         (*itPv)->position().z(),
                                         (*itPv)->tracks().size() ) << endmsg;
          int m = recBin( *itPv );
          m_recVertices[m]++;
          m_recFake[m]++;
        }
        ++itPv;
      } else {
        if ( debug ) info() << format( " --- miss reco  x%7.3f  y%7.3f  z%7.3f   n %3d",
                                       (*itMc)->position().x(),
                                       (*itMc)->position().y(),
                                       (*itMc)->position().z(),
                                       (*itMc)->products().size() ) << endmsg;
        int n = mcBin( *itMc );
        m_mcVertices[n]++;
        ++itMc;
      }
    } else if (  itMc != mcPvs.end() ) {
      if ( debug ) info() << format( " --- miss reco  x%7.3f  y%7.3f  z%7.3f   n %3d",
                                     (*itMc)->position().x(),
                                     (*itMc)->position().y(),
                                     (*itMc)->position().z(),
                                     (*itMc)->products().size() ) << endmsg;
      int n = mcBin( *itMc );
      m_mcVertices[n]++;
      ++itMc;
    } else if ( itPv != myVertices->end() ) {
      if ( (*itPv)->position().rho() < m_maxRadius ) {
        if ( debug ) info() << format( " +++ fake reco  x%7.3f  y%7.3f  z%7.3f   n %3d",
                                       (*itPv)->position().x(),
                                       (*itPv)->position().y(),
                                       (*itPv)->position().z(),
                                       (*itPv)->tracks().size() ) << endmsg;
        int m = recBin( *itPv );
        m_recVertices[m]++;
        m_recFake[m]++;
      } else {
        info() << format( " ??? vertex at large rho   x%7.3f  y%7.3f  z%7.3f   n %3d",
                          (*itPv)->position().x(),
                          (*itPv)->position().y(),
                          (*itPv)->position().z(),
                          (*itPv)->tracks().size() ) << endmsg;
      }
      ++itPv;
    }
  }

  // Number of tracks with IP between min and max values
  for ( LHCb::Tracks::iterator itT = veloTracks->begin(); veloTracks->end() != itT ; ++itT ) {
    if ( (*itT)->checkFlag( LHCb::Track::Backward ) ) continue;
    Gaudi::XYZPoint point = (*itT)->position();
    Gaudi::XYZVector dir  = (*itT)->slopes();
    double lowestIP = 1.e9;
    LHCb::RecVertex* best = NULL;
    for (  itPv = myVertices->begin(); myVertices->end() != itPv ; ++itPv ) {
      double dx = point.x() + ( (*itPv)->position().z() - point.z() ) * dir.x() - (*itPv)->position().x();
      double dy = point.y() + ( (*itPv)->position().z() - point.z() ) * dir.y() - (*itPv)->position().y();
      double ip2 = dx*dx + dy*dy;
      if ( ip2 < lowestIP ) {
        lowestIP = ip2;
        best = *itPv;
      }
    }
    lowestIP = sqrt( lowestIP );
    if ( lowestIP > m_minIPForTrack &&
         lowestIP < m_maxIPForTrack    ) {
      if ( debug ) info() << format( "Large IP %6.3f zTr %7.3f zBest %7.3f",
                                     lowestIP, point.z(), best->position().z() ) << endmsg;
      m_nbLargeIP++;
    }
  }
  m_nEvent++;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VertexChecker::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  info() << "=== MC Vertex reconstruction efficiency ===" << endmsg;
  int totVertices = 0;
  int totFound    = 0;
  double eff = 0.;

  for ( unsigned int kk = 0 ; m_mcVertices.size() > kk ; ++kk ) {
    totVertices += m_mcVertices[kk];
    totFound    += m_mcFound[kk];
    eff = 0.;
    if ( 0 < m_mcVertices[kk] ) {
      eff = 100. * m_mcFound[kk] / double( m_mcVertices[kk] );
    }
    if ( kk != m_mcVertices.size()-1 ) {
      info() << format( "   MC %3d to %3d parts:  N %6d found %6d  eff %6.2f %%",
                        m_binSizeMC*kk, m_binSizeMC*(kk+1), m_mcVertices[kk], m_mcFound[kk], eff ) << endmsg;
    } else {
      info() << format( "   MC   over %3d parts:  N %6d found %6d  eff %6.2f %%",
                        m_binSizeMC*kk, m_mcVertices[kk], m_mcFound[kk], eff ) << endmsg;
    }
  }
  eff = 100. * totFound / double( totVertices );
  info() << format( "   MC total              N %6d found %6d  eff %6.2f %%",
                    totVertices, totFound, eff )
         << endmsg
         << "=== Fake rate of reconstructed vertices ===" << endmsg;
  int totRec = 0;
  int totFake = 0;
  for ( unsigned int kk = 0 ; m_recVertices.size() > kk ; ++kk ) {
    totRec += m_recVertices[kk];
    totFake += m_recFake[kk];
    eff = 0.;
    if ( 0 < m_recVertices[kk] ) {
      eff = 100. * m_recFake[kk] / double( m_recVertices[kk] );
    }
    if ( kk == m_recVertices.size() - 1 ) {
      info() << format( "  REC   over %3d tracks: N %6d fake %6d  ghost %6.2f %%",
                        5*kk, m_recVertices[kk], m_recFake[kk], eff ) << endmsg;
    } else {
      info() << format( "  REC %3d to %3d tracks: N %6d fake %6d  ghost %6.2f %%",
                        5*kk, 5*(kk+1), m_recVertices[kk], m_recFake[kk], eff ) << endmsg;
    }
  }
  eff = 100. * totFake / double( totRec );
  info() << format( "  REC total              N %6d fake %6d  ghost %6.2f %%",
                    totRec, totFake, eff ) << endmsg;

  double meanX = m_sx / m_s0;
  info() << format( "Distance to MC: <x> %7.3f sx %7.3f", meanX, sqrt( m_sx2/m_s0 - meanX*meanX) ) << endmsg;
  double meanY = m_sy / m_s0;
  info() << format( "Distance to MC: <y> %7.3f sy %7.3f", meanY, sqrt( m_sy2/m_s0 - meanY*meanY) ) << endmsg;
  double meanZ = m_sz / m_s0;
  info() << format( "Distance to MC: <z> %7.3f sz %7.3f", meanZ, sqrt( m_sz2/m_s0 - meanZ*meanZ) ) << endmsg;

  double trackRate = double( m_nbLargeIP ) / double ( m_nEvent );
  info() << format( "Number of large ( %5.3f to %5.3f mm) IP tracks/event = %7.2f",
                    m_minIPForTrack, m_maxIPForTrack, trackRate )
         << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
