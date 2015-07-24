#ifndef IFLAGSIGNALCHAIN_H 
#define IFLAGSIGNALCHAIN_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace LHCb {
  class MCParticle;
}

static const InterfaceID IID_IFlagSignalChain ( "IFlagSignalChain", 1, 0 );

/** @class IFlagSignalChain IFlagSignalChain.h
 *  
 *
 *  @author Gloria Corti
 *  @date   2015-07-23
 */
class IFlagSignalChain : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IFlagSignalChain; }

  /// Set flag if particle is signal or from signal
  virtual void setFromSignalFlag( const LHCb::MCParticle* mcPart ) = 0;

protected:

private:

};
#endif // IFLAGSIGNALCHAIN_H
