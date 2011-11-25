// $Id: $
// Include files 



// local
#include "FastVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FastVertex
//
// 2011-11-15 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FastVertex::FastVertex( std::vector<TrackForPV*>:: iterator itT1, 
                        std::vector<TrackForPV*>:: iterator itT2 ) {
  m_s0   = 0.;
  m_u0   = 0.;
  m_sx   = 0.;
  m_uy   = 0.;
  m_stx  = 0.;
  m_uty  = 0.;
  m_sxTx = 0.;
  m_uyTy = 0.;
  m_stx2 = 0.;
  m_uty2 = 0.;
  for ( std::vector<TrackForPV*>::iterator itT = itT1; itT2 >= itT ; ++itT ) {
    addTrack( *itT );
  }
}
//=============================================================================
// Destructor
//=============================================================================
FastVertex::~FastVertex() {} 

//=========================================================================
//  
//=========================================================================
void FastVertex::addTrack( TrackForPV* track ) {
  m_tracks.push_back( track );
  double tx = track->tx();
  double ty = track->ty();
  double x0 = track->x0();
  double y0 = track->y0();
  double wx = track->wx();
  double wy = track->wy();

  m_s0   += wx;
  m_u0   += wy;
  m_sx   += wx * x0;
  m_uy   += wy * y0;
  m_stx  += wx * tx;
  m_uty  += wy * ty;
  m_sxTx += wx * x0 * tx;
  m_uyTy += wy * y0 * ty;
  m_stx2 += wx * tx * tx;
  m_uty2 += wy * ty * ty;

  if ( m_tracks.size() > 2 ) solve();
}
//=========================================================================
//  Remove the specified track
//=========================================================================
void FastVertex::removeTrack( TrackForPV* track ) {
  std::vector<TrackForPV*>::iterator itFound = std::find( m_tracks.begin(), m_tracks.end(), track );
  if ( itFound == m_tracks.end() )  return;  // not included, ignore.
  removeTrack( itFound );
}

//=========================================================================
//  Remove a track specified by an iterator
//=========================================================================
void FastVertex::removeTrack ( std::vector<TrackForPV*>::iterator it ) {
  double tx = (*it)->tx();
  double ty = (*it)->ty();
  double x0 = (*it)->x0();
  double y0 = (*it)->y0();
  double wx = (*it)->wx();
  double wy = (*it)->wy();
  m_tracks.erase( it );

  m_s0   -= wx;
  m_u0   -= wy;
  m_sx   -= wx * x0;
  m_uy   -= wy * y0;
  m_stx  -= wx * tx;
  m_uty  -= wy * ty;
  m_sxTx -= wx * x0 * tx;
  m_uyTy -= wy * y0 * ty;
  m_stx2 -= wx * tx * tx;
  m_uty2 -= wy * ty * ty;

  solve();
}
//=========================================================================
//  Recompute the vertex position
//=========================================================================
void FastVertex::solve ( ) { 
  double den = m_s0 * m_stx2 - m_stx * m_stx  + m_u0 * m_uty2 - m_uty * m_uty;
  double zv = - ( m_s0 * m_sxTx - m_sx * m_stx + m_u0 * m_uyTy - m_uy * m_uty ) / den;
  double xv = ( m_sx + zv * m_stx ) / m_s0;
  double yv = ( m_uy + zv * m_uty ) / m_u0;
  m_vertex = Gaudi::XYZPoint( xv, yv, zv );
}

//=========================================================================
//  
//=========================================================================
void FastVertex::removeWorsts( double maxChi2 ) {
  double highestChi2 = -1;
  std::vector<TrackForPV*>::iterator it;
  std::vector<TrackForPV*>::iterator worst = m_tracks.begin();
  for ( it = m_tracks.begin(); m_tracks.end() != it; ++it ) {
    double chi2 = (*it)->chi2( m_vertex );
    if ( chi2 > highestChi2 ) {
      highestChi2 = chi2;
      worst = it;
    }
  }
  if ( highestChi2 > maxChi2 ) {
    removeTrack( worst );
    if ( m_tracks.size() > 4 ) removeWorsts( maxChi2 );
  }
}

//=========================================================================
//  Produce the covariance matrix.
//=========================================================================
Gaudi::SymMatrix3x3 FastVertex::cov ( ) {
  Gaudi::SymMatrix3x3 cov;
  cov(0,0) = m_s0;
  cov(1,0) = 0.;
  cov(1,1) = m_u0;
  cov(2,0) = - m_stx;
  cov(2,1) = - m_uty;
  cov(2,2) = m_stx2 + m_uty2;

  bool ifail = cov.InvertChol( );
  if (!ifail ) std::cout << "FastVertex:covariance: Choleski inverstion failed." << std::endl;
  return cov;
}
//=============================================================================
