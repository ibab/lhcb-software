// $Id: ICaloParticleMaker.h,v 1.4 2009-04-21 19:01:36 pkoppenb Exp $
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

static const InterfaceID IID_ICaloParticleMaker("ICaloParticleMaker", 1 , 0);

/** @class ICaloParticleMaker ICaloParticleMaker.h Kernel/ICaloParticleMaker.h
 *  Interface class to make Particles from other classes
 *  (e.g. ProtoParticles).
 *  This tools is internally used by the PhysDesktop.
 *
 *  @author Jose Helder Lopes
 *  @date   22/04/2002
 */

class ICaloParticleMaker : virtual public IAlgTool
{

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloParticleMaker; }

  /// Dispatch the making of particles
  virtual StatusCode makeParticles( LHCb::Particle::Vector & parts ) = 0;
  virtual void setPoint ( const Gaudi::XYZPoint cov )  = 0;
  virtual void setPoint ( const Gaudi::XYZPoint pos, const Gaudi::SymMatrix3x3 cov )  = 0;
  virtual void setPoint ( const LHCb::Vertex* vert )  = 0;
};
#endif // DAVINCIKERNEL_ICALOPARTICLEMAKER_H
