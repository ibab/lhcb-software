// $Id: IParticle2MCAssociator.h,v 1.5 2009-02-02 14:33:13 jpalac Exp $
#ifndef KERNEL_IPARTICLE2MCASSOCIATOR_H 
#define KERNEL_IPARTICLE2MCASSOCIATOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
// from DaVinciMCKernel
#include "Kernel/Particle2MCParticle.h"

namespace LHCb
{
  class Particle;
  class MCParticle;
}


static const InterfaceID IID_IParticle2MCAssociator ( "IParticle2MCAssociator", 1, 0 );

/** @class IParticle2MCAssociator IParticle2MCAssociator.h Kernel/IParticle2MCAssociator.h
 *  
 *  Tool interface to generate weighted associations between one or many 
 *  LHCb::Particles and return them to the user in different formats. The
 *  user is in control of the resulting associations and the invoking interface
 *  methods should have no side-effects. TES access should be read-only.
 *  Interface implementations should not populate the TES with their results
 *  directly or indirectly, and should not have any runtime configuration
 *  that alters the result, except for the assumed TES location of the 
 *  LHCb::MCParticles for the relevant methods.
 * 
 *  @author Juan PALACIOS
 *  @date   2009-01-19
 */
class IParticle2MCAssociator : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IParticle2MCAssociator; }

  /**
   *
   * Calculate the weighted associations between an LHCb::Particle and
   * and some LHCb::MCParticles from the TES. The location handling is defined
   * by the implementation.
   * @param particle LHCb::Particle* to be associated
   * @return Particle2MCParticle::ToVector containing sorted weighted associations
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/
  virtual Particle2MCParticle::ToVector 
  associate(const LHCb::Particle* particle) const = 0;
  /**
   *
   * Calculate the weighted associations between an LHCb::Particle and
   * and some LHCb::MCParticles from a TES locaiton.
   * Return the result as Particle2MCParticle::ToVector
   * @param particle LHCb::Particle* to be associated
   * @param mcParticleLocation TES location of LHCb::MCParticles to base the
   * association on.
   * @return Particle2MCParticle::ToVector containing sorted weighted associations
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/
  virtual Particle2MCParticle::ToVector 
  associate(const LHCb::Particle* particle,
            const std::string& mcParticleLocation) const = 0;

  /**
   * Calculate and return the weighted associations between an 
   * LHCb::Particle and some LHCb::MCParticles from a TES locaiton.
   * @param particle LHCb::Particle* to be associated
   * @return Particle2MCParticle::LightTable Relations table containing 
   * sorted weighted relations between the LHCb::Particle and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/
  virtual Particle2MCParticle::LightTable 
  relatedMCPs(const LHCb::Particle*) const = 0;

  /**
   *
   * Calculate and return the weighted associations between an 
   * LHCb::Particle and some LHCb::MCParticles from a TES locaiton.
   * @param particle LHCb::Particle* to be associated
   * @param mcParticleLocation TES location of LHCb::MCParticles to base the
   * association on.
   * @return Particle2MCParticle::LightTable Relations table containing 
   * sorted weighted relations between the LHCb::Particle and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/
  virtual Particle2MCParticle::LightTable 
  relatedMCPs(const LHCb::Particle*,
              const std::string& mcParticleLocation) const = 0;


  /**
   *
   * Calculate and return the weighted associations between a container of
   * LHCb::Particles and a container of LHCb::MCParticles.
   * @param particles Container of LHCb::Particles to be associated
   * @param mcParticles Container of LHCb::MCParticles to base the
   * association on.
   * @return Particle2MCParticle::LightTable Relations table containing 
   * sorted weighted relations between the LHCb::Particles and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/ 
 virtual Particle2MCParticle::LightTable 
  relatedMCPs(const LHCb::Particle* particles,
              const LHCb::MCParticle::ConstVector& mcParticles) const = 0;

  /**
   *
   * Calculate and return the weighted associations between a container of  
   * LHCb::Particle and a container of LHCb::MCParticles on the TES
   * The TES location of the LHCb::MCParticles is handled by the 
   * implementations
   * @param particles Container of LHCb::Particles to be associated
   * @return Particle2MCParticle::LightTable Relations table containing 
   * sorted weighted relations between the LHCb::Particle and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/ 
  virtual Particle2MCParticle::LightTable 
  associations(const LHCb::Particle::ConstVector& particles) const = 0;

  /**
   *
   * Calculate and return the weighted associations between a container of
   * LHCb::Particles and a container of LHCb::MCParticles on the TES
   * @param particles Container of LHCb::Particles to be associated
   * @param mcParticleLocation TES location of LHCb::MCParticles to base the
   * association on.
   * @return Particle2MCParticle::LightTable Relations table containing 
   * sorted weighted relations between the LHCb::Particles and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/ 
  virtual Particle2MCParticle::LightTable 
  associations(const LHCb::Particle::ConstVector& particles,
               const std::string& mcParticleLocation) const = 0;

  /**
   *
   * Calculate and return the weighted associations between a container of  
   * LHCb::Particle and a container of LHCb::MCParticles on the TES
   * The TES location of the LHCb::MCParticles is handled by the 
   * implementations
   * @param particles Container of LHCb::Particles to be associated
   * @return Particle2MCParticle::LightTable Relations table containing 
   * sorted weighted relations between the LHCb::Particle and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/ 
  virtual Particle2MCParticle::LightTable 
  associations(const LHCb::Particle::ConstVector& particles,
               const LHCb::MCParticle::ConstVector& mcParticles) const = 0;

  /**
   *
   * Calculate and return the weighted associations between a container of  
   * LHCb::Particle and a container of LHCb::MCParticles on the TES
   * The TES location of the LHCb::MCParticles is handled by the 
   * implementations
   * @param particles Container of LHCb::Particles to be associated
   * @return Particle2MCParticle::LightTable Relations table containing 
   * sorted weighted relations between the LHCb::Particle and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/ 
  virtual Particle2MCParticle::LightTable 
  associations(const LHCb::Particle::Container& particles) const = 0;

  /**
   *
   * Calculate and return the weighted associations between a container of
   * LHCb::Particles and a container of LHCb::MCParticles on the TES
   * @param particles Container of LHCb::Particles to be associated
   * @param mcParticleLocation TES location of LHCb::MCParticles to base the
   * association on.
   * @return Particle2MCParticle::LightTable Relations table containing 
   * sorted weighted relations between the LHCb::Particles and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/ 
  virtual Particle2MCParticle::LightTable 
  associations(const LHCb::Particle::Container& particles,
               const std::string& mcParticleLocation) const = 0;

  /**
   *
   * Calculate and return the weighted associations between a container of  
   * LHCb::Particle and a container of LHCb::MCParticles on the TES
   * The TES location of the LHCb::MCParticles is handled by the 
   * implementations
   * @param particles Container of LHCb::Particles to be associated
   * @return Particle2MCParticle::LightTable Relations table containing 
   * sorted weighted relations between the LHCb::Particle and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/ 
  virtual Particle2MCParticle::LightTable 
  associations(const LHCb::Particle::Container& particles,
               const LHCb::MCParticle::ConstVector& mcParticles) const = 0;
  /**
   * Calculate the association weight between an LHCb::Particle and an
   * LHCb::MCParticle.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/
  virtual double 
  weight(const LHCb::Particle* particle, 
         const LHCb::MCParticle* mcParticle) const = 0;

};
#endif // KERNEL_IPARTICLE2MCASSOCIATOR_H
