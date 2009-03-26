// $Id: Particle2MCParticle.h,v 1.4 2009-03-26 17:50:31 jpalac Exp $
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


  struct SumWeights : public std::binary_function<MCAssociation, MCAssociation, double>
  {
    double operator() (const double& x,
                       const MCAssociation& assoc) const
    {
      return x + assoc.weight();
    }
  };
    
    struct SortByWeight : public std::binary_function<const MCAssociation, const MCAssociation, bool>
    {
      bool operator() (const MCAssociation& assoc1,
                       const MCAssociation& assoc2) const
      {
        return assoc1.weight() < assoc2.weight();
      }
    
    };
}
#endif // KERNEL_PARTICLE2MCPARTICLE_H
