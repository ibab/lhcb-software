// $Id: PatVeloPhiList.cpp,v 1.3 2009-03-13 16:17:32 ocallot Exp $
// Include files 

// local
#include "PatVeloPhiList.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloPhiList
//
// 2006-07-26 : David Hutchcroft
//-----------------------------------------------------------------------------

namespace Tf {
  //=============================================================================
  // Default constructor, initializes variables
  //=============================================================================
  PatVeloPhiList::PatVeloPhiList( ) {
    m_coord.reserve( 30 );
    m_coord.clear();
    m_phi    = 0.;
    m_cosPhi = 1.;
    m_sinPhi = 0.;
    m_bestCoord = 0;

    m_predX = 0.;
    m_predY = 0.;
    m_tol   = 0.;
    m_chi2  = -1.;
    m_qFact = -1000.;
    m_posX = 0.;
    m_slopeX = 0.;
    m_posY = 0.;
    m_slopeY = 0.;
    m_lastZ = 0;
    m_valid = true;

    m_s0 = m_sx = m_sz = m_sxz=  m_sz2 = m_t0 =  m_ty = m_tz = m_tyz = m_tz2 =0.;
  }

  //=============================================================================
  // Standard constructor, initializes variables
  //=============================================================================
  PatVeloPhiList::PatVeloPhiList( double phi, PatVeloPhiHit* coord ) {
    m_coord.reserve( 30 );
    initialize (phi,coord);
  }

  void PatVeloPhiList::initialize ( double phi, PatVeloPhiHit* coord ){
    m_coord.clear();
    m_phi    = phi;
    m_cosPhi = cos( phi );
    m_sinPhi = sin( phi );
    m_bestCoord = coord;

    m_predX = 0.;
    m_predY = 0.;
    m_tol   = 0.;
    m_chi2  = -1.;
    m_qFact = -1000.;
    m_lastZ = 0;
    m_valid = true;

    m_s0 = m_sx = m_sz = m_sxz=  m_sz2 = m_t0 =  m_ty = m_tz = m_tyz = m_tz2 =0.;

    saveCandidate();  
  }


  /// distance^2 in 2D between track extrapolation and x,y position
  double PatVeloPhiList::distSquared( double x, double y ) {
    double dx = x-m_predX;
    double dy = y-m_predY;
    return dx*dx + dy*dy;
  }

  /// compare new coord to last extrapolation/coord
  bool PatVeloPhiList::isCloser( double xPred, double yPred, PatVeloPhiHit* coord ) {
    double dist = distSquared( xPred, yPred );
    if ( m_tol > dist ) {
      m_tol       = dist;
      m_bestCoord = coord;
      return true;
    }
    return false;
  }

  /// extrapolate to new Z position
  void PatVeloPhiList::setExtrapolation( double z, double r, 
      double tol, double tol2 ) { 
    m_bestCoord = 0;

    m_tol = tol;
    if ( fabs( m_lastZ-z ) > 40 ) m_tol = tol * (m_lastZ-z) / 40;
    if ( fabs( m_lastZ-z ) > 60 ) m_tol = tol * 60 / 40;

    if ( 1 < m_coord.size() ) {
      m_predX = m_posX + z * m_slopeX;
      m_predY = m_posY + z * m_slopeY;
    } else {
      m_predX = r * m_cosPhi;
      m_predY = r * m_sinPhi;
      m_tol   = tol2 * r ;
    }
    m_tol = m_tol * m_tol;
  }

  /// add the candidate to the list (if there is one) and refit
  void PatVeloPhiList::saveCandidate( ){
    if ( 0 == m_bestCoord ) return;
    m_valid = true;

    m_coord.push_back( m_bestCoord ); 

    double r = m_bestCoord->referenceR();
    double x = r * m_bestCoord->cosPhi() ;
    double y = r * m_bestCoord->sinPhi() ;
    double z = m_bestCoord->z();
    m_lastZ = z;

    double variance =  r * r * m_bestCoord->hit()->variance();


    double wx = 1./variance;
    double wy = 1./variance;

    m_s0  += wx;
    m_sx  += wx * x;
    m_sz  += wx * z;
    m_sxz += wx * x * z;
    m_sz2 += wx * z * z;

    m_t0  += wy;
    m_ty  += wy * y;
    m_tz  += wy * z;
    m_tyz += wy * y * z;
    m_tz2 += wy * z * z;

    double den = m_sz2 * m_s0 - m_sz * m_sz ;
    if ( 0 == den ) den = 1;
    m_posX   = ( m_sx  * m_sz2 - m_sxz * m_sz  ) / den;
    m_slopeX = ( m_sxz * m_s0  - m_sx  * m_sz  ) / den;

    den = m_tz2 * m_t0 - m_tz * m_tz ;
    if ( 0 == den ) den = 1;
    m_posY   = ( m_ty  * m_tz2 - m_tyz * m_tz  ) / den;
    m_slopeY = ( m_tyz * m_t0  - m_ty  * m_tz  ) / den;

    m_bestCoord = 0;
  }

  /// Calculate chiSq for this combination of clusters
  double PatVeloPhiList::computeChi2() {
    double chi2 = 0.;
    for ( std::vector<PatVeloPhiHit*>::iterator itC = m_coord.begin();
        m_coord.end() != itC; ++itC ) {
      double r = (*itC)->referenceR();
      double x = r * (*itC)->cosPhi() ;
      double y = r * (*itC)->sinPhi() ;
      double z = (*itC)->z();
      double variance =  r * r * (*itC)->hit()->variance();

      double deltaNum =  m_posX + z * m_slopeX - x;
      double delta2   = deltaNum*deltaNum / variance ;
      chi2 += delta2;
      deltaNum = ( m_posY + z * m_slopeY - y );
      delta2   = deltaNum*deltaNum / variance;
      chi2 += delta2;
    }
    m_chi2 = chi2 / ( 2*(m_coord.size()-2) );
    return m_chi2;
  }

  /// Number of uniquely used clusters
  int PatVeloPhiList::nbUnused() {
    int n = 0;
    for ( std::vector<PatVeloPhiHit*>::iterator itC = m_coord.begin();
        m_coord.end() != itC; ++itC ) {
      if ( !(*itC)->hit()->isUsed() ) ++n;
    }
    return n;
  }
}
