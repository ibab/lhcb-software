
#ifndef RICHMARKOV_CIRCLE_THEOREMS_H
#define RICHMARKOV_CIRCLE_THEOREMS_H

#include "CLHEP/Vector/ThreeVector.h"
#include "finder/FinderException.h"

namespace RichMarkov {

  class CircleTheorems {
  public:
    class PointAtInfinity : public FinderException {
    };
    class RadiusIsInfinite : public FinderException {
    };
  public:
    inline static double twoVectorCross(const Hep2Vector & a,
                                        const Hep2Vector & b) {
      // Returns would-be z-cpt of cross product if the 2-vecs
      // were extended to 3-vecs
      return a.x()*b.y()-a.y()*b.x();
    };
  private:
    inline static double fNumerator(const Hep2Vector & a,
                                    const Hep2Vector & b,
                                    const Hep2Vector & c,
                                    const double aSq,
                                    const double bSq,
                                    const double cSq) {

      // Thanks to Robert (Bob) Yunken (Pen.State.U / U.Western.Australia / Perth) for lots of help with this routine!

      //      const double cSq = c.mag2();
      //const double bSq = b.mag2();
      //const double aSq = a.mag2();

      const double two = 2;
      const Hep2Vector cMinusB = c-b;

      const double top1 = (bSq+cSq)*(c.dot(b)) - two*bSq*cSq;
      const double top2 = aSq* ( cMinusB.mag2() );
      const double top3 = (cSq-bSq)*( a.dot(cMinusB) );

      const double top = top1+top2-top3;

      return top;
    };
    inline static double fDenominator(const Hep2Vector & a,
                                      const Hep2Vector & b,
                                      const Hep2Vector & c) {

      // Thanks to Robert (Bob) Yunken (Pen.State.U / U.Western.Australia / Perth) for lots of help with this routine!
      const double two = 2;
      const double part = twoVectorCross(a,b)+twoVectorCross(b,c)+twoVectorCross(c,a);
      const double partSq = part*part;
      const double bot = two*partSq;
      return bot;
    };
    inline static double fDumb(const Hep3Vector & a,
                               const Hep3Vector & b,
                               const Hep3Vector & c) {

      // Thanks to Robert (Bob) Yunken (Pen.State.U / U.Western.Australia / Perth) for lots of help with this routine!

      const double cSq = c.mag2();
      const double bSq = b.mag2();
      const double aSq = a.mag2();
      const double two = 2;
      const Hep3Vector cMinusB = c-b;

      const double top1 = (bSq+cSq)*(c.dot(b)) - two*bSq*cSq;
      const double top2 = aSq* ( cMinusB.mag2() );
      const double top3 = (cSq-bSq)*( a.dot(cMinusB) );

      const double top = top1+top2-top3;

      const double bot = two*( (a.cross(b)+b.cross(c)+c.cross(a) ).mag2() );

      return top/bot;
    };
  public:
    inline static Hep3Vector dumbCentreOfCircleThrough(const Hep3Vector & a,
                                                       const Hep3Vector & b,
                                                       const Hep3Vector & c) {
      return fDumb(a,b,c)*a + fDumb(b,c,a)*b + fDumb(c,a,b)*c;
    };
    inline static Hep2Vector centreOfCircleThrough(const Hep2Vector & a,
                                                   const Hep2Vector & b,
                                                   const Hep2Vector & c) {
      const double aSq = a.mag2();
      const double bSq = b.mag2();
      const double cSq = c.mag2();

      const Hep2Vector numerator =  fNumerator(a,b,c,aSq,bSq,cSq)*a + fNumerator(b,c,a,bSq,cSq,aSq)*b + fNumerator(c,a,b,cSq,aSq,bSq)*c ;
      const double denominator = fDenominator(a,b,c);

      if (denominator==0) {
        //std::cerr << "Dumpolds drat" << std::endl;
        throw PointAtInfinity();
      };

      return numerator/denominator;
    };
    inline static double radiusOfCircleThrough(const Hep2Vector & a,
                                               const Hep2Vector & b,
                                               const Hep2Vector & c) {
      static bool first=true;
      if (first) {
        first = false;
        std::cerr << "The method CircleTheorems::radiusOfCircleThrough(...) could be much improved!" << std::endl;
      };
      try {
        const Hep2Vector centre = centreOfCircleThrough(a,b,c);
        return (a-centre).mag();
      } catch (PointAtInfinity ) {
        throw RadiusIsInfinite();
      };
    };

  };

}

#endif
