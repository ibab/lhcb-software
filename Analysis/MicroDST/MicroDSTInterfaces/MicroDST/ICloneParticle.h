// $Id: ICloneParticle.h,v 1.3 2009-07-29 21:29:01 jpalac Exp $
#ifndef MICRODST_ICLONEPARTICLE_H 
#define MICRODST_ICLONEPARTICLE_H 1

// Include files
// from STL
#include <string>

// from MicroDST
#include "MicroDST/ICloner.h"
namespace LHCb 
{
  class Particle;
}

/** @class ICloneParticle MicroDST/ICloneParticle.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-30
 */

class GAUDI_API ICloneParticle : virtual public MicroDST::ICloner<LHCb::Particle> 
{

public: 

  /// Interface ID
  DeclareInterfaceID(ICloneParticle, 2, 0 );

  /// Destructor
  virtual ~ICloneParticle() { }

};

#endif // MICRODST_ICLONEPARTICLE_H
