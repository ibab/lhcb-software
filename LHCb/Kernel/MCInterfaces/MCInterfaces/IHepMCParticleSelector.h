#ifndef MCINTERFACES_IHEPMCPARTICLESELECTOR_H
#define MCINTERFACES_IHEPMCPARTICLESELECTOR_H

#include "GaudiKernel/IAlgTool.h"

namespace HepMC {
  class GenParticle;
}

/** @class IHepMCParticleSelector IHepMCParticleSelector.h MCInterfaces/IHepMCParticleSelector.h
 *
 *  Interface Class for selection of particles given a criteria
 *
 *  @author Victor Coco victor.coco@cern.ch
 *  @date   22.10.2009
 */

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_IHepMCParticleSelector("IHepMCParticleSelector", 0 , 0);

class IHepMCParticleSelector : virtual public IAlgTool {

public:

  /// Static access to interface id
  static const InterfaceID& interfaceID() { return IID_IHepMCParticleSelector; }

  /** Method to select or reject a given HepMCParticle
   *  @param aParticle Pointer to the HepMCParticle to consider
   *  @return Boolean indicating if the given HepMCParticle is selected or not
   *  @retval true  HepMCParticle is selected
   *  @retval false HepMCParticle is rejected
   */
  virtual bool accept( const HepMC::GenParticle* aParticle ) const = 0;

  /** Boolean operator to select or reject a given HepMCParticle
   *  @param aParticle Pointer to the HepMCParticle to consider
   *  @return Boolean indicating if the given HepMCParticle is selected or not
   *  @retval true  HepMCParticle is selected
   *  @retval false HepMCParticle is rejected
   */
  inline bool operator() ( const HepMC::GenParticle* aParticle ) const
  {
    return this->accept(aParticle);
  }

};

#endif // MCINTERFACES_IMCPARTICLESELECTOR_H
