#include "CircleParams.h"
#include "Constants.h"
#include "ProbabilityUtils.h"
#include "GenericRingFinder/GenericRing.h"
#include "CLHEP/Random/RandGauss.h"
#include "Utils/MessageHandler.h"

namespace Lester 
{

  int CircleParams::_creationCount = 0;

  /// returns +ve dist if point p is outside the circle
  double CircleParams::signedDistanceTo(const Small2Vector & p) const {
    return (p-_centre).mag()-_radius;
  }

  double CircleParams::distanceSqTo(const Small2Vector & p) const {
    const double d= signedDistanceTo(p);
    return d*d;
  }

  double CircleParams::distanceTo(const Small2Vector & p) const {
    const double d = signedDistanceTo(p);
    return ((d>=0) ? d : -d);
  }

  void CircleParams::statusReport() {
    Lester::messHandle().debug() << "CircleParams reports " 
                                 << _creationCount<< " constructions." << Lester::endmsg;
  }

  const Small2Vector & CircleParams::centre() const {
    return _centre;
  }

  double CircleParams::radius() const {
    return _radius;
  }

  CircleParams::CircleParams() :
    _centre(0,0),
    _radius(0) {
    // Default constructor!
    //setMuBasedOnRadiusAndCentre();
    assert(++_creationCount);
  }

  CircleParams::CircleParams(const Small2Vector & cen, const double r) :
    _centre(cen),
    _radius(r) {
    //setMuBasedOnRadiusAndCentre();
    assert(++_creationCount);
  }

  CircleParams::CircleParams(const GenRingF::GenericRing & circle) :
    _centre(circle.x(), circle.y()),
    _radius(circle.radius()) {
    //setMuBasedOnRadiusAndCentre();
    assert(++_creationCount);
  }

  std::ostream & CircleParams::printMeTo(std::ostream & os) const 
  {
    return os << "CircleParams[cen="<<centre()<<", r="<<radius()/*<<", mu="<<mu()*/ <<"]";
  }

  CircleParams CircleParams::jitterSymm1() const
  {
    const Small2Vector newcen = centre()+Small2Vector(0.1*Constants::whiffWhaff*RandGauss::shoot(),0.1*Constants::whiffWhaff*RandGauss::shoot());
    const double trialnewrad = (radius()+0.1*Constants::whiffWhaff*RandGauss::shoot());
    const double newrad = ((trialnewrad>0)?trialnewrad:radius());
    return CircleParams(newcen,newrad);
  }

}
