#ifndef MCEVENT_MCFUN_H
#define MCEVENT_MCFUN_H 1
 
#include "Event/MCVertex.h"
#include "Event/MCParticle.h"

namespace LHCb{
  namespace MC{
  
    /** z of the first interaction vertex of a particle 
    * Silly interactions PhotoElectric, RICHPhotoElectric
                         Cerenkov and DeltaRay are ignored
    * @param particle
    * @return z of first interaction 
    */ 
    double zInteraction(const LHCb::MCParticle* aParticle);
 
    /** 
    * Silly interactions  PhotoElectric, RICHPhotoElectric
    *                     Cerenkov and DeltaRay return false
    *  @param type
    *  @return true if not a silly vertex  
    */
    bool realInteraction(const LHCb::MCVertex::MCVertexType& type);
  }
}

#endif // MCEVENT_MCFUN_H
