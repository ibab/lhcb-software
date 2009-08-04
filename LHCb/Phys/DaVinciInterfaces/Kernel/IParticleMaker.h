// $Id: IParticleMaker.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef DAVINCIKERNEL_IPARTICLEMAKER_H
#define DAVINCIKERNEL_IPARTICLEMAKER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"

/** @class IParticleMaker IParticleMaker.h Kernel/IParticleMaker.h
 *  Interface class to make Particles from other classes
 *  (e.g. ProtoParticles).
 *  This tools is internally used by the PhysDesktop.
 *
 *  @author Jose Helder Lopes
 *  @date   22/04/2002
 */

class GAUDI_API IParticleMaker : virtual public IAlgTool
{

public:

  DeclareInterfaceID(IParticleMaker, 2, 0);

  /// Dispatch the making of particles
  virtual StatusCode makeParticles( LHCb::Particle::ConstVector & parts ) = 0;

};
#endif // DAVINCIKERNEL_IPARTICLEMAKER_H
