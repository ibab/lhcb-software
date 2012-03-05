// Include files
#include "GaudiKernel/PhysicalConstants.h"

// local
#include "PatVeloOpenTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloOpenTrack
//
// 2007-03-13 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatVeloOpenTrack::PatVeloOpenTrack(  ) {
  m_x0 = 0.;
  m_tx = 0.;
  m_y0 = 0.;
  m_ty = 0.;
  m_chi2 = 0.;
  m_backward = false;
  m_firstZ = 0.;
  m_lastZ  = 0.;
  m_valid  = true;
  m_half   = 0;
  m_errX2 = 0.;
  m_errY2 = 0.;
  m_errTx2 = 0.;
  m_errTy2 = 0.;
}

//=========================================================================
//  Usual constructor: 2R, 2Phi and the phi's strip parameters
//=========================================================================
PatVeloOpenTrack::PatVeloOpenTrack ( Tf::PatVeloRHit*   cR0,   Tf::PatVeloRHit*   cR2, 
                                     Tf::PatVeloPhiHit* cPhi0, Tf::PatVeloPhiHit* cPhi2 ) {

  m_half = int( cR0->sensor()->isRight() );   //== Not very nice: A bool -> int conversion!
  
  double r0    = cR0->coordHalfBox();
  double zR0   = cR0->z();
  double r2    = cR2->coordHalfBox();
  double zR2   = cR2->z();
  double zPhi0 = cPhi0->z();
  double zPhi2 = cPhi2->z();

  double rP0 = r0 + (r2-r0) * (zPhi0-zR0) / (zR2 - zR0);
  double rP2 = r0 + (r2-r0) * (zPhi2-zR0) / (zR2 - zR0);
  double phi0 = asin( cPhi0->referenceR() / rP0 ) + cPhi0->referencePhi();
  double phi2 = asin( cPhi2->referenceR() / rP2 ) + cPhi2->referencePhi();
  double dPhi = phi2 - phi0;
  phi0 = phi0 + dPhi * (zR0-zPhi0)/(zPhi2 - zPhi0);
  phi2 = phi2 + dPhi * (zR2-zPhi2)/(zPhi0 - zPhi2);

  //== phi0 is the azimuth in the R0 sensor, same phi2
  //== Adjust phi at R to make phi at Phi sensors OK
  for (int it = 1; it < 10; it++) {
    double c0 = cos(phi0);
    double s0 = sin(phi0);
    double c2 = cos(phi2);
    double s2 = sin(phi2);
    m_tx = (     r0*c0 -     r2*c2 ) / ( zR0 - zR2 );
    m_x0 = ( zR0*r2*c2 - zR2*r0*c0 ) / ( zR0 - zR2 );
    m_ty = (     r0*s0 -     r2*s2 ) / ( zR0 - zR2 );
    m_y0 = ( zR0*r2*s2 - zR2*r0*s0 ) / ( zR0 - zR2 );
    double dPhi0 = ( asin( cPhi0->referenceR() / rAtZ( zPhi0 ) ) + cPhi0->referencePhi() - phiAtZ( zPhi0 ));
    double dPhi2 = ( asin( cPhi2->referenceR() / rAtZ( zPhi2 ) ) + cPhi2->referencePhi() - phiAtZ( zPhi2 ));
    if (  Gaudi::Units::pi < dPhi0 ) dPhi0 -= 2. *  Gaudi::Units::pi;
    if (  Gaudi::Units::pi < dPhi2 ) dPhi2 -= 2. *  Gaudi::Units::pi;
    if ( -Gaudi::Units::pi > dPhi0 ) dPhi0 += 2. *  Gaudi::Units::pi;
    if ( -Gaudi::Units::pi > dPhi2 ) dPhi2 += 2. *  Gaudi::Units::pi;  
    phi0 = phi0 + dPhi0 + (dPhi2 - dPhi0) * (zR0-zPhi0)/(zPhi2 - zPhi0);
    phi2 = phi2 + dPhi2 + (dPhi0 - dPhi2) * (zR2-zPhi2)/(zPhi0 - zPhi2);
    if ( 1.e-4 > fabs( dPhi0 ) && 1.e-4 > fabs( dPhi2 ) ) break;
  }
  m_chi2 = 0.;
  m_backward = false;
  m_firstZ = 0.;
  m_lastZ  = 0.;
  m_valid  = true;
  m_errX2 = 0.;
  m_errY2 = 0.;
  m_errTx2 = 0.;
  m_errTy2 = 0.;
}
//=============================================================================
// Destructor
//=============================================================================
PatVeloOpenTrack::~PatVeloOpenTrack() {} 

