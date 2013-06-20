// $Id: IAddTTClusterTool.h,v 1.6 2010-02-09 08:28:04 decianm Exp $
#ifndef TRACKINTERFACES_IADDTTCLUSTERTOOL_H
#define TRACKINTERFACES_IADDTTCLUSTERTOOL_H 1

// Include files
// -------------
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IAddTTClusterTool ( "IAddTTClusterTool", 1, 0 );

/** @class IAddTTClusterTool IAddTTClusterTool.h TrackInterfaces/IAddClusterTool.h
 *
 *  @author:  Jeroen van Tilburg Jeroen.van.Tilburg@cern.nl
 *  @auhtor:  Michel De Cian (removed obsolete pure virtual functions)	
 *  @date:    04-02-2010
 */

// forward declaration
namespace LHCb {
  class Track;
  class State;
  class STCluster;
  
}
class PatTTHit;


class IAddTTClusterTool : virtual public IAlgTool {
  
public: 
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IAddTTClusterTool; }

  /// Add TT clusters to matched tracks
  virtual StatusCode addTTClusters( LHCb::Track& track ) = 0;
  virtual StatusCode returnTTClusters( LHCb::State& state, std::vector<PatTTHit*>& ttHits, double& finalChi2, double p) = 0;

  virtual StatusCode addTTClusters( LHCb::Track& track, 
                                    std::vector<LHCb::STCluster*>& ttClusters,
                                    std::vector<double>& ttChi2s ) = 0;

  virtual double distanceToStrip( const LHCb::Track& track, 
                                  const LHCb::STCluster& ttCluster ) = 0;
  
  

};
#endif // TRACKINTERFACES_IADDTTCLUSTERTOOL_H
