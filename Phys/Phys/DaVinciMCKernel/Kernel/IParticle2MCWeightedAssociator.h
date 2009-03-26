// $Id: IParticle2MCWeightedAssociator.h,v 1.3 2009-03-26 12:17:10 jpalac Exp $
#ifndef KERNEL_IPARTICLE2MCWEIGHTEDASSOCIATOR_H 
#define KERNEL_IPARTICLE2MCWEIGHTEDASSOCIATOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

//
#include "Kernel/Particle2MCParticle.h"

static const InterfaceID IID_IParticle2MCWeightedAssociator ( "IParticle2MCWeightedAssociator", 1, 0 );

/** @class IParticle2MCWeightedAssociator IParticle2MCWeightedAssociator.h Kernel/IParticle2MCWeightedAssociator.h
 *  
 *  Tool interface to generate weighted associations between an 
 *  LHCb::Particle and return them to the user. The
 *  user is in control of the resulting associations
 *  Interface implementations should not populate the TES with their results,
 *  and should not have any runtime configuration
 *  that alters the result, except for the assumed TES location of the 
 *  LHCb::MCParticles for the relevant methods.
 *
 *  @author Juan PALACIOS
 *  @date   2009-03-25
 */
class IParticle2MCWeightedAssociator : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IParticle2MCWeightedAssociator; }

  /**
   *
   * Calculate the weighted associations between an LHCb::Particle and
   * and some LHCb::MCParticles from the TES. The location handling is defined
   * by the implementation.
   * @param particle LHCb::Particle* to be associated
   * @return Particle2MCParticle::ToVector containing sorted weighted associations
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @author Vladimir Gligorov
   * @date   2009-02-02
   *
   **/
  virtual Particle2MCParticle::ToVector 
  relatedMCPs(const LHCb::Particle* particle) const = 0;

  /**
   *
   * Calculate the weighted associations between an LHCb::Particle and
   * and some LHCb::MCParticles
   * @param particle LHCb::Particle* to be associated
   * @param mcParticleLocation TES location of LHCb::MCParticle::Container
   *         from which to associate
   * @return Particle2MCParticle::ToVector containing sorted weighted 
   *         associations
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/
  virtual Particle2MCParticle::ToVector 
  relatedMCPs(const LHCb::Particle* particle,
              const std::string& mcParticleLocation) const = 0;

  /**
   *
   * Calculate the weighted associations between an LHCb::Particle and
   * and some LHCb::MCParticles
   * @param particle LHCb::Particle* to be associated
   * @param mcParticles Container of LHCb::MCParticle from which to associate
   * @return Particle2MCParticle::ToVector containing sorted weighted associations
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/
  virtual Particle2MCParticle::ToVector 
  relatedMCPs(const LHCb::Particle* particle,
              const LHCb::MCParticle::Container& mcParticles) const = 0;
  /**
   *
   * Calculate the weighted associations between an LHCb::Particle and
   * and some LHCb::MCParticles
   * @param particle LHCb::Particle* to be associated
   * @param mcParticles Container of LHCb::MCParticle from which to associate
   * @return Particle2MCParticle::ToVector containing sorted weighted associations
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/
  virtual Particle2MCParticle::ToVector 
  relatedMCPs(const LHCb::Particle* particle,
              const LHCb::MCParticle::ConstVector& mcParticles) const = 0;

  /**
   *
   * Return the association weight between an LHCb::Particle and an
   * LHCb::MCParticle
   *
   * @param particle pointer to LHCb::Particle to be associated
   * @param mcParticle pointer to LHCb::MCParticle to be associated
   * @return weight of the association. It should be a probability, therefore
   *         it is bounded by 0 and 1.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-26-03
   **/
  virtual double 
  associationWeight(const LHCb::Particle* particle,
                    const LHCb::MCParticle* mcParticle) const = 0;

};
#endif // KERNEL_IPARTICLE2MCWEIGHTEDASSOCIATOR_H
