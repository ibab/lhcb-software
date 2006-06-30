// $Id: IAddTTClusterTool.h,v 1.2 2006-06-30 14:03:59 cattanem Exp $
#ifndef TRACKMATCHING_IADDTTCLUSTERTOOL_H
#define TRACKMATCHING_IADDTTCLUSTERTOOL_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IAddTTClusterTool ( "IAddTTClusterTool", 1, 0 );

/** @class IAddTTClusterTool IAddTTClusterTool.h  
 *
 *  @author:  Jeroen van Tilburg Jeroen.van.Tilburg@cern.nl
 *  @date:    12-05-2006
 */

// forward declaration
namespace LHCb {
  class STCluster;
  class Track;
}

class IAddTTClusterTool : virtual public IAlgTool {
  
public: 
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IAddTTClusterTool; }

  /// Add TT clusters to matched tracks
  virtual StatusCode addTTClusters( LHCb::Track& track ) = 0;

  /// Add TT clusters to matched tracks
  virtual StatusCode addTTClusters( LHCb::Track& track, 
                                    std::vector<LHCb::STCluster*>& ttClusters,
                                    std::vector<double>& ttChi2s ) = 0;

  /// Get the distance to the strip
  virtual double distanceToStrip( const LHCb::Track& track, 
                                  const LHCb::STCluster& ttCluster ) = 0;

};
#endif // TRACKMATCHING_IADDTTCLUSTERTOOL_H
