// $Id: IPVFitter.h,v 1.2 2008-02-22 10:09:49 cattanem Exp $
#ifndef IPVFITTER_H 
#define IPVFITTER_H 1

// from STL
#include <vector>

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/Point3DTypes.h"

namespace LHCb {
  class Track;
  class RecVertex;
}

// Interface ID 
static const InterfaceID IID_IPVFitter("IPVFitter",1,0);

class IPVFitter : virtual public IAlgTool {
 public:
  // Retrieve interface ID
  static const InterfaceID& interfaceID() {return IID_IPVFitter;}

  // Fit
  virtual StatusCode fitVertex(const Gaudi::XYZPoint seedPoint, 
			       std::vector<const LHCb::Track*>& tracks,
                               LHCb::RecVertex& vtx, 
                               std::vector<const LHCb::Track*>& tracks2remove) = 0;

};
#endif // IPVFITTER_H
