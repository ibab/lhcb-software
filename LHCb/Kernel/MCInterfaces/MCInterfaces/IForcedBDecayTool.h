// $Id: IForcedBDecayTool.h,v 1.1.1.1 2007-05-25 16:24:33 cattanem Exp $
#ifndef IFORCEDBDECAYTOOL_H 
#define IFORCEDBDECAYTOOL_H 1

#include <string>
#include "GaudiKernel/IAlgTool.h"

namespace LHCb {
  class MCParticle;
}

static const InterfaceID IID_IForcedBDecayTool("IForcedBDecayTool", 1 , 0); 

/** @class IForcedBDecayTool IForcedBDecayTool.h 
 *  
 *  v1.2
 *  @author Marco Musy (Milano)
 *  @date   2007-02-06
 */
class IForcedBDecayTool : virtual public IAlgTool {
public:
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IForcedBDecayTool; };
 
  virtual const LHCb::MCParticle*  forcedB( void )=0;

};
#endif // IFORCEDBDECAYTOOL_H
