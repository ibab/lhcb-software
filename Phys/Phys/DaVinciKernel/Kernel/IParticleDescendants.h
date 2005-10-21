// $Id: IParticleDescendants.h,v 1.1 2005-10-21 14:47:04 pkoppenb Exp $
#ifndef KERNEL_IPARTICLEDESCENDANTS_H 
#define KERNEL_IPARTICLEDESCENDANTS_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"

static const InterfaceID IID_IParticleDescendants ( "IParticleDescendants", 1, 0 );

/** @class IParticleDescendants IParticleDescendants.h Kernel/IParticleDescendants.h
 *  
 *  Return the descendants of a Particle
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-10-19
 */
class IParticleDescendants : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IParticleDescendants; }

  // Return all descendants of a Particle
  virtual const ParticleVector descendants(const Particle* ) = 0 ;
  
  // Return all descendants of level i for a Particle
  virtual const ParticleVector descendants(const Particle*, int ) = 0 ;
  
  // Return all stable descendants of a Particle
  virtual const ParticleVector finalStates(const Particle* ) = 0 ;

protected:

private:

};
#endif // KERNEL_IPARTICLEDESCENDANTS_H
