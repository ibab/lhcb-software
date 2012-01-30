#include "DetDesc/compact/Detector.h"

namespace DetDesc {

  struct TPCData;

  struct GearTPC : public Geometry::Subdetector {
    typedef TPCData Object;
    GearTPC(const Geometry::RefElement& e);
    double innerRadius() const;
    double outerRadius() const;
    double pressure() const;
  };
}
