// $Id: IDaVinciSmartAssociator.h,v 1.3 2009-02-20 22:41:54 gligorov Exp $
#ifndef IDAVINCISMARTASSOCIATOR_H 
#define IDAVINCISMARTASSOCIATOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Kernel/Particle2MCLinker.h"
#include "Kernel/IParticle2MCAssociator.h"
#include "Kernel/Particle2MCAssociatorBase.h"

// forward declarations
namespace LHCb
{
  class Particle;
}

static const InterfaceID IID_IDaVinciSmartAssociator ( "IDaVinciSmartAssociator", 1, 0 );

/** @class IDaVinciSmartAssociator IDaVinciSmartAssociator.h
 * 
 *  "Smart" associator for any kind of particle; to use just pass the particle
 *  to the method, returns a ToRange object containing all the associated  
 *  MCParticles.
 * 
 *  @author V. Gligorov
 *  @date   2009-01-13
 */
class IDaVinciSmartAssociator : virtual public IAlgTool {

public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IDaVinciSmartAssociator; }

  //virtual Particle2MCLinker::ToRange associate(const LHCb::Particle*) = 0;

  virtual Particle2MCParticle::ToVector
  associate(const LHCb::Particle*) const ;

  virtual Particle2MCParticle::ToVector
  associate(const LHCb::Particle*,
            const std::string&) const ;

  virtual double
  weight(const LHCb::Particle*,
         const LHCb::MCParticle*) const ;

};
#endif // IDAVINCISMARTASSOCIATOR_H
