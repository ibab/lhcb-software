// $Id: ICloneProtoParticle.h,v 1.2 2009-07-29 16:46:46 jpalac Exp $
#ifndef MICRODST_ICLONEPROTOPARTICLE_H 
#define MICRODST_ICLONEPROTOPARTICLE_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb 
{
  class ProtoParticle;
}

/** @class ICloneProtoParticle ICloneProtoParticle.h MicroDST/ICloneProtoParticle.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-12-05
 */
class GAUDI_API ICloneProtoParticle : virtual public IAlgTool {
public: 

  DeclareInterfaceID(ICloneProtoParticle, 2, 0 );

  virtual LHCb::ProtoParticle* operator() (const LHCb::ProtoParticle* proto) = 0;

protected:

private:

};
#endif // MICRODST_ICLONEPROTOPARTICLE_H
