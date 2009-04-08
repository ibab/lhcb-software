// $Id: Particle2MCParticle.cpp,v 1.2 2009-04-08 06:52:15 jpalac Exp $
// Include files 



// local
#include "Kernel/Particle2MCParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for functions in namespace : Particle2MCParticle
//
// 2009-04-06 : Juan PALACIOS
//-----------------------------------------------------------------------------
namespace Particle2MCParticle 
{
  
  ToVector 
  FilterMCAssociations(const ToVector& mcAssociations,
                       const LHCb::MCParticle::ConstVector& mcps) 
  {
    if (mcps.empty()) return Particle2MCParticle::ToVector();
    const Particle2MCParticle::NotInRange pred(&mcps);
    Particle2MCParticle::ToVector tmp(mcAssociations);
    Particle2MCParticle::ToVector::iterator new_last = 
      std::remove_if(tmp.begin(), tmp.end(), pred);
    return Particle2MCParticle::ToVector(tmp.begin(), new_last);
  }
  

}

//=============================================================================
