#ifndef TRACKINTERFACES_IHLTV0UPGRADE_H 
#define TRACKINTERFACES_IHLTV0UPGRADE_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward declarations
namespace LHCb {
 class TwoProngVertex;
}

static const InterfaceID IID_IHltV0Upgrade ( "IHltV0Upgrade", 1, 0 );

/** @class IHltV0Upgrade IHltV0Upgrade.h TrackInterfaces/IHltV0Upgrade.h
 *  
 *
 *  @author Jaap Panman
 *  @date   2008-03-04
 */
class IHltV0Upgrade : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IHltV0Upgrade; }

  // Estimate two prong vertex after refitting tracks using TrackTraj
  virtual StatusCode process( LHCb::TwoProngVertex& vertex ) const = 0;

};
#endif // TRACKINTERFACES_IHLTV0UPGRADE_H 
