// $Id: IParticle2MCAssociator.h,v 1.1 2009-01-19 18:18:28 jpalac Exp $
#ifndef KERNEL_IPARTICLE2MCASSOCIATOR_H 
#define KERNEL_IPARTICLE2MCASSOCIATOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Relations/RelationWeighted.h"
// from DaVinciMCKernel
#include "Kernel/MCAssociation.h"

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

  typedef Relations::RelationWeighted<LHCb::Particle, LHCb::MCParticle, double> Particle2MCTable;

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IParticle2MCAssociator; }

  virtual MCAssociation association(const LHCb::Particle*,
                                    const std::string& mcParticleLocation) const = 0;

  virtual Particle2MCTable associations(const LHCb::Particle::ConstVector& particles,
                                        const std::string& mcParticleLocation) const = 0;

  virtual Particle2MCTable associations(const LHCb::Particle::Container& particles,
                                        const std::string& mcParticleLocation) const = 0;

  virtual Particle2MCTable associations(const LHCb::Particle::ConstVector& particles,
                                        const LHCb::MCParticle::ConstVector& mcParticles) const = 0;

  virtual Particle2MCTable associations(const LHCb::Particle::Container& particles,
                                        const LHCb::MCParticle::ConstVector& mcParticles) const = 0;

  
};
#endif // KERNEL_IPARTICLE2MCASSOCIATOR_H
