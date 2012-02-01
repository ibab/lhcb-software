#include "DetDesc/compact/Detector.h"

namespace DetDesc {

  struct TPCData;

  struct GearTPC : public Geometry::Subdetector {
    typedef TPCData Object;
    GearTPC(const Geometry::RefElement_type<TNamed>& e);
    double innerRadius() const;
    double outerRadius() const;
    double pressure() const;
  };
}
