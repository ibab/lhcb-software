#include "DetDesc/detector/ILDExTPC.h"
#include "DetDesc/detector/GearTPC.h"
#include "DetDesc/lcdd/Volumes.h"
#include "DetDesc/lcdd/Shapes.h"
#include "TGeoTube.h"

using namespace std;

namespace DetDesc {
  using namespace Geometry;

  GearTPC::GearTPC(const RefElement& e) : Geometry::Subdetector(e) {
  }

  double GearTPC::innerRadius() const {
    Subdetector gas   = data<Object>()->gas;
    Tube        tube  = gas.volume().solid();
    return tube->GetRmin();
  }
  double GearTPC::outerRadius() const {
    Subdetector gas   = data<Object>()->gas;
    Tube        tube  = gas.volume().solid();
    return tube->GetRmax();
  }
  double GearTPC::pressure() const {
    return data<Object>()->pressure;
  }
}
