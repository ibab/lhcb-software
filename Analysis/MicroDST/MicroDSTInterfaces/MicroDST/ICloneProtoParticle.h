// $Id: ICloneProtoParticle.h,v 1.3 2009-07-29 21:29:01 jpalac Exp $
#ifndef MICRODST_ICLONEPROTOPARTICLE_H 
#define MICRODST_ICLONEPROTOPARTICLE_H 1
//
#include "MicroDST/ICloner.h"
// Forward declarations
namespace LHCb 
{
  class ProtoParticle;
}

/** @class ICloneProtoParticle MicroDST/ICloneProtoParticle.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-12-05
 */
class GAUDI_API ICloneProtoParticle : virtual public MicroDST::ICloner<LHCb::ProtoParticle>
{

public: 

  /// Interface ID
  DeclareInterfaceID(ICloneProtoParticle, 2, 0 );

  /// Destructor
  virtual ~ICloneProtoParticle() { }

};

#endif // MICRODST_ICLONEPROTOPARTICLE_H
