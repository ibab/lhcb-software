// $Id: IParticleMaker.h,v 1.1.1.1 2004-08-24 06:59:45 pkoppenb Exp $
#ifndef DAVINCITOOLS_IPARTICLEMAKER_H 
#define DAVINCITOOLS_IPARTICLEMAKER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IParticleMaker("IParticleMaker", 1 , 0);

/** @class IParticleMaker IParticleMaker.h DaVinciTools/IParticleMaker.h
 *  Interface class to make Particles from other classes
 *  (e.g. ProtoParticles).
 *  This tools is internally used by the PhysDesktop.
 *
 *  @author Jose Helder Lopes
 *  @date   22/04/2002
 */
class IParticleMaker : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IParticleMaker; }
  
  /// Dispatch the makong of particles 
  virtual StatusCode makeParticles( ParticleVector & parts ) = 0;
  

};
#endif // DAVINCITOOLS_IPARTICLEMAKER_H
