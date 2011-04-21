// $Id: IParticleDescendants.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef KERNEL_IPARTICLEDESCENDANTS_H 
#define KERNEL_IPARTICLEDESCENDANTS_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"

/** @class IParticleDescendants IParticleDescendants.h Kernel/IParticleDescendants.h
 *  
 *  Return the descendants of a Particle
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-10-19
 */
class GAUDI_API IParticleDescendants : virtual public IAlgTool {
public: 

  DeclareInterfaceID(IParticleDescendants, 2, 0);

  // Return all descendants of a Particle
  virtual const LHCb::Particle::ConstVector descendants(const LHCb::Particle* ) = 0 ;
  
  // Return all descendants of level i for a Particle
  virtual const LHCb::Particle::ConstVector descendants(const LHCb::Particle*, int ) = 0 ;
  
  // Return all stable descendants of a Particle
  virtual const LHCb::Particle::ConstVector finalStates(const LHCb::Particle* ) = 0 ;

protected:

private:

};
#endif // KERNEL_IPARTICLEDESCENDANTS_H
