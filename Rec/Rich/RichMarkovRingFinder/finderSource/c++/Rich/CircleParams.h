
#ifndef LESTER_CIRCLE_PARAMS_H
#define LESTER_CIRCLE_PARAMS_H

#include <iostream>

#include "CircleParams.fwd"
#include "Small2Vector.h"
#include "GenericRingFinder/GenericRing.fwd"

namespace Lester {

  class CircleParams 
  {
  public:
    /// returns +ve dist if point p is outside the circle
    double signedDistanceTo(const Small2Vector & p) const;
    double distanceSqTo(const Small2Vector & p) const;
    double distanceTo(const Small2Vector & p) const;
    //double priorProbability() const;
    //void setMuBasedOnRadiusAndCentre();
  private:
    static int _creationCount;
    Small2Vector _centre;
    double _radius;
    //double _mu; // poisson mu parameter for hits
  public:
    static void statusReport();
    const Small2Vector & centre() const;
    //double mu() const;
    double radius() const;
    CircleParams();
    CircleParams(const Small2Vector & cen, const double r);
    CircleParams(const GenRingF::GenericRing & circle);
    std::ostream & printMeTo(std::ostream & os) const;
    CircleParams jitterSymm1() const;
    friend inline std::ostream & operator<<(std::ostream & os, const Lester::CircleParams & cp)
    {
      return cp.printMeTo(os);
    }
  };
};


#endif
