// $Id: IParticleDescendants.h,v 1.2 2006-03-15 13:34:02 pkoppenb Exp $
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
  virtual const LHCb::Particle::ConstVector descendants(const LHCb::Particle* ) = 0 ;
  
  // Return all descendants of level i for a Particle
  virtual const LHCb::Particle::ConstVector descendants(const LHCb::Particle*, int ) = 0 ;
  
  // Return all stable descendants of a Particle
  virtual const LHCb::Particle::ConstVector finalStates(const LHCb::Particle* ) = 0 ;

protected:

private:

};
#endif // KERNEL_IPARTICLEDESCENDANTS_H
