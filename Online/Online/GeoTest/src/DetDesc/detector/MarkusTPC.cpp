#include "DetDesc/detector/ILDExTPC.h"
#include "DetDesc/detector/MarkusTPC.h"

namespace DetDesc {
  double MarkusTPC::feature() const {
    ILDExTPC tpc = *this;
    return tpc.getWeight()*tpc.getVolume();
  }
}
