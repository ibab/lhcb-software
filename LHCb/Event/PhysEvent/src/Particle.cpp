// $Id: Particle.cpp,v 1.9 2002-10-03 12:52:30 gcorti Exp $
// Include files 

// STD and STL
#include <iostream> // from Gaudi
#include "GaudiKernel/StreamBuffer.h" 

// local
#include "Event/Particle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Particle
//                                 derived from class KeyedObjeect
//
// 19/03/2002 : Gloria Corti
//-----------------------------------------------------------------------------

//=============================================================================
// Copy constructor
//=============================================================================
Particle::Particle(const Particle& part)
  : KeyedObject<int>()
  , m_particleID( part.particleID() ), m_confLevel( part.confLevel() )
  , m_momentum( part.momentum() ), m_pointOnTrack( part.pointOnTrack() )
  , m_mass( part.mass() ), m_massErr( part.massErr() )
  , m_momentumErr( part.momentumErr() )
  , m_pointOnTrackErr( part.pointOnTrackErr() )
  , m_posMomCorr( part.posMomCorr() ), m_isResonance( part.isResonance() )
  , m_slopesMomErr( part.slopesMomErr() )
  , m_posSlopesCorr( part.posSlopesCorr() ), m_desktop( 0 )
  , m_origin( part.origin() )
{
  if( part.endVertex() != NULL ) {
    m_endVertex = part.endVertex()->clone();
  }
}

//=============================================================================
// Clone 
//=============================================================================
Particle* Particle::clone() const
{
  return new Particle(*this);
}

//=============================================================================
// Assignment operator
//=============================================================================
Particle& Particle::operator=(const Particle& orig) {
  
  // protect against self assignement
  if( this != &orig ) {
    m_particleID = orig.particleID();
    m_confLevel = orig.confLevel();  
    m_momentum = orig.momentum();
    m_pointOnTrack = orig.pointOnTrack();
    m_mass = orig.mass();
    m_massErr = orig.massErr();  
    m_momentumErr = orig.momentumErr();
    m_pointOnTrackErr = orig.pointOnTrackErr();
    m_posMomCorr = orig.posMomCorr();
    m_isResonance = orig.isResonance();
    m_slopesMomErr = orig.slopesMomErr();
    m_posSlopesCorr = orig.posSlopesCorr();
    m_desktop = 0;
    // Keep the same reference to end vertex ???
    m_endVertex = orig.endVertex();
    // Keep the same reference to origination object
    m_origin = orig.origin();
  }
  return *this;
}

//=============================================================================
// charge in units of positron charge
//=============================================================================
double Particle::charge() const {

  double ch = (particleID().threeCharge())/3.0;
  return ch;

};

//=============================================================================
// Set covariance matrix relative to momentum (4x4) and propagate to 
// (sx, sy, P)
//=============================================================================
void Particle::setMomentumErr(const HepSymMatrix& value)
{
  // Set momentum error
  m_momentumErr = value;

  // Set corresponding error on (sx,sy,P)
  HepMatrix trMomToSlopes(3,3,0);
  double px = m_momentum.px();
  double py = m_momentum.py();
  double pz = m_momentum.pz();
  double p  = m_momentum.vect().mag();

  // Transpose of Transformation matrix of P(sx,sy,P) = f{ S(px,py,pz) }
  trMomToSlopes(1,1) = 1./pz;          ///< dSx/dPx
  trMomToSlopes(1,2) = 0.;             ///< dSx/dPy
  trMomToSlopes(1,3) = -px/(pz*pz);    ///< dSx/dPz
  trMomToSlopes(2,1) = 0.;             ///< dSy/dPx
  trMomToSlopes(2,2) = 1./pz;          ///< dSy/dPy
  trMomToSlopes(2,3) = -py/(pz*pz);    ///< dSy/dPz
  trMomToSlopes(3,1) = px/p;           ///< dP/dPx
  trMomToSlopes(3,2) = py/p;           ///< dP/dPy
  trMomToSlopes(3,3) = pz/p;           ///< dP/dPz  
   
  // Now obtain the new covariance matrix C(P) = transpose(T) x C(S) x T
  m_slopesMomErr = (m_momentumErr.sub(1,3)).similarity(trMomToSlopes);
}

