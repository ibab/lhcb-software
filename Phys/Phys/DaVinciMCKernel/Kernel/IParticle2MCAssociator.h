// $Id: IParticle2MCAssociator.h,v 1.11 2009-10-15 14:07:30 jpalac Exp $
#ifndef KERNEL_IPARTICLE2MCASSOCIATOR_H 
#define KERNEL_IPARTICLE2MCASSOCIATOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/MCParticle.h"
namespace LHCb
{
  class Particle;
  class MCParticle;
}




/** @class IParticle2MCAssociator IParticle2MCAssociator.h Kernel/IParticle2MCAssociator.h
 *  
 *  Tool interface to generate a single association between an 
 *  LHCb::Particle and return them to the user. The
 *  user is in control of the resulting association and invoking the interface
 *  methods should have no side-effects. TES access should be read-only.
 *  Interface implementations should not populate the TES with their results
 *  directly or indirectly, and should not have any runtime configuration
 *  that alters the result, except for the assumed TES location of the 
 *  LHCb::MCParticles for the relevant methods.
 * 
 *  @author Juan PALACIOS
 *  @date   2009-01-19
 */
class GAUDI_API IParticle2MCAssociator : virtual public IAlgTool {
public: 

DeclareInterfaceID( IParticle2MCAssociator, 1, 0 );

  /**
   * Calculate and return the LHCb::MCParticle associated to an
   * LHCb::Particle. The associated LHCb::Particle is selected from a
   * container of LHCb::MCParticles from a TES location.
   * @param particle LHCb::Particle* to be associated
   * @return const LHCb::MCParticle* to associated LHCb::MCParticle
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-03-25
   *
   **/
  virtual const LHCb::MCParticle* 
  relatedMCP(const LHCb::Particle*) const = 0;


  /**
   * Calculate and return the LHCb::MCParticle associated to an
   * LHCb::Particle. The associated LHCb::Particle is selected from a
   * container of LHCb::MCParticles from a TES location.
   * @param particle LHCb::Particle* to be associated
   * @return const LHCb::MCParticle* to associated LHCb::MCParticle
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-05-07
   *
   **/
  virtual const LHCb::MCParticle* 
  operator() (const LHCb::Particle*) const = 0;

  /**
   * Calculate and return the LHCb::MCParticle associated to an
   * LHCb::Particle. The associated LHCb::Particle is selected from a
   * container of LHCb::MCParticles from a TES location.
   * @param particle LHCb::Particle* to be associated
   * @param mcParticleLocation TES location of MCParticles
   * @return const LHCb::MCParticle* to associated LHCb::MCParticle
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-03-25
   *
   **/
  virtual const LHCb::MCParticle*
  relatedMCP(const LHCb::Particle*,
             const std::string& mcParticleLocation) const = 0;

  /**
   * Calculate and return the LHCb::MCParticle associated to an
   * LHCb::Particle. The associated LHCb::Particle is selected from a
   * container of LHCb::MCParticles.
   * @param particle LHCb::Particle* to be associated
   * @param mcParticles Container of MCParticles
   * @return const LHCb::MCParticle* to associated LHCb::MCParticle
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-03-25
   *
   **/
  virtual const LHCb::MCParticle*
  relatedMCP(const LHCb::Particle* particles,
             const LHCb::MCParticle::ConstVector& mcParticles) const = 0;

  /**
   * Calculate and return the LHCb::MCParticle associated to an
   * LHCb::Particle. The associated LHCb::Particle is selected from a
   * container of LHCb::MCParticles.
   * @param particle LHCb::Particle* to be associated
   * @param mcParticles Container of MCParticles
   * @return const LHCb::MCParticle* to associated LHCb::MCParticle
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-03-25
   *
   **/
  virtual const LHCb::MCParticle*
  relatedMCP(const LHCb::Particle* particles,
             const LHCb::MCParticle::Container& mcParticles) const = 0;


};
#endif // KERNEL_IPARTICLE2MCASSOCIATOR_H
