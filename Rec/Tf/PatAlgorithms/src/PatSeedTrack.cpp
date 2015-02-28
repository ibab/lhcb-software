// $Id: PatSeedTrack.cpp,v 1.5 2009-11-26 18:00:48 mschille Exp $
// Include files

#include <cmath>
#include <algorithm>

// local
#include "PatSeedTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatSeedTrack
//
// 2006-10-17 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Constructor with 3 arguments, one per station. Define the x trajectory.
// Hits are NOT stored in the track.
//=============================================================================
PatSeedTrack::PatSeedTrack(
    double x0, double x1, double x2,
    double z0, double z1, double z2,
    double zRef, double dRatio ) :
  m_valid(true), m_nbPlanes(0),
  m_z0(zRef),
  m_dx(dRatio),
  m_ay(0.), m_by(0.), m_cosine(1.), m_chi2(HUGE_VAL)
{
  const auto dz0 = z0 - zRef, dz1 = z1 - zRef, dz2 = z2 - zRef;
  const auto qt0 = dz0 * dz0 * (1 + dz0 * dRatio);
  const auto qt1 = dz1 * dz1 * (1 + dz1 * dRatio);
  const auto qt2 = dz2 * dz2 * (1 + dz2 * dRatio);
  const auto rt1 = (qt0 - qt1) / (z0 - z1);
  const auto rt2 = (qt0 - qt2) / (z0 - z2);
  const auto dx02 = x0 - x2, dx01 = x0 - x1;
  const auto dz02 = z0 - z2, dz01 = z0 - z1;
  m_cx = (dx02 / dz02 - dx01 / dz01) / (rt2 - rt1);
  m_bx = dx01 / dz01 - m_cx * rt1;
  m_ax = x1 - m_bx * dz1 - m_cx * qt1;

  std::fill(m_planeList.begin(), m_planeList.end(), 0);
}

//=========================================================================
//  Constructor with 4 arguments, a space point in IT. Fix x, y and slopex
//  the 4 coordinates are added to the track.
//=========================================================================
PatSeedTrack::PatSeedTrack(
    PatFwdHit* c0, PatFwdHit* c1, PatFwdHit* c2, PatFwdHit* c3,
    double zRef, double dRatio, double arrow ) :
  m_valid(true), m_nbPlanes(0), m_dx(dRatio), m_ay(0.0), m_cosine(1.),
  m_chi2(HUGE_VAL)
{
  //== x = a + b*dz + c*dz^2 + d*dz^3
  //== d = dRation * c
  //== c = curvature * (x at z=0 )
  //== solve with c0 and c3 only

  // arrow is m_cx * dz^2 for nominal distance between stations.
  const double curvature = - arrow;
  const double dz0 = c0->z() - zRef;
  const double curv0 = curvature * ( 1. + dRatio * dz0 ) * dz0 * dz0;
  const double a0 =  1. + curv0;
  const double b0 = dz0 - zRef * curv0;
  const double dz3 = c3->z() - zRef;
  const double curv3 = curvature * ( 1. + dRatio * dz3 ) * dz3 * dz3;
  const double a3 =  1. + curv3;
  const double b3 = dz3 - zRef * curv3;
  const double x0 = c0->x();
  const double x3 = c3->x();

  m_z0 = zRef;
  m_bx = ( x0 * a3 - a0 * x3 ) / ( b0 * a3 - a0 * b3 );
  m_ax = ( x0 * b3 - b0 * x3 ) / ( a0 * b3 - b0 * a3 );
  m_cx = curvature * ( m_ax - zRef * m_bx );

  m_by = .5 * ( ( xAtZ( c1->z() ) - c1->x() ) / c1->hit()->dxDy() +
                ( xAtZ( c2->z() ) - c2->x() ) / c2->hit()->dxDy()  ) / m_z0 ;

  std::fill(m_planeList.begin(), m_planeList.end(), 0);

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

// vim: shiftwidth=2 tw=78
