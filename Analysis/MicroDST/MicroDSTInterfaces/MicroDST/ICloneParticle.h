// $Id: ICloneParticle.h,v 1.1.1.1 2007-12-10 08:23:45 jpalac Exp $
#ifndef MICRODST_ICLONEPARTICLE_H 
#define MICRODST_ICLONEPARTICLE_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace LHCb 
{
  class Particle;
}


static const InterfaceID IID_ICloneParticle ( "ICloneParticle", 1, 0 );

/** @class ICloneParticle ICloneParticle.h MicroDST/ICloneParticle.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-30
 */
class ICloneParticle : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICloneParticle; }

  virtual LHCb::Particle* operator() (const LHCb::Particle* particle) = 0;

protected:

private:

};
#endif // MICRODST_ICLONEPARTICLE_H
