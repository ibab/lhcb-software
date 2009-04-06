// $Id: Particle2MCParticle.cpp,v 1.1 2009-04-06 13:05:36 jpalac Exp $
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
    const Particle2MCParticle::NotInRange pred(&mcps);
    Particle2MCParticle::ToVector tmp(mcAssociations);
    Particle2MCParticle::ToVector::iterator new_last = 
      std::remove_if(tmp.begin(), tmp.end(), pred);
    return Particle2MCParticle::ToVector(tmp.begin(), new_last);
  }
  

}

//=============================================================================
