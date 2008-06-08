
#include "CircleTheorems.h"
#include "Small2Vector.h"
#include "Small3Vector.h"
#include "Utils/MessageHandler.h"

namespace Lester {

  double CircleTheorems::twoVectorCross(const Small2Vector & a,
                                        const Small2Vector & b) {
    // Returns would-be z-cpt of cross product if the 2-vecs
    // were extended to 3-vecs
    return a.x()*b.y()-a.y()*b.x();
  }

  double CircleTheorems::fNumerator(const Small2Vector & a,
                                    const Small2Vector & b,
                                    const Small2Vector & c,
                                    const double aSq,
                                    const double bSq,
                                    const double cSq) {

    // Thanks to Robert (Bob) Yunken (Pen.State.U / U.Western.Australia / Perth) for lots of help with this routine!

    //      const double cSq = c.mag2();
    //const double bSq = b.mag2();
    //const double aSq = a.mag2();

    const double two = 2;
    const Small2Vector cMinusB = c-b;

    const double top1 = (bSq+cSq)*(c.dot(b)) - two*bSq*cSq;
    const double top2 = aSq* ( cMinusB.mag2() );
    const double top3 = (cSq-bSq)*( a.dot(cMinusB) );

    const double top = top1+top2-top3;

    return top;
  }

  double CircleTheorems::fDenominator(const Small2Vector & a,
                                      const Small2Vector & b,
                                      const Small2Vector & c) {

    // Thanks to Robert (Bob) Yunken (Pen.State.U / U.Western.Australia / Perth) for lots of help with this routine!
    const double two = 2;
    const double part = twoVectorCross(a,b)+twoVectorCross(b,c)+twoVectorCross(c,a);
    const double partSq = part*part;
    const double bot = two*partSq;
    return bot;
  }

  double CircleTheorems::fDumb(const Small3Vector & a,
                               const Small3Vector & b,
                               const Small3Vector & c) {

    // Thanks to Robert (Bob) Yunken (Pen.State.U / U.Western.Australia / Perth) for lots of help with this routine!

    const double cSq = c.mag2();
    const double bSq = b.mag2();
    const double aSq = a.mag2();
    const double two = 2;
    const Small3Vector cMinusB = c-b;

    const double top1 = (bSq+cSq)*(c.dot(b)) - two*bSq*cSq;
    const double top2 = aSq* ( cMinusB.mag2() );
    const double top3 = (cSq-bSq)*( a.dot(cMinusB) );

    const double top = top1+top2-top3;

    const double bot = two*( (a.cross(b)+b.cross(c)+c.cross(a) ).mag2() );

    return top/bot;
  }

  Small3Vector CircleTheorems::dumbCentreOfCircleThrough(const Small3Vector & a,
                                                         const Small3Vector & b,
                                                         const Small3Vector & c) {
    return a*fDumb(a,b,c) + b*fDumb(b,c,a) + c*fDumb(c,a,b);
  }

  Small2Vector CircleTheorems::centreOfCircleThrough(const Small2Vector & a,
                                                     const Small2Vector & b,
                                                     const Small2Vector & c) {
    const double aSq = a.mag2();
    const double bSq = b.mag2();
    const double cSq = c.mag2();

    const Small2Vector numerator =  a*fNumerator(a,b,c,aSq,bSq,cSq) + b*fNumerator(b,c,a,bSq,cSq,aSq) + c*fNumerator(c,a,b,cSq,aSq,bSq) ;
    const double denominator = fDenominator(a,b,c);

    if (denominator==0) {
      throw PointAtInfinity();
    };

    return numerator/denominator;
  }

  double CircleTheorems::radiusOfCircleThrough(const Small2Vector & a,
                                               const Small2Vector & b,
                                               const Small2Vector & c) {
    static bool first=true;
    if (first) {
      first = false;
      Lester::messHandle().debug() << "The method CircleTheorems::radiusOfCircleThrough(...) could be much improved!" << Lester::endmsg;
    };
    try {
      const Small2Vector centre = centreOfCircleThrough(a,b,c);
      return (a-centre).mag();
    } catch (PointAtInfinity pai) {
      throw RadiusIsInfinite();
    }
  }


} // namespace

