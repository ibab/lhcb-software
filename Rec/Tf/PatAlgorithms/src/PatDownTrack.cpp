// $Id: PatDownTrack.cpp,v 1.4 2009-10-08 10:09:46 sstahl Exp $
// Include files 

// local
#include "PatDownTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatDownTrack, from Pat/PatKShort package
//
// 2007-10-18 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatDownTrack::PatDownTrack( LHCb::Track* tr, 
                            double zTT,
                            const std::vector<double>& magnetParams,
                            const std::vector<double>& momentumParams,
                            const std::vector<double>& yParams,
                            const double errZMag,
                            const double magnetScale) :
  m_magPar(&magnetParams), 
  m_momPar(&momentumParams), 
  m_track(tr),
  m_magnetScale(magnetScale), 
  m_zTT(zTT)
{
  m_hits.reserve(8);
    
  // -- Number of IT hits
  unsigned int nbIT = std::count_if( tr->lhcbIDs().begin(), tr->lhcbIDs().end(), 
                                     [](const LHCb::LHCbID id){ return id.isIT();});
  
  m_state = &tr->closestState( 10000. );
  
  // -- Calculate the z position of the point in the magnet
  const double zMagnet = 
    (*m_magPar)[0] + 
    (*m_magPar)[1] * m_state->ty() * m_state->ty() +
    (*m_magPar)[2] * m_state->tx() * m_state->tx() + (*m_magPar)[3] / m_state->p();

  const double dz      = zMagnet - m_state->z();
  const double xMagnet = m_state->x() + dz * m_state->tx();
  m_slopeX       = xMagnet / zMagnet;
  
  const double dSlope2 = (m_slopeX - m_state->tx())*(m_slopeX - m_state->tx());

  double by = m_state->y() / ( m_state->z() + 
                               ( yParams[0] * fabs(m_state->ty()) * zMagnet + yParams[1] )* dSlope2  );
  
  if ( 4 < nbIT ) by = m_state->ty(); 
  m_slopeY = by * ( 1. + yParams[0] * fabs(by) * dSlope2 );
 
  const double yMagnet = m_state->y() + dz * by - yParams[1] * by * dSlope2;
  m_magnet = Gaudi::XYZPoint( xMagnet, yMagnet, zMagnet );
  
  // -- These numbers have to be revised...
  m_errXMag = errZMag * std::abs( m_slopeX - m_state->tx() ) + 2.;
  m_errYMag = 15.;
  if ( 4 < nbIT ) m_errYMag = 4.;

  //=== Save for reference
  m_magnetSave = m_magnet;
  m_slopeXSave = m_slopeX;
  m_displX     = 0.;
  m_displY     = 0.;

  //=== Initialize all other data members
  m_moment     = 0.;
  m_chi2       = 0.;
  m_slopeXCand = m_slopeX;

  m_curvature  = 1.7e-5 * ( m_state->tx() - m_slopeX );
}
//=============================================================================
