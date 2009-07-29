// $Id: ICloneParticle.h,v 1.2 2009-07-29 16:46:46 jpalac Exp $
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

/** @class ICloneParticle ICloneParticle.h MicroDST/ICloneParticle.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-30
 */
class GAUDI_API ICloneParticle : virtual public IAlgTool {
public: 
  DeclareInterfaceID(ICloneParticle, 2, 0 );

  virtual LHCb::Particle* operator() (const LHCb::Particle* particle) = 0;

};
#endif // MICRODST_ICLONEPARTICLE_H
