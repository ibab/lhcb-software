
#include "OTDet/WalkRelation.h"

#include <math.h>

namespace OTDet
{
  WalkRelation::WalkRelation(): off(0), amp(0), tau(0), dpt(0)
  {
  }

  WalkRelation::WalkRelation(double off, double amp, double tau, double dpt)
  {
    WalkRelation::off = off;
    WalkRelation::amp = amp;
    WalkRelation::tau = tau;
    WalkRelation::dpt = dpt;
  }

  double WalkRelation::walk(double l) const
  {
    if(tau == 0) return off;
    if(l < 0) l = 0;
    return off + amp * (tanh(l / tau) - 1.0) + dpt * l / 1000.0;
  }
}
