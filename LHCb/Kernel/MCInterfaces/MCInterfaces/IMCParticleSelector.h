#ifndef MCINTERFACES_IMCPARTICLESELECTOR_H
#define MCINTERFACES_IMCPARTICLESELECTOR_H

#include "GaudiKernel/IAlgTool.h"

namespace LHCb {
  class MCParticle;
}

/** @class IMCParticleSelector IMCParticleSelector.h MCInterfaces/IMCParticleSelector.h
 *
 *  Interface Class for selection of particles given a criteria
 *
 *  @author M.Needham
 *  @date   13/3/2002
 */

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IMCParticleSelector("IMCParticleSelector", 0 , 0); 

class IMCParticleSelector : virtual public IAlgTool {

public: 

  /// Static access to interface id
  static const InterfaceID& interfaceID() { return IID_IMCParticleSelector; }
   
 /// select
 virtual bool accept(const LHCb::MCParticle* aParticle) const=0;

};

#endif // MCINTERFACES_IMCPARTICLESELECTOR_H

