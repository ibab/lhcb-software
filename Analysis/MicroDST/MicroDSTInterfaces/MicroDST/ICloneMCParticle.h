// $Id: ICloneMCParticle.h,v 1.2 2009-07-29 16:46:46 jpalac Exp $
#ifndef MICRODST_ICLONEMCPARTICLE_H 
#define MICRODST_ICLONEMCPARTICLE_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations

namespace LHCb 
{
  class MCParticle;
}

/** @class ICloneMCParticle ICloneMCParticle.h MicroDST/ICloneMCParticle.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-30
 */
class GAUDI_API ICloneMCParticle : virtual public IAlgTool {
public: 

  DeclareInterfaceID(ICloneMCParticle, 2, 0 );

  virtual LHCb::MCParticle* operator() (const LHCb::MCParticle* mcPart) = 0;

};
#endif // MICRODST_ICLONEMCPARTICLE_H
