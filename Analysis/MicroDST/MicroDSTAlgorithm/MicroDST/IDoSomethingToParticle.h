// $Id: IDoSomethingToParticle.h,v 1.1 2007-11-02 16:26:16 jpalac Exp $
#ifndef MICRODST_IDOSOMETHINGTOPARTICLE_H 
#define MICRODST_IDOSOMETHINGTOPARTICLE_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IDoSomethingToParticle ( "IDoSomethingToParticle", 1, 0 );

/** @class IDoSomethingToParticle IDoSomethingToParticle.h MicroDST/IDoSomethingToParticle.h
 *  
 *
 * 
 *  Interface to add, extract or modify a particle that has 
 *  not yet been stored in the TES.
 *  
 *  @author Juan PALACIOS
 *  @date   2007-11-01
 */
class IDoSomethingToParticle : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { 
    return IID_IDoSomethingToParticle; 
  }

  virtual StatusCode operator()  (LHCb::Particle* particle) = 0;

  virtual StatusCode doSomething (LHCb::Particle* particle) =0;
  

protected:

private:

};
#endif // MICRODST_IDOSOMETHINGTOPARTICLE_H
