#ifndef IPVFITTER_H
#define IPVFITTER_H 1

// from STL
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Point3DTypes.h"

namespace LHCb {
  class Track;
  class RecVertex;
}

// Interface ID

class IPVFitter : public extend_interfaces<IAlgTool> {
 public:
  DeclareInterfaceID(IPVFitter,2,0);

  // Fit
  virtual StatusCode fitVertex(const Gaudi::XYZPoint& seedPoint,
			                   const std::vector<const LHCb::Track*>& tracks,
                               LHCb::RecVertex& vtx,
                               std::vector<const LHCb::Track*>& tracks2remove) const = 0;

};
#endif // IPVFITTER_H
