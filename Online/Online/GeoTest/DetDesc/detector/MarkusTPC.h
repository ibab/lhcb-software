#include "DetDesc/compact/Detector.h"

namespace DetDesc {

  struct TPCData;

  struct MarkusTPC : public Geometry::Subdetector {
    typedef TPCData Object;
    MarkusTPC(const RefElement& e) : Geometry::Subdetector(e) {}
    double feature() const;
  };
}
