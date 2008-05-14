// $Id: PatSeedTrack.cpp,v 1.2 2008-05-14 17:22:18 mschille Exp $
// Include files

// local
#include "PatSeedTrack.h"
#include "PatFwdFitParabola.h"
#include "PatFwdFitLine.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatSeedTrack
//
// 2006-10-17 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Constructor with 3 arguments, one per station. Define the x trajectory.
// Hits are NOT stored in the track.
//=============================================================================
PatSeedTrack::PatSeedTrack( double x0, double x1, double x2, double z0, double z1, double z2,
                            double zRef, double dRatio ) :
	m_valid(true), m_ay(0.), m_by(0.), m_cosine(1.),
	m_nbPlanes(0)
{
  m_z0 = .5 * ( z0 + z2 );
  double dz = .5 * ( z2 - z0 );
  m_bx = ( x2 - x0 ) / ( 2. * dz );
  m_ax = x1 - ( z1 - m_z0 ) * m_bx;
  m_cx = ( x2 + x0 -2*m_ax ) / ( 2. * dz * dz );
  m_dx = 0.;

  // Applies a correction in cubic term, proportional to cx at zRef.

  double zDiff = zRef - m_z0;
  m_dx = dRatio * m_cx / ( 1. - 3* zDiff * dRatio ) ;
  m_bx = m_bx - m_dx * dz * dz;  // Correct so that we still go through the points

  m_ax = m_ax + zDiff * ( m_bx + zDiff * ( m_cx + zDiff * m_dx ));
  m_bx = m_bx + zDiff * ( 2 * m_cx + zDiff * 3 * m_dx );
  m_cx = m_cx + zDiff * ( 3 * m_dx );
  m_z0 = zRef;

  std::fill(m_planeList.begin(), m_planeList.end(), 0);
  m_coords.clear();
  m_coords.reserve(32);
}

//=========================================================================
//  Constructor with 4 arguments, a space point in IT. Fix x, y and slopex
//  the 4 coordinates are added to the track.
//=========================================================================
PatSeedTrack::PatSeedTrack( PatFwdHit* c0, PatFwdHit* c1, PatFwdHit* c2, PatFwdHit* c3,
                            double zRef, double dRatio, double arrow ) :
	m_valid(true), m_ay(0.0), m_cosine(1.), m_nbPlanes(0)
{
  //== x = a + b*dz + c*dz^2 + d*dz^3
  //== d = dRation * c
  //== c = curvature * (x at z=0 )
  //== solve with c0 and c3 only

  // arrow is m_cx * dz^2 for nominal distance between stations.
  double curvature = - arrow / ( 700*700.);
  double dz0 = c0->z() - zRef;
  double curv0 = curvature * ( 1. + dRatio * dz0 ) * dz0 * dz0;
  double a0 =  1. + curv0;
  double b0 = dz0 - zRef * curv0;
  double dz3 = c3->z() - zRef;
  double curv3 = curvature * ( 1. + dRatio * dz3 ) * dz3 * dz3;
  double a3 =  1. + curv3;
  double b3 = dz3 - zRef * curv3;
  double x0 = c0->x();
  double x3 = c3->x();

  m_z0 = zRef;
  m_ax = ( x0 * b3 - b0 * x3 ) / ( a0 * b3 - b0 * a3 );
  m_bx = ( x0 * a3 - a0 * x3 ) / ( b0 * a3 - a0 * b3 );
  m_cx = curvature * ( m_ax - zRef * m_bx );
  m_dx = dRatio * m_cx;

  m_by = .5 * ( ( xAtZ( c1->z() ) - c1->x() ) / c1->hit()->dxDy() +
                ( xAtZ( c2->z() ) - c2->x() ) / c2->hit()->dxDy()  ) / m_z0 ;

  std::fill(m_planeList.begin(), m_planeList.end(), 0);
  m_coords.clear();
  m_coords.reserve(32);

  addCoord( c0 );
  addCoord( c1 );
  addCoord( c2 );
  addCoord( c3 );
}
//=============================================================================
// Destructor
//=============================================================================
PatSeedTrack::~PatSeedTrack() {}
//=============================================================================
