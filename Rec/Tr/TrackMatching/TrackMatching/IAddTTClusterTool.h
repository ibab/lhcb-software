// $Id: IAddTTClusterTool.h,v 1.1 2006-05-15 13:45:28 jvantilb Exp $
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

class IAddTTClusterTool : virtual public IAlgTool {
  
public: 
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IAddTTClusterTool; }

  /// Add TT clusters to matched tracks
  virtual StatusCode addTTClusters( LHCb::Track& track ) = 0;

};
#endif // TRACKMATCHING_IADDTTCLUSTERTOOL_H
