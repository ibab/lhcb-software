
// STD
#include <iostream>

// local
#include "Event/Particle.h"

#include "LHCbMath/MatrixManip.h"

//=============================================================================

Gaudi::SymMatrix7x7 LHCb::Particle::covMatrix() const
{
  Gaudi::Matrix7x7 full;

  full.Place_at(m_posCovMatrix,0,0);
  full.Place_at(m_momCovMatrix,3,3);
  full.Place_at(m_posMomCovMatrix,3,0);
  full.Place_at(ROOT::Math::Transpose(m_posMomCovMatrix),0,3);

  return Gaudi::SymMatrix7x7( full.LowerBlock() );
}

//=============================================================================

std::ostream& LHCb::Particle::fillStream(std::ostream& s) const
{
  s << "{ "
    << "particleID      : " << m_particleID << std::endl
    << "measuredMass    : " << m_measuredMass << std::endl
    << "measuredMassErr : " << m_measuredMassErr << std::endl
    << "momentum        : " << m_momentum << std::endl
    << "referencePoint  : " << m_referencePoint << std::endl
    << "momCovMatrix    : " << std::endl << m_momCovMatrix << std::endl
    << "posCovMatrix    : " << std::endl << m_posCovMatrix << std::endl
    << "posMomCovMatrix : " << std::endl << m_posMomCovMatrix << std::endl
    << "extraInfo       : [";
  for ( ExtraInfo::const_iterator i = extraInfo().begin();
        i != extraInfo().end(); ++i )
  {
    if ( (*i).first < (int)LHCb::Particle::LastGlobal )
    {
      const LHCb::Particle::additionalInfo info =
        static_cast<LHCb::Particle::additionalInfo>((*i).first);
      s << " " << info << "=" << (*i).second;
    }
    else
    {
      s << " " << (*i).first << "=" << (*i).second;
    }
  }
  return s << " ] }";
}

//=============================================================================
