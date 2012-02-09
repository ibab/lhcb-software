// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DetDesc/detector/ILDExTPC.h"
#include "DetDesc/detector/GearTPC.h"
#include "DetDesc/lcdd/Volumes.h"
#include "DetDesc/lcdd/Shapes.h"
#include "TGeoTube.h"

using namespace std;

namespace DetDesc {
  using namespace Geometry;

  GearTPC::GearTPC(const Ref_t& e) : Geometry::DetElement(e) {
  }

  double GearTPC::innerRadius() const {
    DetElement gas   = data<Object>()->gas;
    Tube       tube  = gas.placement().volume().solid();
    return tube->GetRmin();
  }
  double GearTPC::outerRadius() const {
    DetElement gas   = data<Object>()->gas;
    Tube       tube  = gas.placement().volume().solid();
    return tube->GetRmax();
  }
  double GearTPC::pressure() const {
    return data<Object>()->pressure;
  }
}
