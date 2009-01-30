// $Id: GenericParticle2MCAssociator.h,v 1.2 2009-01-30 18:54:41 jpalac Exp $
#ifndef GENERICPARTICLE2MCASSOCIATOR_H 
#define GENERICPARTICLE2MCASSOCIATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticle2MCAssociator.h"            // Interface
#include "Kernel/Particle2MCAssociatorBase.h"         // Common base class


/** @class GenericParticle2MCAssociator GenericParticle2MCAssociator.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2009-01-20
 */
template <typename AssocLogic>
class GenericParticle2MCAssociator : public Particle2MCAssociatorBase, 
                                     virtual public IParticle2MCAssociator {
public: 
  /// Standard constructor
  GenericParticle2MCAssociator( const std::string& type, 
                                const std::string& name,
                                const IInterface* parent)
    :
    Particle2MCAssociatorBase(type, name, parent)
  {
    declareInterface<IParticle2MCAssociator>(this);
  }

  virtual ~GenericParticle2MCAssociator( ) {} ///< Destructor
  
  virtual double weight(const LHCb::Particle* particle, 
                        const LHCb::Particle* mcParticle) const 
  {
    return AssocLogic::weight(particle, mcParticle);
  }

  //===========================================================================
  // finalize
  //===========================================================================
  StatusCode DaVinciSmartAssociator::finalize() {
    return Particle2MCAssociatorBase::finalize() ; 
  }

};
#endif // GENERICPARTICLE2MCASSOCIATOR_H
