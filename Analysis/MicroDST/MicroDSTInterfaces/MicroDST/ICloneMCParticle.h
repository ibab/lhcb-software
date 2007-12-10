// $Id: ICloneMCParticle.h,v 1.1.1.1 2007-12-10 08:23:45 jpalac Exp $
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

static const InterfaceID IID_ICloneMCParticle ( "ICloneMCParticle", 1, 0 );

/** @class ICloneMCParticle ICloneMCParticle.h MicroDST/ICloneMCParticle.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-30
 */
class ICloneMCParticle : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICloneMCParticle; }

  virtual LHCb::MCParticle* operator() (const LHCb::MCParticle* mcPart) = 0;
  
protected:

private:

};
#endif // MICRODST_ICLONEMCPARTICLE_H
