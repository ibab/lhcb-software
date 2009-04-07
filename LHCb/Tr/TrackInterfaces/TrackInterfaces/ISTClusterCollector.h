// $Id: ISTClusterCollector.h,v 1.1 2009-04-07 07:12:14 mneedham Exp $
#ifndef ISTCLUSTERCOLLECTOR_H
#define ISTCLUSTERCOLLECTOR_H 1

#include "GaudiKernel/IAlgTool.h"

/** @class ISTClusterCollector ISTClusterCollector.h
 *
 *  Interface Class for collecting STClusters around a track
 *
 *  @author M.Needham
 *  @date   27/3/2009
 */

#include <vector>

namespace LHCb{
  class STCluster;
  class Track;
}

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_ISTClusterCollector("ISTClusterCollector",0 ,0);

class ISTClusterCollector : virtual public IAlgTool {

public: 

  typedef struct {
    LHCb::STCluster* cluster;
    double residual;   
  } Hit;
  
  typedef std::vector<Hit> Hits; 

   /// Static access to interface id
  static const InterfaceID& interfaceID() { return IID_ISTClusterCollector; }
   
  /// collect the hits
  virtual StatusCode execute(const LHCb::Track& track,
                             Hits& outputCont) const = 0; 

};

#endif // ISTCLUSTERCOLLECTOR_H
