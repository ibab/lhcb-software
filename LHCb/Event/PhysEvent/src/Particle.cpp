// $Id: Particle.cpp,v 1.21 2006-06-02 16:38:38 jpalac Exp $
// Include files 

// STD and STL
#include <iostream> // from Gaudi

// local
#include "Event/Particle.h"

#include "LHCbMath/MatrixManip.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Particle
//                                 derived from class KeyedObjeect
//
// 19/03/2002 : Gloria Corti
// 25/07/2005 : P. Koppenburg
//-----------------------------------------------------------------------------

//=============================================================================
// Copy constructor
//=============================================================================
LHCb::Particle::Particle(const LHCb::Particle& part)
  : 
  KeyedObject<int>(),
  m_particleID( part.m_particleID ),
  m_measuredMass( part.m_measuredMass ),
  m_measuredMassErr( part.m_measuredMassErr ),
  m_momentum( part.m_momentum ),
  m_referencePoint( part.m_referencePoint ),
  m_momCovMatrix( part.m_momCovMatrix ),
  m_posCovMatrix( part.m_posCovMatrix ),
  m_posMomCovMatrix( part.m_posMomCovMatrix ),
  m_extraInfo( part.m_extraInfo ),
  m_endVertex( part.m_endVertex ),
  m_proto(part.m_proto ),
  m_daughters( part.m_daughters )
{
}

//=============================================================================
// Assignment operator
//=============================================================================
LHCb::Particle& LHCb::Particle::operator=(const LHCb::Particle& orig) {
  
  // protect against self assignement
  if( this != &orig ) {
    m_particleID      = orig.m_particleID;
    m_momentum        = orig.m_momentum;
    m_referencePoint  = orig.m_referencePoint;
    m_measuredMass    = orig.m_measuredMass;
    m_measuredMassErr = orig.m_measuredMassErr;  
    m_posCovMatrix    = orig.m_posCovMatrix;
    m_momCovMatrix    = orig.m_momCovMatrix;
    m_posMomCovMatrix = orig.m_posMomCovMatrix;
    m_extraInfo       = orig.m_extraInfo;
    m_endVertex       = orig.m_endVertex;
    m_proto           = orig.m_proto;
    m_daughters       = orig.m_daughters;
    
  }
  return *this;
}
//=============================================================================
// Create and return the covariance matrix
//=============================================================================
Gaudi::SymMatrix7x7 LHCb::Particle::covMatrix() const 
{

  Gaudi::Matrix7x7 full;

  full.Place_at(m_posCovMatrix,0,0);
  full.Place_at(m_momCovMatrix,3,3);
  full.Place_at(m_posMomCovMatrix,3,0);
  full.Place_at(ROOT::Math::Transpose(m_posMomCovMatrix), 0,3);

  return Gaudi::Math::Symmetrize(full);
	
}