//=========================================================================
//  Update the track parameters after adding coordinates
//=========================================================================
void PatVeloOpenTrack::update ( ) {
  if ( 3 > m_rCoords.size() ) return;
  if ( 3 > m_phiCoords.size() ) return;
  double s0  = 0;
  double sz  = 0.;
  double sz2 = 0.;
  double sc  = 0.;
  double szc = 0.;
  double ss  = 0.;
  double szs = 0.;
  PatVeloRHits::const_iterator itC;
  for ( itC = m_rCoords.begin(); m_rCoords.end() != itC; ++itC ) {
    double w = (*itC)->weight();
    double z = (*itC)->z();
    double phi = phiAtZ( z );
    double c = (*itC)->coordHalfBox() * cos( phi );
    double s = (*itC)->coordHalfBox() * sin( phi );
    s0  += w;
    sz  += w * z;
    sz2 += w * z * z;
    sc  += w * c;
    szc += w * z * c;
    ss  += w * s;
    szs += w * z * s;
  }

  PatVeloPhiHits::const_iterator itP;
  for ( itP = m_phiCoords.begin(); m_phiCoords.end() != itP; ++itP ) {
    double z = (*itP)->z();
    double r = rAtZ( z );
    double w = 1. / (*itP)->hit()->variance() / r / r;
    double phi= (*itP)->referencePhi() + asin( (*itP)->referenceR()/r );
    double c = r * cos( phi );
    double s = r * sin( phi );
    s0  += w;
    sz  += w * z;
    sz2 += w * z * z;
    sc  += w * c;
    szc += w * z * c;
    ss  += w * s;
    szs += w * z * s;
  }
  double den = s0 * sz2 - sz * sz;
  m_x0 = ( sc  * sz2 - szc * sz  ) / den;
  m_tx = ( s0  * szc - sc  * sz  ) / den;
  m_y0 = ( ss  * sz2 - szs * sz  ) / den;
  m_ty = ( s0  * szs - ss  * sz  ) / den;

  m_errX2  = 1./s0;
  m_errY2  = 1./s0;
  m_errTx2 = 1./sz;
  m_errTy2 = 1./sz;
}

//=========================================================================
//  Final fit, compute chi2, backward and first values
//=========================================================================
void PatVeloOpenTrack::finalFit ( ) {
  update();
  //== Compute chi2
  PatVeloRHits::const_iterator itC;
  m_chi2 = 0.;
  for ( itC = m_rCoords.begin(); m_rCoords.end() != itC; ++itC ) {
    double w = (*itC)->weight();
    double z = (*itC)->z();
    double phi = phiAtZ( z );
    double c = (*itC)->coordHalfBox() * cos( phi );
    double s = (*itC)->coordHalfBox() * sin( phi );
    double dx = xAtZ( z ) - c;
    double dy = yAtZ( z ) - s;
    m_chi2 += w * (dx*dx + dy * dy);
  }
  PatVeloPhiHits::const_iterator itP;
  for ( itP = m_phiCoords.begin(); m_phiCoords.end() != itP; ++itP ) {
    double z = (*itP)->z();
    double r = rAtZ( z );
    double w = 1. / (*itP)->hit()->variance() / r / r;
    double phi= (*itP)->referencePhi() + asin( (*itP)->referenceR()/r );
    double c = r * cos( phi );
    double s = r * sin( phi );
    double dx = xAtZ( z ) - c;
    double dy = yAtZ( z ) - s;
    m_chi2 += w * (dx*dx + dy * dy);
  }
  m_chi2 /= (m_rCoords.size() + m_phiCoords.size() - 4);

  double rMin = 1.e6;
  double rMax = 0.;
  for ( itC = m_rCoords.begin(); m_rCoords.end() != itC; ++itC ) {
    if ( rAtZ( (*itC)->z() ) < rMin ) {
      rMin = rAtZ( (*itC)->z() );
      m_firstZ = (*itC)->z();
    }
    if ( rAtZ( (*itC)->z() ) > rMax ) {
      rMax = rAtZ( (*itC)->z() );
      m_lastZ = (*itC)->z();
    }
  }
  int nUp = 0;
  int nDown = 0;
  for ( itC = m_rCoords.begin(); m_rCoords.end() != itC; ++itC ) {
    if ( m_firstZ > (*itC)->z() ) {
      ++nUp;
    } else {
      ++nDown;
    }
  }
  m_backward = nUp > nDown;
}
//=============================================================================
