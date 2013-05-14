#ifndef IBDECAYTOOL_H 
#define IBDECAYTOOL_H 1

#include <string>
#include "GaudiKernel/IAlgTool.h"

namespace LHCb {
  class MCParticle;
}

static const InterfaceID IID_IBDecayTool("IBDecayTool", 1 , 0); 

/** @class IBDecayTool IBDecayTool.h 
 *  
 */

class IBDecayTool : virtual public IAlgTool {
public:
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IBDecayTool; };
 
  virtual const LHCb::MCParticle*  forcedB( void )=0;

};
#endif // IBDECAYTOOL_H
