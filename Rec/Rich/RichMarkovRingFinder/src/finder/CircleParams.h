#ifndef RICHMARKOV_CIRCLE_PARAMS_H
#define RICHMARKOV_CIRCLE_PARAMS_H

#include <cassert>
#include <iostream>
#include "MathsConstants/MathsConstants.h"
#include "Constants.h"
#include "CLHEP/Vector/TwoVector.h"


namespace RichMarkov {
  class Canvas;  // fwd decs
}
#include "CirclePriors.fwd"

namespace RichMarkov {

  template <class Mode, const Mode & mode>
  class CircleParams {

  public:
    typedef CirclePriors<Mode,mode> CirclePriorsT;

  public:

    double priorProbability() const;

    static inline const double
    calculateMeanNumberOfHitsPerUnitLengthBasedOnRadius(const double r)
    {
      // calculate"mean number of hits per unit length" for a circle of radius r.
      // (nothing to do with per unit length!)
      const double twopi = MathsConstants::twopi;
      const double ans = mode.circleHitsParameter() * r /twopi;
      return ans;
    };

    static inline const double
    calculateMeanNumberOfHitsBasedOnRadius(const double r)
    {
      // calculate poisson mu "mean number of hits" for a circle of radius r.
      // (nothing to do with per unit length!)
      const double ans = mode.circleHitsParameter() * r * r;
      return ans;
    };

  private:
    void internallySetMeanNumberOfHitsGeometricallyCorrected();

  public:
    inline void setMeanNumberOfHitsBasedOnRadiusAndCentre() {
      //       const double circumference = Constants::pi*radius();
      // _mu = circumference * Constants::circleHitsPerUnitLengthParameter;
      _mu = calculateMeanNumberOfHitsBasedOnRadius(_radius);
      internallySetMeanNumberOfHitsGeometricallyCorrected();
    };

    bool operator == ( const CircleParams & other) const {
      return ( _centre                   == other._centre &&
               _radius                   == other._radius &&
               _mu                       == other._mu &&
               _muGeometricallyCorrected == other._muGeometricallyCorrected );
    };

  private:

    static int _creationCount;
    Hep2Vector _centre;
    double _radius;
    double _mu; // poisson mu parameter for hits on one circle
    double _muGeometricallyCorrected; // poisson mu parameter for hits on one circle

  public:
    static void statusReport() {
      assert(std::cout << "CircleParams reports " << _creationCount<< " constructions." << std::endl);
    };
    inline const Hep2Vector & centre() const {
      return _centre;
    };
    inline double meanNumberOfHits() const {
      return _mu;
    };
    inline double meanNumberOfHitsGeometricallyCorrected() const {
      return _muGeometricallyCorrected;
    };
    inline double radius() const {
      return _radius;
    };
    CircleParams() :
      _centre(),
      _radius(0) {
      // Default constructor!
      setMeanNumberOfHitsBasedOnRadiusAndCentre();
      assert(++_creationCount);
    };
    CircleParams(const Hep2Vector & cen, const double r) :
      _centre(cen),
      _radius(r) {
      setMeanNumberOfHitsBasedOnRadiusAndCentre();
      assert(++_creationCount);
    };
    std::ostream & printMeTo(std::ostream & os) const {
      return os << "CircleParams[cen="<<centre()
                <<", r="<<radius()
                <<", mu="<<meanNumberOfHits()
                <<", muGeoAcc="<<meanNumberOfHitsGeometricallyCorrected()
                <<"]";
    };
    void draw(Canvas & canvas) const;
    CircleParams jitterSymm1() const;
    void jokeSetRandom();
  };
}

template<class Mode, const Mode & mode>
inline std::ostream & operator<<(std::ostream & os, const RichMarkov::CircleParams<Mode, mode> & cp) {
  return cp.printMeTo(os);
};

#include "CircleParams.tcc"

#endif
