
#ifndef LESTER_CIRCLE_PARAMS_H
#define LESTER_CIRCLE_PARAMS_H

#include "CircleParams.fwd"

#include "CLHEP/Vector/TwoVector.h"
#include "opengl/BasicCanvasInterface.fwd"
#include "GenericRingFinder/GenericRing.fwd"
#include <iostream>

namespace Lester {

  class CircleParams {
  public:
    /// returns +ve dist if point p is outside the circle
    double signedDistanceTo(const Hep2Vector & p) const;
    double distanceSqTo(const Hep2Vector & p) const;
    double distanceTo(const Hep2Vector & p) const;
    double priorProbability() const;
    void setMuBasedOnRadiusAndCentre();
    bool operator==(const CircleParams & other) const;
  private:
    static int _creationCount;
    Hep2Vector _centre;
    double _radius;
    double _mu; // poisson mu parameter for hits
  public:
    static void statusReport();
    inline const Hep2Vector & centre() const;
    inline double mu() const;
    inline double radius() const;
    CircleParams();
    CircleParams(const Hep2Vector & cen, const double r);
    CircleParams(const GenRingF::GenericRing & circle);
    std::ostream & printMeTo(std::ostream & os) const;
#ifdef LESTER_USE_GRAPHICS
    void draw(BasicCanvasInterface & canvas) const;
#endif
    CircleParams jitterSymm1() const;
    void jokeSetRandom();
  };
};

std::ostream & operator<<(std::ostream & os, const Lester::CircleParams & cp);

#endif
