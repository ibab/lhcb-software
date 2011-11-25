// $Id: $
// Include files 



// local
#include "TrackForPV.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackForPV
//
// 2011-11-15 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
// Add 90 micron in quadrature to the error: This is the MS, for fixed Pt track.
//=============================================================================
TrackForPV::TrackForPV( LHCb::Track* track, double xBeam, double yBeam ) {
  m_track = track;
  m_point = track->position();
  m_dir   = track->slopes();
  double msError = 0.0081;  // 90 microns squared
  m_wx = ( 1. + m_dir.x() * m_dir.x() ) / ( m_track->firstState().errX2() + msError );
  m_wy = ( 1. + m_dir.y() * m_dir.y() ) / ( m_track->firstState().errY2() + msError );
  m_used = false;

  double x0 = m_point.x() - m_point.z() * m_dir.x() - xBeam;
  double y0 = m_point.y() - m_point.z() * m_dir.y() - yBeam;
  double den = m_wx * m_dir.x() * m_dir.x() + m_wy * m_dir.y() * m_dir.y();
  m_zAtBeam = - ( m_wx * x0 * m_dir.x() + m_wy * y0 * m_dir.y() ) / den ;
  double xb = m_point.x() + m_dir.x() * ( m_zAtBeam - m_point.z() ) - xBeam;
  double yb = m_point.y() + m_dir.y() * ( m_zAtBeam - m_point.z() ) - yBeam;
  m_rAtBeam = sqrt( xb*xb + yb*yb );
}
//=============================================================================
// Destructor
//=============================================================================
TrackForPV::~TrackForPV() {} 

//=============================================================================
