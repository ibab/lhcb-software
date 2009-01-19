// $Id: IDaVinciSmartAssociator.h,v 1.2 2009-01-19 18:15:51 jpalac Exp $
#ifndef IDAVINCISMARTASSOCIATOR_H 
#define IDAVINCISMARTASSOCIATOR_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Kernel/Particle2MCLinker.h"

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

  virtual Particle2MCLinker::ToRange associate(const LHCb::Particle*) = 0;

};
#endif // IDAVINCISMARTASSOCIATOR_H
