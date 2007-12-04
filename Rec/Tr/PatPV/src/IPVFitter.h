// $Id: IPVFitter.h,v 1.1 2007-12-04 08:46:52 witekma Exp $
#ifndef IPVFITTER_H 
#define IPVFITTER_H 1

// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

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
			       LHCb::RecVertex& vtx) = 0;

};
#endif // IPVFITTER_H
