// $Id: ICaloParticleMaker.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef DAVINCIKERNEL_ICALOPARTICLEMAKER_H
#define DAVINCIKERNEL_ICALOPARTICLEMAKER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"

// Forward declarations
namespace LHCb {
  class Vertex;
}

/** @class ICaloParticleMaker ICaloParticleMaker.h Kernel/ICaloParticleMaker.h
 *  Interface class to make Particles from other classes
 *  (e.g. ProtoParticles).
 *  This tools is internally used by the PhysDesktop.
 *
 *  @author Jose Helder Lopes
 *  @date   22/04/2002
 */

class GAUDI_API ICaloParticleMaker : virtual public IAlgTool
{

public:

  DeclareInterfaceID(ICaloParticleMaker, 2, 0);

  /// Dispatch the making of particles
  virtual StatusCode makeParticles( LHCb::Particle::Vector & parts ) = 0;
  virtual void setPoint ( const Gaudi::XYZPoint cov )  = 0;
  virtual void setPoint ( const Gaudi::XYZPoint pos, const Gaudi::SymMatrix3x3 cov )  = 0;
  virtual void setPoint ( const LHCb::Vertex* vert )  = 0;
};
#endif // DAVINCIKERNEL_ICALOPARTICLEMAKER_H
