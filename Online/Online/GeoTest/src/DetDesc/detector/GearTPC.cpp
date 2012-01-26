#include "DetDesc/detector/ILDExTPC.h"
#include "DetDesc/detector/GearTPC.h"
#include "DetDesc/lcdd/Volumes.h"
#include "DetDesc/lcdd/Shapes.h"
#include "DetDesc/Internals.h"
#include "TGeoTube.h"

using namespace std;

namespace DetDesc {
  using namespace Geometry;

  GearTPC::GearTPC(const RefElement& e) : Geometry::Subdetector(e) {
    verifyObject<Value<TNamed,Object> >();
  }

  double GearTPC::innerRadius() const {
    Subdetector gas   = getAttr(gas);
    TGeoTube&   tube  = (TGeoTube&)gas.volume().solid().shape();
    return tube.GetRmin();
  }
  double GearTPC::outerRadius() const {
    Subdetector gas   = getAttr(gas);
    TGeoTube&   tube  = (TGeoTube&)gas.volume().solid().shape();
    return tube.GetRmax();
  }
  double GearTPC::pressure() const {
    return getAttr(pressure);
  }
}
