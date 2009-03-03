// $Id: Particle2MCParticle.h,v 1.3 2009-03-03 16:05:38 jpalac Exp $
#ifndef KERNEL_PARTICLE2MCPARTICLE_H 
#define KERNEL_PARTICLE2MCPARTICLE_H 1

// Include files
#include "Event/MCParticle.h"
#include "Event/Particle.h"
#include "Relations/Relation.h"
#include "Relations/Relation1D.h"
#include "Kernel/MCAssociation.h"

/** @namespace Particle2MCParticle Kernel/Particle2MCParticle.h
 *  
 *
 *  Namespace containing types corresponding to the Particle -> MCParticle 
 *  uni-directional weighted relationship.
 *
 *  @author Juan PALACIOS
 *  @date   2009-01-20
 */
namespace Particle2MCParticle {

  typedef LHCb::Relation1D<LHCb::Particle, 
                           LHCb::MCParticle  >            Table      ;

  typedef Relations::Relation<LHCb::Particle, 
                              LHCb::MCParticle >          LightTable ;
  typedef Table::Range                                    Range      ;
  typedef Table::IBase::TypeTraits::Entry                 Relation   ;
  typedef Table::IBase::TypeTraits::Entries               Relations  ;
  typedef Table::To                                       To         ;
  typedef Table::From                                     From       ;

  typedef std::vector<MCAssociation>                      ToVector;
  
}
#endif // KERNEL_PARTICLE2MCPARTICLE_H
