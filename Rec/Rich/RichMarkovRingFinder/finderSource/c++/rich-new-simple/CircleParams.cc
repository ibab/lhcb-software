#include "CircleParams.h"
#include "Constants.h"
#include "ProbabilityUtils.h"
#include "CirclePriors.h"
#include "GraphicsObjects.h"
#include "GenericRingFinder/GenericRing.h"

namespace Lester {

  int CircleParams::_creationCount = 0;

  /// returns +ve dist if point p is outside the circle
  double CircleParams::signedDistanceTo(const Hep2Vector & p) const {
    return (p-_centre).mag()-_radius;
  }

  double CircleParams::distanceSqTo(const Hep2Vector & p) const {
    const double d= signedDistanceTo(p);
    return d*d;
  }

  double CircleParams::distanceTo(const Hep2Vector & p) const {
    const double d = signedDistanceTo(p);
    return ((d>=0) ? d : -d);
  }

  double CircleParams::priorProbability() const {
    const Hep2Vector c=centre();
    const double r=radius();
    //const double mu=c.mu();

    const double part1 = 
      CirclePriors::priorProbabilityOfCentre(c);
    const double part2 =
      CirclePriors::priorProbabilityOfRadius(r);

    //std::cout << "JORR = " << part1 << " " <<part2<<std::endl;
    const double ans = part1*part2;
    //ans *= exponentialProb(mu,circleMuMuParameter);

    return ans;

  }

  void CircleParams::setMuBasedOnRadiusAndCentre() {
    const double circumference = MathsConstants::pi*radius();
    _mu = circumference * Constants::circleHitsPerUnitLengthParameter;
  }

  bool CircleParams::operator==(const CircleParams & other) const {
    return _centre==other.centre() && 
      _radius==other.radius() &&
      _mu==other.mu();
  }

  void CircleParams::statusReport() {
    assert(std::cout << "CircleParams reports " << _creationCount<< " constructions." << std::endl);
  }

  inline const Hep2Vector & CircleParams::centre() const {
    return _centre;
  }

  inline double CircleParams::mu() const {
    return _mu;
  }

  inline double CircleParams::radius() const {
    return _radius;
  }

  CircleParams::CircleParams() :
    _centre(),
    _radius(0) {
    // Default constructor!
    setMuBasedOnRadiusAndCentre();
    assert(++_creationCount);
  }

  CircleParams::CircleParams(const Hep2Vector & cen, const double r) :
    _centre(cen),
    _radius(r) {
    setMuBasedOnRadiusAndCentre();
    assert(++_creationCount);
  }

  CircleParams::CircleParams(const GenRingF::GenericRing & circle) :
    _centre(circle.x(), circle.y()),
    _radius(circle.radius()) {
    setMuBasedOnRadiusAndCentre();
    assert(++_creationCount);
  }

  std::ostream & CircleParams::printMeTo(std::ostream & os) const {
    return os << "CircleParams[cen="<<centre()<<", r="<<radius()<<", mu="<<mu()<<"]";
  }

#ifdef LESTER_USE_GRAPHICS
  void CircleParams::draw(BasicCanvasInterface & canvas) const {
    canvas.circle(centre().x(), centre().y(), radius());
  }
#endif

  CircleParams CircleParams::jitterSymm1() const {
    const Hep2Vector newcen = centre()+Hep2Vector(0.1*Constants::whiffWhaff*RandGauss::shoot(),0.1*Constants::whiffWhaff*RandGauss::shoot());
    const double trialnewrad = (radius()+0.1*Constants::whiffWhaff*RandGauss::shoot());
    const double newrad = ((trialnewrad>0)?trialnewrad:radius());
    //const double newmu = fabs(mu+0.1*RandGauss::shoot());
    return CircleParams(newcen,newrad);
  }

  void CircleParams::jokeSetRandom() {
    _centre = CirclePriors::sampleFromCircleCentreDistribution();
    //mu=RandExponential::shoot(circleMuMuParameter);
    _radius = CirclePriors::sampleFromCircleRadiusDistribution();
    setMuBasedOnRadiusAndCentre();
  }

}

std::ostream & operator<<(std::ostream & os, const Lester::CircleParams & cp) {
  return cp.printMeTo(os);
}

