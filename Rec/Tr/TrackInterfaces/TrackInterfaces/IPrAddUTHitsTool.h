#ifndef TRACKINTERFACES_IPRADDUTHITSTOOL_H
#define TRACKINTERFACES_IPRADDUTHITSTOOL_H 1

// Include files
// -------------
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"


/** @class IPrAddUTHitsTool IPrAddUTHitsTool.h TrackInterfaces/IPrAddUTHitsTool.h
 *
 *  @author:  Michel De Cian	
 *  @date:    13-11-2013
 */

// forward declaration
namespace LHCb {
  class Track;
  class State;
}

class PrUTHit;

class IPrAddUTHitsTool : public extend_interfaces<IAlgTool> {
public: 
  DeclareInterfaceID( IPrAddUTHitsTool, 2, 0 );

  /// Add UT clusters to matched tracks
  virtual StatusCode addUTHits( LHCb::Track& track ) const = 0;
  virtual StatusCode returnUTHits( LHCb::State& state, std::vector<PrUTHit*>& utHits, double& finalChi2, double p) const = 0;
      

};
#endif // TRACKINTERFACES_IPRADDUTHITSTOOL_H
