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
                            const std::array<double,7>& magnetParams,
                            const std::array<double,3>& momentumParams,
                            const std::vector<double>& yParams,
                            const double magnetScale) :
  //  m_magPar(&magnetParams), 
  m_momPar(&momentumParams), 
  m_track(tr),
  m_magnetScale(magnetScale), 
  m_zTT(zTT),
  m_ignore(false),
  m_firedLayers(0)
{
  m_hits.reserve(6);
    
  // -- Number of IT hits
  const unsigned int nbIT = std::count_if( tr->lhcbIDs().begin(), tr->lhcbIDs().end(), 
                                           [](const LHCb::LHCbID id){ return id.isIT();});
  
  m_state = &tr->closestState( 10000. );
  
  // -- See KsFitParams to see how these coefficients are derived.
  double zMagnet = magnetParams[0] + 
    magnetParams[1] * m_state->ty() * m_state->ty() +  
    magnetParams[2] * m_state->tx() * m_state->tx() +
    magnetParams[3] * 1/m_state->p() +
    magnetParams[4] * std::abs( m_state->x() ) +
    magnetParams[5] * std::abs( m_state->y() ) +
    magnetParams[6] * std::abs( m_state->ty() );
  
  

  const double dz      = zMagnet - m_state->z();
  double xMagnet = m_state->x() + dz * m_state->tx();
  m_slopeX       = xMagnet / zMagnet;
  
  const double dSlope = std::abs( m_slopeX - m_state->tx() );
  const double dSlope2 = dSlope*dSlope;

  double by = m_state->y() / ( m_state->z() + 
                               ( yParams[0] * fabs(m_state->ty()) * zMagnet + yParams[1] )* dSlope2  );
  
  if ( 4 < nbIT ) by = m_state->ty(); 
  m_slopeY = by * ( 1. + yParams[0] * fabs(by) * dSlope2 );
 
  const double yMagnet = m_state->y() + dz * by - yParams[1] * by * dSlope2;
  
  // -- These resolutions are semi-empirical and are obtained by fitting residuals
  // -- with MCHits and reconstructed tracks
  // -- See Tracking &Alignment meeting, 19.2.2015.
  if(nbIT > 3 ){
    m_errXMag = dSlope * 30.0 + 0.5;
    m_errYMag = dSlope * 18.0 + 1.5;
  }else{
    m_errXMag = dSlope2 * 15.0 + dSlope * 15.0 + 3.0;
    m_errYMag = dSlope2 * 80.0 + dSlope * 10.0 + 4.0;
  }

  m_magnet = Gaudi::XYZPoint( xMagnet, yMagnet, zMagnet );

  //=== Save for reference
  m_magnetSave = m_magnet;
  m_slopeXSave = m_slopeX;
  m_displX     = 0.;
  m_displY     = 0.;

  //=== Initialize all other data members
  m_chi2       = 0.;
  m_slopeXCand = m_slopeX;

  m_curvature  = 1.7e-5 * ( m_state->tx() - m_slopeX );
}
//=============================================================================
