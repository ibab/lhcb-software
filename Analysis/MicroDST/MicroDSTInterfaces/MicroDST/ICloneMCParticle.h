// $Id: ICloneMCParticle.h,v 1.3 2009-07-29 21:29:01 jpalac Exp $
#ifndef MICRODST_ICLONEMCPARTICLE_H 
#define MICRODST_ICLONEMCPARTICLE_H 1
// from MicroDST
#include "MicroDST/ICloner.h"

// Forward declarations

namespace LHCb 
{
  class MCParticle;
}

/** @class ICloneMCParticle MicroDST/ICloneMCParticle.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-30
 */
class GAUDI_API ICloneMCParticle : virtual public MicroDST::ICloner<LHCb::MCParticle> 
{

public:

  /// Interface ID
  DeclareInterfaceID(ICloneMCParticle, 2, 0 );

  /// Destructor
  virtual ~ICloneMCParticle() { }

};

#endif // MICRODST_ICLONEMCPARTICLE_H
