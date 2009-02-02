// $Id: IParticle2MCAssociator.h,v 1.4 2009-02-02 13:03:45 jpalac Exp $
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
 *
 *  @author Juan PALACIOS
 *  @date   2009-01-19
 */
class IParticle2MCAssociator : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IParticle2MCAssociator; }
  virtual Particle2MCParticle::ToVector 
  associate(const LHCb::Particle* particle) const = 0;

  virtual Particle2MCParticle::ToVector 
  associate(const LHCb::Particle* particle,
            const std::string& mcParticleLocation) const = 0;

  virtual Particle2MCParticle::LightTable 
  relatedMCPs(const LHCb::Particle*) const = 0;

  virtual Particle2MCParticle::LightTable 
  relatedMCPs(const LHCb::Particle*,
              const std::string& mcParticleLocation) const = 0;
  
  virtual Particle2MCParticle::LightTable 
  relatedMCPs(const LHCb::Particle*,
              const LHCb::MCParticle::ConstVector& mcParticles) const = 0;

  virtual Particle2MCParticle::LightTable 
  associations(const LHCb::Particle::ConstVector& particles) const = 0;
  
  virtual Particle2MCParticle::LightTable 
  associations(const LHCb::Particle::ConstVector& particles,
               const std::string& mcParticleLocation) const = 0;

  virtual Particle2MCParticle::LightTable 
  associations(const LHCb::Particle::ConstVector& particles,
               const LHCb::MCParticle::ConstVector& mcParticles) const = 0;

  virtual Particle2MCParticle::LightTable 
  associations(const LHCb::Particle::Container& particles) const = 0;

  virtual Particle2MCParticle::LightTable 
  associations(const LHCb::Particle::Container& particles,
               const std::string& mcParticleLocation) const = 0;

  virtual Particle2MCParticle::LightTable 
  associations(const LHCb::Particle::Container& particles,
               const LHCb::MCParticle::ConstVector& mcParticles) const = 0;

  virtual double 
  weight(const LHCb::Particle* particle, 
         const LHCb::MCParticle* mcParticle) const = 0;

};
#endif // KERNEL_IPARTICLE2MCASSOCIATOR_H
