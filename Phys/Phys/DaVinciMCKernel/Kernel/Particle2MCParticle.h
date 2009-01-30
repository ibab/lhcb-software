// $Id: Particle2MCParticle.h,v 1.2 2009-01-30 18:36:51 jpalac Exp $
#ifndef KERNEL_PARTICLE2MCPARTICLE_H 
#define KERNEL_PARTICLE2MCPARTICLE_H 1

// Include files
#include "Event/MCParticle.h"
#include "Event/Particle.h"
#include "Relations/RelationWeighted.h"
#include "Relations/RelationWeighted1D.h"
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
  typedef double                                          WeightType ;
  typedef LHCb::RelationWeighted1D<LHCb::Particle, 
                                   LHCb::MCParticle, 
                                   WeightType>            Table      ;

  typedef Relations::RelationWeighted<LHCb::Particle, 
                                      LHCb::MCParticle, 
                                      WeightType>         LightTable ;
  typedef Table::Range                                    Range      ;
  typedef Table::IBase::TypeTraits::Entry                 Relation   ;
  typedef Table::IBase::TypeTraits::Entries               Relations  ;
  typedef Table::To                                       To         ;
  typedef Table::From                                     From       ;
  typedef WeightType                                      Weight     ;

  typedef std::vector<MCAssociation>                      ToVector;
  
}
#endif // KERNEL_PARTICLE2MCPARTICLE_H
