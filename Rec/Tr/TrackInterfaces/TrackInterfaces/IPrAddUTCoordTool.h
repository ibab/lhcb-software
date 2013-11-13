// $Id: IAddTTClusterTool.h,v 1.6 2010-02-09 08:28:04 decianm Exp $
#ifndef TRACKINTERFACES_IPRADDUTCOORDTOOL_H
#define TRACKINTERFACES_IPRADDUTCOORDTOOL_H 1

// Include files
// -------------
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IPrAddUTCoordTool ( "IPrAddUTCoordTool", 1, 0 );

/** @class IPrAddUTCoordTool IPrAddUTCoordTool.h TrackInterfaces/IPrAddUTCoordTool.h
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


class IPrAddUTCoordTool : virtual public IAlgTool {
  
public: 
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPrAddUTCoordTool; }

  /// Add UT clusters to matched tracks
  virtual StatusCode addUTClusters( LHCb::Track& track ) = 0;
  virtual StatusCode returnUTClusters( LHCb::State& state, std::vector<PrUTHit*>& utHits, double& finalChi2, double p) = 0;
      

};
#endif // TRACKINTERFACES_IPRADDUTCOORDTOOL_H