//=============================================================================
// Set matrix with correlation errors between momemtum and pointOnTrack
// (momentum x point) and propagate to slopesMomCorr
//=============================================================================
void Particle::setPosMomCorr(const HepMatrix& value)
{
  // Set correlation errors
  m_posMomCorr = value;

  // Put 3x3 position 3-momentum eletement in a temporary matrix
  HepMatrix temp = value.sub(1,3,1,3);

  // Propagate to slopesMomCorr
  HepMatrix trMomToSlopes(3,3,0);
  double px = m_momentum.px();
  double py = m_momentum.py();
  double pz = m_momentum.pz();
  double p  = m_momentum.vect().mag();

  // Transpose of Transformation matrix of P(sx,sy,P) = f{ S(px,py,pz) }
  trMomToSlopes(1,1) = 1./pz;          ///< dSx/dPx
  trMomToSlopes(1,2) = 0.;             ///< dSx/dPy
  trMomToSlopes(1,3) = -px/(pz*pz);    ///< dSx/dPz
  trMomToSlopes(2,1) = 0.;             ///< dSy/dPx
  trMomToSlopes(2,2) = 1./pz;          ///< dSy/dPy
  trMomToSlopes(2,3) = -py/(pz*pz);    ///< dSy/dPz
  trMomToSlopes(3,1) = px/p;           ///< dP/dPx
  trMomToSlopes(3,2) = py/p;           ///< dP/dPy
  trMomToSlopes(3,3) = pz/p;           ///< dP/dPz 

  // Now obtain the new non diagonal elements: lower angle
  m_posSlopesCorr = trMomToSlopes * temp;
  
}

//=============================================================================
// Set error matrix on (slopeX, slopeY, P) and propagate to momentumErr
//=============================================================================
void Particle::setSlopesMomErr(const HepSymMatrix& value)
{
  // Set error on (sx,sy,P)
  m_slopesMomErr = value;

  // Set corresponding error on (Px,Py,Pz,E)
  HepMatrix trSlopesToMom(4,3,0);
  double p = m_momentum.vect().mag();
  //double pz = fabs(m_momentum.pz());
  double pz = m_momentum.pz();
  double sx = slopeX();
  double sy = slopeY();
  double a = pz/p;
  double a2 = a*a;
  // Transpose of Transformation matrix of P(px,py,pz) = f{ S(sx,sy,P) }
  trSlopesToMom(1,1) = a2*pz*(1+sy*sy);    ///< dPx/dSx
  trSlopesToMom(1,2) = -a2*pz*(sx*sy);     ///< dPx/dSy
  trSlopesToMom(1,3) = a*sx;               ///< dPx/dP
  trSlopesToMom(2,1) = -a2*pz*(sx*sy);     ///< dPy/dSx
  trSlopesToMom(2,2) = a2*pz*(1+sx*sx);    ///< dPy/dSy
  trSlopesToMom(2,3) = a*sy;               ///< dPy/dP
  trSlopesToMom(3,1) = -a2*pz*sx;          ///< dPz/dSx
  trSlopesToMom(3,2) = -a2*pz*sy;          ///< dPz/dSy
  trSlopesToMom(3,3) = a;                  ///< dPz/dP  
  trSlopesToMom(4,1) = 0.0;                ///< dE/dSx
  trSlopesToMom(4,2) = 0.0;                ///< dE/dSy
  trSlopesToMom(4,3) = p/m_momentum.e();   ///< dE/dP
   
  // Now obtain the new covariance matrix C(P) = transpose(T) x C(S) x T
  m_momentumErr = m_slopesMomErr.similarity(trSlopesToMom);
 
}

//=============================================================================
// Set correlation matrix with errors between (slopeX, slopeY, P) 
// and pointOnTrack and propagate to posMomCorr
//=============================================================================
void Particle::setPosSlopesCorr(const HepMatrix& value)
{
  // Set correlation errors
  m_posSlopesCorr = value;

  // Propagate to posMomCorr
  HepMatrix trSlopesToMom(3,3,0);
  double p = m_momentum.vect().mag();
  //double pz = fabs(m_momentum.pz());
  double pz = m_momentum.pz();
  double sx = slopeX();
  double sy = slopeY();
  double a = pz/p;
  double a2 = a*a;
  // Transpose of Transformation matrix of P(px,py,pz) = f{ S(sx,sy,P) }
  trSlopesToMom(1,1) = a2*pz*(1+sy*sy);    ///< dPx/dSx
  trSlopesToMom(1,2) = -a2*pz*(sx*sy);     ///< dPx/dSy
  trSlopesToMom(1,3) = a*sx;               ///< dPx/dP
  trSlopesToMom(2,1) = -a2*pz*(sx*sy);     ///< dPy/dSx
  trSlopesToMom(2,2) = a2*pz*(1+sx*sx);    ///< dPy/dSy
  trSlopesToMom(2,3) = a*sy;               ///< dPy/dP
  trSlopesToMom(3,1) = -a2*pz*sx;          ///< dPz/dSx
  trSlopesToMom(3,2) = -a2*pz*sy;          ///< dPz/dSy
  trSlopesToMom(3,3) = a;                  ///< dPz/dP  
 
  // Now obtain the new non diagonal elements: lower square
  HepMatrix temp = trSlopesToMom * m_posSlopesCorr;
  m_posMomCorr.sub(1,1,temp);  
//    for( int i = 1; i<=3; i++ ) {
//      for( int j = 1; j<=3; j++ ) {
//        m_posMomCorr(i,j) = temp(i,j);
//      }
//    }
  for(int i4 = 1; i4<=3; i4++) {
    m_posMomCorr(4,i4) = 0.0;
  }
  
}
