// $Id: ICloneProtoParticle.h,v 1.1.1.1 2007-12-10 08:23:45 jpalac Exp $
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

static const InterfaceID IID_ICloneProtoParticle ( "ICloneProtoParticle", 1, 0 );

/** @class ICloneProtoParticle ICloneProtoParticle.h MicroDST/ICloneProtoParticle.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-12-05
 */
class ICloneProtoParticle : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICloneProtoParticle; }

  virtual LHCb::ProtoParticle* operator() (const LHCb::ProtoParticle* proto) = 0;

protected:

private:

};
#endif // MICRODST_ICLONEPROTOPARTICLE_H
