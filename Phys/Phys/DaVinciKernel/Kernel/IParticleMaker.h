// $Id: IParticleMaker.h,v 1.3 2006-06-18 14:30:27 jonrob Exp $
#ifndef DAVINCIKERNEL_IPARTICLEMAKER_H
#define DAVINCIKERNEL_IPARTICLEMAKER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"

static const InterfaceID IID_IParticleMaker("IParticleMaker", 1 , 0);

/** @class IParticleMaker IParticleMaker.h Kernel/IParticleMaker.h
 *  Interface class to make Particles from other classes
 *  (e.g. ProtoParticles).
 *  This tools is internally used by the PhysDesktop.
 *
 *  @author Jose Helder Lopes
 *  @date   22/04/2002
 */

class IParticleMaker : virtual public IAlgTool
{

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IParticleMaker; }

  /// Dispatch the making of particles
  virtual StatusCode makeParticles( LHCb::Particle::ConstVector & parts ) = 0;

};
#endif // DAVINCIKERNEL_IPARTICLEMAKER_H
